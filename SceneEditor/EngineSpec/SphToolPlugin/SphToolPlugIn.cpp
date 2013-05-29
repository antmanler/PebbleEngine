// ****************************************************************************************
//
// Pebble Scene Editor 
// --------------------------------------
// Copyright (C) 2011 Antmanler
// 
// This file is part of the Pebble Scene Editor.
//
// The Pebble Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License 
//
// The Pebble Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Credits : Volker Wiendl who created the major work of the original editor and Horde3D
// is a flexible graphics engine. see <http://www.horde3d.org> and <http://http://goo.gl/Me2HT>
//
// ****************************************************************************************
#include <QApplication>

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QProgressDialog>

#include <QtCore/QTextStream>
#include <QtGui/QImageReader>
#include <QtGui/QWizard>
#include <QtCore/QMap>

#include <map>
#include <vector>
#include <math.h>
#include <Horde3D/Horde3D.h>
#include <Horde3D/Horde3DUtils.h>
#include <Horde3D/Horde3DSph.h>
#include <Horde3D/Horde3DTerrain.h>

#include <SceneEditorCore/SceneFile.h>
#include <SceneEditorCore/EditorService.h>
#include <SceneEditorCore/EditorUtils.h>

#include <dflib/dfutils.h>
#include <dflib/matrix4x4f.h>
#include <dflib/distancefield3d.h>
#include <dflib/particles.h>

#include "GeneratorWizard.h"
#include "SPHScnSDFPage.h"
#include "SPHScnVarPage.h"

#include "dfWidget.h"
#include "SphToolPlugIn.h"

namespace {

static void update_bbox( const int hid, vector3f &bbmin, vector3f &bbmax) {

    if ( H3DNodeTypes::Group ==  h3dGetNodeType(hid)) {

        int nodes = h3dFindNodes(hid, "", H3DNodeTypes::Undefined );
        vector<int> find_ret; find_ret.reserve(nodes);
        for (int i = 0; i < nodes; ++i) find_ret.push_back(h3dGetNodeFindResult(i));

        for (int i = 0; i < nodes; ++i) {
            if ( h3dGetNodeParent(find_ret[i]) == hid)
                update_bbox(find_ret[i], bbmin, bbmax);
        }

    } else {
        float minx, miny, minz, maxx, maxy, maxz;
        h3dGetNodeAABB(hid, &minx, &miny, &minz, &maxx, &maxy, &maxz);
        bbmin = bbmin.vmin(vector3f(minx, miny, minz));
        bbmax = bbmax.vmax(vector3f(maxx, maxy, maxz));
    }

}

} // namespace

SphToolPlugIn::SphToolPlugIn(QObject* parent /*= 0*/)
    : QObject(parent)
    , m_scene(0)
    , curr_procdlg_(NULL)
    , curr_dfgen_(NULL)
{
    QAction* dfAction = new QAction(tr("Generate SDF for current scene"), this);
    dfAction->setEnabled(false);
    connect(dfAction, SIGNAL(triggered()), this, SLOT(generateSdfForScene()));
    m_actions.push_back(dfAction);

    dfAction = new QAction(tr("Generate SDF from model"), this);
    dfAction->setEnabled(false);
    connect(dfAction, SIGNAL(triggered()), this, SLOT(generateSdfForModel()));
    m_actions.push_back(dfAction);

    dfAction = new QAction(tr("Generate SPH simulator scene"), this);
    dfAction->setEnabled(false);
    connect(dfAction, SIGNAL(triggered()), this, SLOT(generateSphScene()));
    m_actions.push_back(dfAction);

    QAction* dfvAction = new QAction(tr("View SDF file"), this);
    dfvAction->setEnabled(true);
    dfvAction->setData("dfviewer");
    connect(dfvAction, SIGNAL(triggered()), this, SLOT(viewSdf()));
    m_actions.push_back(dfvAction);
}


SphToolPlugIn::~SphToolPlugIn()
{
}

QString SphToolPlugIn::plugInName() const
{
    return "Sph Tools PlugIn";
}

QList<QAction*> SphToolPlugIn::actions() const
{
    return m_actions;
}

void SphToolPlugIn::setSceneFile(SceneFile* scene)
{
    m_scene = scene;
    foreach(QAction* action, m_actions) {
        if (action->data().toString() == "dfviewer") {
            action->setEnabled(true);
            continue;
        }
        action->setEnabled(scene != 0);
    }
}

void SphToolPlugIn::generateSdfForScene()
{
    QDir dfDir = QDir::current();
    QWizard wizard;
    wizard.addPage(new GeneratorWizard(&wizard));
//	wizard.adjustSize();
    if (wizard.exec() == QDialog::Accepted) {

        int nodes = h3dFindNodes(H3DRootNode, "", H3DNodeTypes::Undefined );
        // save the find results locally  because h3dGetNodeFindResult() is not safe
        vector<int> find_ret; find_ret.reserve(nodes);
        for (int i = 0; i < nodes; ++i) find_ret.push_back(h3dGetNodeFindResult(i));

        QVector<int> nodelist;
        // collect model and terrain nodes to list
        for (int i = 0; i<nodes; ++i) {
            // only for active nodes and none editor geometry
			QString nodename((h3dGetNodeParamStr(find_ret[i], H3DNodeParams::NameStr)));
            if ( (h3dGetNodeFlags(find_ret[i]) & H3DNodeFlags::Inactive) || nodename.startsWith("__editor", Qt::CaseInsensitive)) 
				continue;

            switch(h3dGetNodeType(find_ret[i])) {
            case H3DNodeTypes::Mesh:
            case H3DEXT_NodeType_Terrain:
                nodelist.push_back(find_ret[i]);
                break;
            default : break;
            }
        }
        int nbx = wizard.field("nbx").toInt();
        int nby = wizard.field("nby").toInt();
        int nbz = wizard.field("nbz").toInt();
        // start generate df
        QFileInfo dffile(dfDir.absoluteFilePath(wizard.field("target").toString()));
        if (!dffile.absoluteDir().exists()) {
            dfDir.mkpath(dffile.absoluteDir().absolutePath());
        }
        bool ret = generateSdf(dffile.absoluteFilePath(), nodelist, nbx, nby, nbz, wizard.field("nfr").toInt());
        if (ret && wizard.field("viewdf").toBool()) {
            DFWidget *df_viewer = new DFWidget();
            df_viewer->setAttribute(Qt::WA_DeleteOnClose, true);
            df_viewer->setDf(QFileInfo(dffile.absoluteFilePath()));
            df_viewer->show();
        }
    }
}

void SphToolPlugIn::generateSdfForModel()
{
    // select model from repositry
    QString newmodel = EditorService::getModelFile(QDir::currentPath(), 0, tr("Select a SceneGraph file to generate SDF"));

    if (!newmodel.isEmpty()) {
        /// Add model to scene and make it inacitve
        // add resource for model
        H3DRes modres = h3dAddResource( H3DResTypes::SceneGraph, qPrintable(newmodel), 0 );
        // Load data
        if (modres == 0 || !SceneEditor::load_h3dres_from_file_qrc(".")) {
            if( modres ) h3dRemoveResource(modres);
            // release unused resources within Horde3D
            h3dReleaseUnusedResources(H3DResTypes::Undefined);
        }
        // add to scene graph
        H3DNode model = h3dAddNodes( H3DRootNode, modres );
        // make this node inacitve
        h3dSetNodeFlags(model, H3DNodeFlags::Inactive, true);

        // generate SDF
        QDir dfDir = QDir::current();
        QWizard wizard;
        wizard.addPage(new GeneratorWizard(&wizard));
        wizard.setField("target", QString("simdata/%1.df").arg(h3dGetNodeParamStr(model, H3DNodeParams::NameStr)));
//		wizard.adjustSize();
        if (wizard.exec() == QDialog::Accepted) {

            int nodes = h3dFindNodes(model, "", H3DNodeTypes::Mesh );
            // save the find results locally  because h3dGetNodeFindResult() is not safe
            QVector<int> nodelist; nodelist.reserve(nodes);
            for (int i = 0; i < nodes; ++i) nodelist.push_back(h3dGetNodeFindResult(i));

            int nbx = wizard.field("nbx").toInt();
            int nby = wizard.field("nby").toInt();
            int nbz = wizard.field("nbz").toInt();
            // start generate df
            QFileInfo dffile(dfDir.absoluteFilePath(wizard.field("target").toString()));
            if (!dffile.absoluteDir().exists()) {
                dfDir.mkpath(dffile.absoluteDir().absolutePath());
            }
            bool ret = generateSdf(dffile.absoluteFilePath(), nodelist, nbx, nby, nbz, wizard.field("nfr").toInt());
            if (ret && wizard.field("viewdf").toBool()) {
                DFWidget *df_viewer = new DFWidget();
                df_viewer->setAttribute(Qt::WA_DeleteOnClose, true);
                df_viewer->setDf(QFileInfo(dffile.absoluteFilePath()));
                df_viewer->show();
            }
        }

        // remove node and release resources
        h3dRemoveNode(model);
        h3dReleaseUnusedResources(H3DResTypes::Undefined);
    }
}

bool SphToolPlugIn::generateSdf(QString df_file, const QVector<int>& hordelist, int nbx, int nby, int nbz, int nfr)
{
    QProgressDialog dlg(tr("Processing scene nodes..."), tr("Abort"), 0, hordelist.size()+1);
    dlg.setModal(true);
    dlg.setMinimumDuration(0);
    /// First collect mesh from each model
    Mesh mesh;
    mesh.beginGemo();
    QVector<int>::const_iterator itr = hordelist.begin();
    for(int cnt=0; itr!=hordelist.end(); ++itr, ++cnt) {
        dlg.setValue(cnt);
        QApplication::processEvents();
        // process geometry
        dfut::appendH3DGeomToMesh((*itr), mesh);
    }
    mesh.endGeom();

    dlg.setLabelText("Compute normals");
    dlg.setValue(hordelist.size());
    mesh.computeNormals();
    //    mesh.saveMesh(QString(df_file+".mesh").toStdString().c_str());
    //    mesh.save2Obj(QString(df_file+".obj").toStdString().c_str());

    // Second generate df3d
    dlg.setLabelText("Start generating");
    // dlg.setMaximum(dlg.maximum() + nbx*nby*nbz + mesh.triangles.size());
    DistanceField3D generator(nbx, nby, nbz);
    curr_procdlg_ = &dlg;
    curr_dfgen_ = &generator;
    generator.sigNotify()->connect(make_slot(this, &SphToolPlugIn::recvNotify));
    generator.meshToDistanceField(mesh);
    curr_procdlg_ = 0x0;
    curr_dfgen_ = 0x0;
    if (!dlg.wasCanceled()) {
        generator.checkConsistency(nfr);
        generator.writeToFile(df_file.toStdString());
        return true;
    }
    return false;
}

bool SphToolPlugIn::generateSphScene(QString sphconf, QString dffilename,
                                     const QMap<QString, QVector<int> >& fluidmap, const QMap<QString, QVector<int> >& rigidmap,
                                     const float setp_size, const float smooth_length, const vector3f &llower, const float scale)
{
    const int prcess_steps = fluidmap.size() + rigidmap.size() +1;
    QProgressDialog dlg(tr("Processing scene nodes..."), tr("Abort"), 0, prcess_steps);
    dlg.setModal(true);
    dlg.setMinimumDuration(0);

    curr_procdlg_ = &dlg;

    // open sdf file
    QString sdffile(QDir::current().absoluteFilePath(dffilename));
    DistanceField3D sdf(sdffile.toStdString());

    // get conffile info
    QFileInfo conffile(QDir::current().absoluteFilePath(sphconf));
    QFile fr(conffile.absoluteFilePath());
    if (fr.open(QIODevice::WriteOnly)) {
        QTextStream out(&fr);
        out << "step:" << setp_size << endl;

        QMap<QString, QVector<int> >::const_iterator itr = fluidmap.begin();
        // process fluid particle source in current scene
        for(int cnt=0; itr!=fluidmap.end(); ++itr, ++cnt) {
            //            if ( h3dGetNodeType(*itr) != H3DEXT_NodeType_Sph ) continue;
            dlg.setMaximum(prcess_steps);
            dlg.setValue(cnt);
            QApplication::processEvents();
            QString ret = generateParticleConf(conffile, itr.key(), &sdf, itr.value(), smooth_length, llower, scale);
            if (!ret.isEmpty()) out << ret << endl;
            // handle user canceled
            if (dlg.wasCanceled()) break;
        }

        // process rigid particle source in current scene
        itr = rigidmap.begin();
        for(int cnt=0; itr!=rigidmap.end(); ++itr, ++cnt) {
            //            if ( h3dGetNodeType(*itr) != H3DEXT_NodeType_Sph ) continue;
            dlg.setMaximum(prcess_steps);
            dlg.setValue(cnt);
            QApplication::processEvents();
            QString ret = generateParticleConf(conffile, itr.key(), &sdf, itr.value(), smooth_length, llower, scale, true);
            if (!ret.isEmpty()) out << ret << endl;
            // handle user canceled
            if (dlg.wasCanceled()) break;
        }

        out << "df:" << conffile.absoluteDir().relativeFilePath(sdffile) << endl;
        fr.close();
        if (dlg.wasCanceled()) {
            conffile.absoluteDir().remove(conffile.absoluteFilePath());
        }
    }
    dlg.setMaximum(prcess_steps);
    dlg.setValue(prcess_steps - 1);
    curr_procdlg_ = 0x0;
    if (dlg.wasCanceled()) return false;
    return true;
}

QString SphToolPlugIn::generateParticleConf(QFileInfo conffile, QString tag, const DistanceField3D *sdf, const QVector<int>& sphlist, const float smooth_length,
                                            const vector3f& llower, const float scale, const bool is_rigid)
{
    Particles *particles = new Particles();
    float r = FLT_MAX;
    float density = FLT_MAX;

    QVector<int>::const_iterator itr = sphlist.begin();
    // process fluid particle source in current scene
    for(int cnt=0; itr!=sphlist.end(); ++itr, ++cnt) {
        int sphnode = *itr;
        // get fr resources
        int frres = h3dGetNodeParamI(sphnode, H3DEXTSph::VolumeResI);
        if ( 0 == frres ) continue;

        // get fr properties
        int par_num = h3dGetResParamI(frres, H3DEXTSphRes::ParticleElem, 0, H3DEXTSphRes::ParticleNumI);
        float sample_dist = h3dGetResParamF(frres, H3DEXTSphRes::ParticleElem, 0, H3DEXTSphRes::SampleDistanceF, 0);
        float density_2 = h3dGetResParamF(frres, H3DEXTSphRes::ParticleElem, 0, H3DEXTSphRes::DensityF, 0);
        if ( 0 == par_num || 0 == sample_dist || 0 == density_2 ) continue;

        // get particles pointer
        float *points = (float*)h3dMapResStream(frres, H3DEXTSphRes::ParticleElem, 0, 0, true, false);
        h3dUnmapResStream(frres);
        if ( NULL == points ) continue;

        QString node_name ( h3dGetNodeParamStr(sphnode, H3DNodeParams::NameStr) );
        if (curr_procdlg_) {
            curr_procdlg_->setLabelText(QString("Generating particels for %1...").arg(node_name));
            curr_procdlg_->setMaximum(par_num);
            curr_procdlg_->setValue(0);
        }

        const float *m16 = NULL;
        h3dGetNodeTransMats(sphnode, NULL, &m16);
        matrix4x4f mat(m16);
        vector3f bbmin(FLT_MAX, FLT_MAX, FLT_MAX), bbmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        update_bbox(sphnode, bbmin, bbmax);
        vector3f size = bbmax-bbmin;
        float scale2  = size.x;
        if (size.y>scale2) scale2 = size.y;
        if (size.z>scale2) scale2 = size.z;

        float r_2 = smooth_length * sample_dist * scale2 * scale;

        for ( int i = 0; i < par_num; ++i ) {
            if ( 0 == i%500 && curr_procdlg_) {
                curr_procdlg_->setValue(i);
                QApplication::processEvents();
            }
            vector3f parpos;
            memcpy(parpos.v, points + 3*i, sizeof(float)*3);
            mat.transformPoint(&parpos);
            parpos = (parpos-llower)*scale;
            Particle particle(parpos, r_2, density_2);
            if (!constrainParticle(sdf, &particle, sample_dist))
                particles->particles.push_back(particle);
        }

        r = min(r, r_2);
        density = min(density, density_2);
    }

    if (curr_procdlg_) {
        curr_procdlg_->setMaximum(is_rigid ? 3 : 2);
        curr_procdlg_->setValue(1);
        curr_procdlg_->setLabelText(QString("Computing particels mass for %1...").arg(tag));
        curr_procdlg_->setCursor(Qt::BusyCursor);
        QApplication::processEvents();
    }

    particles->computeNeighbors();
    particles->computeParticlesMass();

    QFileInfo par_file(conffile.absoluteDir()
                       .absoluteFilePath(QString("%1_%2.par")
                                         .arg(conffile.baseName())
                                         .arg(tag)));

    particles->writeToFile(par_file.absoluteFilePath().toStdString());

    if (curr_procdlg_) {
        curr_procdlg_->setValue(curr_procdlg_->value() + 1);
        curr_procdlg_->setCursor(Qt::ArrowCursor);
        QApplication::processEvents();
    }

    QString conf_str;
    if (is_rigid) {
        // generate local position
        vector3f mop; // center of mass
        size_t N = particles->particles.size();
        for (size_t i = 0; i<N; ++i) {
            mop = mop + particles->particles.at(i);
        }
        mop = mop / N;
        Particles localpos;
        localpos.particles.reserve(N);
        for (size_t i = 0; i<N; ++i) {
            localpos.particles.push_back(particles->particles.at(i) - mop);
        }

        // save to file
        QFileInfo rigid_par_file(conffile.absoluteDir()
                                 .absoluteFilePath(QString("%1_%2_local.par")
                                                   .arg(conffile.baseName())
                                                   .arg(tag)));

        localpos.writeToFile(rigid_par_file.absoluteFilePath().toStdString());

        conf_str = QString("rigid_smooth_length:%1\n"
                           "rigid_density:%2\n"
                           "rigid_mass:%3\n"
                           "rigid_data:%4\n"
                           "rigid_local:%5")
                   .arg(r).arg(density).arg(particles->template_mass)
                   .arg(conffile.absoluteDir().relativeFilePath(par_file.absoluteFilePath()))
                   .arg(conffile.absoluteDir().relativeFilePath(rigid_par_file.absoluteFilePath()));
    } else {

        conf_str  = QString("smooth_length:%1\n"
                            "density:%2\n"
                            "mass:%3\n"
                            "data:%4")
                    .arg(r).arg(density).arg(particles->template_mass)
                    .arg(conffile.absoluteDir().relativeFilePath(par_file.absoluteFilePath()));
    }

    delete particles;
    return conf_str;
}

bool SphToolPlugIn::constrainParticle(const DistanceField3D *sdf, Particle *particle, const float sample_dist)
{
    vector3f pos = *particle;
    float dist;
    vector3f normal;
    sdf->distance(pos,dist,normal);
    if (dist >= 0) return true;

    // create ghost particle near boundaries
    if (dist < particle->r) {
        particle->ghostpos = pos+(2*dist+sample_dist*0.707f)*normal;
        sdf->distance(particle->ghostpos, dist);
        if ( dist < 0 ) particle->hasghost = true;
    }
    return false;
}

void SphToolPlugIn::recvNotify(const char* stepname, int curr, int steps)
{
    if (curr_procdlg_ && curr_dfgen_ && curr_procdlg_->wasCanceled()) {
        curr_dfgen_->cancel();
    } else if (curr_procdlg_) {
        curr_procdlg_->setLabelText(QString("%1 (%2/%3) ...").arg(stepname).arg(curr).arg(steps));
        curr_procdlg_->setMaximum(steps);
        curr_procdlg_->setValue(curr);
    }
    QApplication::processEvents();
}

void SphToolPlugIn::viewSdf()
{
    QString filter = tr("Distance field files") + QString("(*.df)");
    filter+= tr(";;All files (*.*)");

    QString fileName = QFileDialog::getOpenFileName(0, tr("Select distance field"), QDir::currentPath(), filter);
    if (fileName.isEmpty()) return;

    DFWidget *df_viewer = new DFWidget();
    df_viewer->setAttribute(Qt::WA_DeleteOnClose, true);
    df_viewer->setDf(QFileInfo(fileName));
    df_viewer->show();
}

void SphToolPlugIn::generateSphScene()
{
    QDir dfDir = QDir::current();
    QWizard wizard;
    wizard.addPage(new SPHScnSDFPage(&wizard));
    wizard.addPage(new SPHScnVarPage(&wizard));
//	wizard.adjustSize();
    if (wizard.exec() == QDialog::Accepted) {

        /// 1. processing scene
        int nodes = h3dFindNodes(H3DRootNode, "", H3DNodeTypes::Undefined );
        // save the find results locally  because h3dGetNodeFindResult() is not safe
        vector<int> find_ret; find_ret.reserve(nodes);
        for (int i = 0; i < nodes; ++i) find_ret.push_back(h3dGetNodeFindResult(i));

        QMap<QString, QVector<int> > fluidmap;
        QMap<QString, QVector<int> > rigidmap;
        QVector<int> boundaries;
        vector3f bbmin(FLT_MAX, FLT_MAX, FLT_MAX), bbmax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        bool has_boundary = false;

        // collect model and terrain nodes to list
        for (int i = 0; i<nodes; ++i) {
            // only for active nodes
            if ( h3dGetNodeFlags(find_ret[i]) & H3DNodeFlags::Inactive ) continue;

            if( H3DEXT_NodeType_Sph == h3dGetNodeType(find_ret[i])) {
                QString tag(h3dGetNodeParamStr(find_ret[i], H3DNodeParams::TagStr));
                if ( tag.startsWith(wizard.field("fluid_tag").toString(), Qt::CaseInsensitive)) {
                    fluidmap[tag].push_back(find_ret[i]);
                } else if (tag.startsWith(wizard.field("rigid_tag").toString(), Qt::CaseInsensitive)) {
                    rigidmap[tag].push_back(find_ret[i]);
                }

            } else if (0 == QString(h3dGetNodeParamStr(find_ret[i], H3DNodeParams::TagStr))
                       .compare(wizard.field("boundary_tag").toString(), Qt::CaseInsensitive)) {
                has_boundary = true;
                boundaries.push_back(find_ret[i]);
                update_bbox(find_ret[i], bbmin, bbmax);
            }
        }
        if (!has_boundary) return;
        vector3f size = bbmax-bbmin;
        float scale = size.x;
        if (size.y>scale) scale = size.y;
        if (size.z>scale) scale = size.z;
        scale = 1.0f/scale;

        /// 2. generate sdf file for boundary
        QString dffile_name = wizard.field("dffile").toString();
        if ( wizard.field("is_gen_sdf").toBool() ) {
            dffile_name = wizard.field("dftarget").toString();

            // collect meshes
            QVector<int> meshlist;
            QVector<int>::const_iterator itr = boundaries.begin();
            for(int cnt=0; itr!=boundaries.end(); ++itr, ++cnt) {
                int nodes = h3dFindNodes(*itr, "", H3DNodeTypes::Mesh );
                for (int i = 0; i < nodes; ++i) meshlist.push_back(h3dGetNodeFindResult(i));
            }

            int nbx = wizard.field("nbx").toInt();
            int nby = wizard.field("nby").toInt();
            int nbz = wizard.field("nbz").toInt();

            // start generate df
            QFileInfo dffile(dfDir.absoluteFilePath(dffile_name));
            if (!dffile.absoluteDir().exists()) {
                dfDir.mkpath(dffile.absoluteDir().absolutePath());
            }
            if (!generateSdf(dffile.absoluteFilePath(), meshlist,
                             nbx, nby, nbz, wizard.field("nfr").toInt())) return;
        }

        generateSphScene(wizard.field("target_name").toString(),
                         dffile_name,
                         fluidmap,
                         rigidmap,
                         wizard.field("timestep").toFloat(),
                         wizard.field("smooth_length").toFloat(),
                         bbmin, scale);
    }
}

Q_EXPORT_STATIC_PLUGIN(SphToolPlugIn)
Q_EXPORT_PLUGIN2(SphToolPlugIn, SphToolPlugIn)
