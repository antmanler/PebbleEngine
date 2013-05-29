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
#include "QSphNodePage.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtGui/QProgressDialog>
//#include <QtGui/QFileDialog>
#include <QtGui/QWizard>

//#include <HordeSceneEditorCore/EditorService.h>
#include <Horde3D/Horde3D.h>
#include <Horde3D/Horde3DUtils.h>

#include <dflib/mesh.h>
#include <dflib/distancefield3d.h>
#include <dflib/dfutils.h>
#include <dflib/mc.h>

#include <vector>
#include <set>
#include <SceneEditorCore/EditorUtils.h>
#include "SPHGenerator.h"

namespace
{
// response for the distance3d notify
struct DfNotifyResponder : public sigslot::has_slots<>
{
    DfNotifyResponder(QProgressDialog *pbd, DistanceField3D *dfgen)
        : curr_procdlg_(pbd)
        , curr_dfgen_(dfgen)
    {}

    void recvNotify(const char* stepname, int curr, int steps)
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

    QProgressDialog *curr_procdlg_;
    DistanceField3D *curr_dfgen_;
};

static vector<vector3> gTriangle;
static vector<vector3> gNormal;

static vector<vector3> gPos;
static vector<vector3> gNor;

struct Vec3Cmp {
    bool operator () (const vector3& lsh, const vector3& rhs) const {
#define lenq(r) (((r).x)*((r).x)+((r).y)*((r).y)+((r).z)*((r).z))
        return lenq(lsh) - lenq(rhs) > 0.0f;
#undef lenq
    }
} ;

static void gSaveMesh(vector3 *v1, vector3 *v2, vector3 *v3,vector3 *n1, vector3 *n2, vector3 *n3)
{
    gTriangle.push_back(*v1);
    gTriangle.push_back(*v2);
    gTriangle.push_back(*v3);

    gNormal.push_back(*n1);
    gNormal.push_back(*n2);
    gNormal.push_back(*n3);
}

}

QSphNodePage::QSphNodePage(QWidget* parent /*= 0*/) : QWizardPage(parent)
{
    setupUi(this);
    setTitle(tr("Add new sph fluid"));
    setSubTitle(tr("Create a new sph fluid for the current scene"));
    registerField("name*", m_name);
    registerField("voldatafile*", m_volumeData, "volFile", SIGNAL(voldataChanged()));
    registerField("material*", m_material, "materialFile", SIGNAL(materialChanged()));
}

SphNodeUtils::SphNodeUtils(QObject* parent)
    : QObject(parent)
{
}

QString SphNodeUtils::createParticlesFromSDF()
{
    QWizard wizard;
    wizard.addPage(new SPHGenerator(&wizard, 1));
    //	wizard.adjustSize();
    if (wizard.exec() == QDialog::Accepted) {
        QString dffile = wizard.field("dffile").toString();
        DistanceField3D df3d;
        df3d.readFromFile(dffile.toStdString());
        QString frfile = wizard.field("target_name").toString();
        vector3f bbmin(wizard.field("xmin").toFloat(), wizard.field("ymin").toFloat(), wizard.field("zmin").toFloat());
        vector3f bbmax(wizard.field("xmax").toFloat(), wizard.field("ymax").toFloat(), wizard.field("zmax").toFloat());
        unsigned int particles = dfToParticles(&df3d,
                                               bbmin, bbmax,
                                               wizard.field("sample_dist").toFloat(),
                                               wizard.field("density").toFloat(),
                                               wizard.field("negative").toBool(),
                                               wizard.field("rigid").toBool(),
                                               frfile);
        if (particles > 0) {
            return frfile;
        }
    }
    return QString();
}

QString SphNodeUtils::createParticlesFromModle()
{
    QWizard wizard;
    wizard.addPage(new SPHGenerator(&wizard, 0));
    //	wizard.adjustSize();
    if (wizard.exec() == QDialog::Accepted) {
        QString newmodel = wizard.field("modelscn").toString();
        // 1. generate distance field
        /// Add model to scene and make it inacitve
        // add resource for model
        H3DRes modres = h3dAddResource( H3DResTypes::SceneGraph, qPrintable(newmodel), 0 );
        // Load data
        if (modres == 0 || !SceneEditor::load_h3dres_from_file_qrc(".")) {
            if( modres ) h3dRemoveResource(modres);
            // release unused resources within Horde3D
            h3dReleaseUnusedResources(H3DResTypes::Undefined);
            return QString();
        }
        // add to scene graph
        H3DNode model = h3dAddNodes( H3DRootNode, modres );
        // make this node inacitve
        h3dSetNodeFlags(model, H3DNodeFlags::Inactive, true);
        QProgressDialog dlg(tr("Processing scene nodes..."), tr("Abort"), 0, 2, 0, Qt::Dialog);
        dlg.setModal(true);
        dlg.setMinimumDuration(0);
        // convert to mesh
        Mesh mesh;
        mesh.beginGemo();
        dfut::appendH3DGeomToMesh(model, mesh);
        mesh.endGeom();
        dlg.setValue(1);
        // remove node and release resources
        h3dRemoveNode(model);
        h3dReleaseUnusedResources(H3DResTypes::Undefined);
        dlg.setValue(2);

        dlg.setLabelText("Compute normals");
        mesh.computeNormals();
        //                mesh.save2Obj(QString("tmp.obj").toStdString().c_str());
        // generate distance 3d
        dlg.setLabelText("Start generating");

        int nbx = wizard.field("nbx").toInt();
        int nby = wizard.field("nby").toInt();
        int nbz = wizard.field("nbz").toInt();
        DistanceField3D generator(nbx, nby, nbz);
        DfNotifyResponder responder(&dlg, &generator);
        generator.sigNotify()->connect(make_slot(&responder, &DfNotifyResponder::recvNotify));

        generator.meshToDistanceField(mesh);
        // check if user canceled
        if (dlg.wasCanceled())   return QString();
        generator.checkConsistency(wizard.field("nfr").toInt());
        QString frfile = wizard.field("target_name").toString();
        vector3f bbmin(wizard.field("xmin").toFloat(), wizard.field("ymin").toFloat(), wizard.field("zmin").toFloat());
        vector3f bbmax(wizard.field("xmax").toFloat(), wizard.field("ymax").toFloat(), wizard.field("zmax").toFloat());
        unsigned int particles = dfToParticles(&generator,
                                               bbmin, bbmax,
                                               wizard.field("sample_dist").toFloat(),
                                               wizard.field("density").toFloat(),
                                               false,
                                               wizard.field("rigid").toBool(),
                                               frfile);
        if (particles > 0) {
            return frfile;
        }
    }
    return QString();
}

unsigned int SphNodeUtils::dfToParticles(DistanceField3D *df3d,
                                         const vector3f &bbmin, const vector3f &bbmax,
                                         const float sample_dist,
                                         const float density,
                                         const bool negative,
                                         const bool rigid,
                                         const QString& filename)
{
    if (!(sample_dist > 0)) return 0;
    float r = sample_dist;

    float spacing = r * 1.7320508075688772; //sqrt(3.0f);
    float spacing2 = 0.5f*spacing;
    float c = negative ? -1.0f : 1.0f;
    std::vector<vector3f> particles;
    QProgressDialog dlg(tr("Generating particles..."), tr("Abort"), 0, floor((bbmax.z-bbmin.z)/spacing2), 0, Qt::Dialog);
    dlg.setModal(true);
    dlg.setMinimumDuration(0);

    unsigned int psize = 0;
    if (!rigid) {
        // generate fluid particles
        bool flag1 = false;
        // generate particles from distance field
        for (float z = bbmin.z; z <= bbmax.z; z += spacing2) {
            // UI logical
            QApplication::processEvents();
            // user canceled
            if (dlg.wasCanceled())return 0;
            // update progress bar
            dlg.setValue(floor(z/spacing2));

            flag1 = !flag1;
            bool flag2 = flag1;
            for (float x = bbmin.x; x <= bbmax.x; x += spacing2) {
                float y0 = bbmin.y;
                if (flag2) y0 += spacing * 0.5f;
                flag2 = !flag2;

                for (float y = y0; y <= bbmax.y; y += spacing) {
                    // get the distance at the point x,y,z
                    // if it's positive, we're inside the fluid volume
                    // add a particle and set its distance to surface to d
                    // (this is needed for our surface reconstruction method)
                    float d;
                    df3d->distance(vector3f(x,y,z),d);
                    if ((d*c)>=0) particles.push_back(vector3f(x,y,z));
                }
            }
        }

        psize = particles.size();
    } else {
       // generate rigid particles
        gTriangle.clear();
        gNormal.clear();
        gPos.clear();
        gNor.clear();

        dlg.setMaximum(2);
        //the last two parameters are not used , set a arbitrary value.
        renderofs *r=render_create(sample_dist*0.5, 10.0, 10.0);
        renderCreatImplicit(r, df3d->nbx, df3d->nby, df3d->nbz, df3d->distance());
        render_marching_cube(r, -sample_dist, reinterpret_cast<AddTriangleCallback*> (gSaveMesh));
        render_delete(r);
        dlg.setValue(1);

        qDebug("%d, %d, %d", df3d->nbx, df3d->nby, df3d->nbz);

        dlg.setMaximum(gTriangle.size());

        set<vector3, Vec3Cmp> posset;

        for( int i=0; i<gTriangle.size();i++ ) {

            QApplication::processEvents();
            dlg.setValue(i);

            if (posset.find(gTriangle.at(i)) == posset.end()) {
                posset.insert(gTriangle.at(i));
                gPos.push_back(gTriangle.at(i));
                gNor.push_back(gNormal.at(i));
            }
        }
        psize = gPos.size();
        qDebug("%d(%lu)", psize, gTriangle.size());
    }

    if (0 == psize) return psize;

    // write to traget file
    QFileInfo frfile(QDir::current().absoluteFilePath(filename));
    if (!frfile.absoluteDir().exists()) {
        QDir::current().mkpath(frfile.absoluteDir().absolutePath());
    }
    QFile fr(frfile.absoluteFilePath());
    if (fr.open(QIODevice::WriteOnly)) {
        // wirte header: number of particles
        fr.write(reinterpret_cast<char*>(&psize), sizeof(unsigned int));
        // wirte particles
        if (!rigid) {
            fr.write(reinterpret_cast<char*>(&particles[0]), 3*psize*sizeof(float));
        } else {
            fr.write(reinterpret_cast<char*>(&gPos[0]), 3*psize*sizeof(float));
        }
        // finally atthach the extra info to the tail
        fr.write(reinterpret_cast<char*>(const_cast<float*>(&sample_dist)), sizeof(float));
        fr.write(reinterpret_cast<char*>(const_cast<float*>(&density)), sizeof(float));
        if (rigid) {
            // write rigid normals
            fr.write(reinterpret_cast<char*>(&gNor[0]), 3*psize*sizeof(float));
        }

        fr.close();
    } else return 0;

    return psize;
}

QSphNodePage::~QSphNodePage()
{
}

void QSphNodePage::initializePage()
{
    m_material->init( QDir::currentPath() );
    // init volume data combobox
    m_volumeData->init( QDir::currentPath() );
    m_volumeData->blockSignals(true);
    m_volumeData->addItem(tr("Import from df file"), QVariant((int) QVariant::UserType + 2));
    m_volumeData->addItem(tr("Import from Repository"), QVariant((int) QVariant::UserType + 1));
    m_volumeData->blockSignals(false);
    connect(m_volumeData, SIGNAL(currentIndexChanged(int)), this, SLOT(currentChanged(int)));
    last_sel_ = m_volumeData->currentText();
}

void QSphNodePage::currentChanged(int index)
{
    if (m_volumeData->itemData(index).isValid() && m_volumeData->itemData(index) == QVariant((int)QVariant::UserType + 2)) {
        SphNodeUtils utils;
        QString frfile = utils.createParticlesFromSDF();
        if (!frfile.isEmpty()) {
            // update combo box
            m_volumeData->blockSignals(true);
            m_volumeData->removeItem(index);
            m_volumeData->removeItem(m_volumeData->findText(tr("Import from Repository")));
            // add new particle file
            m_volumeData->addItem(frfile);
            m_volumeData->addItem(tr("Import from df file"), QVariant(QVariant::UserType + 2));
            m_volumeData->addItem(tr("Import from Repository"), QVariant((int) QVariant::UserType + 1));
            m_volumeData->setCurrentIndex(m_volumeData->findText(frfile));
            m_volumeData->blockSignals(false);
            last_sel_ = frfile;
        } else {
            m_volumeData->setCurrentIndex(m_volumeData->findText(last_sel_));
            return;
        }

    } else if ( m_volumeData->itemData(index).isValid() && m_volumeData->itemData(index) == QVariant((int)QVariant::UserType+1) ) {
        SphNodeUtils utils;
        QString frfile = utils.createParticlesFromModle();
        if (!frfile.isEmpty()) {
            // update combo box
            m_volumeData->blockSignals(true);
            m_volumeData->removeItem(index);
            m_volumeData->removeItem(m_volumeData->findText(tr("Import from df file")));
            // add new particle file
            m_volumeData->addItem(frfile);
            m_volumeData->addItem(tr("Import from df file"), QVariant(QVariant::UserType + 2));
            m_volumeData->addItem(tr("Import from Repository"), QVariant((int) QVariant::UserType + 1));
            m_volumeData->setCurrentIndex(m_volumeData->findText(frfile));
            m_volumeData->blockSignals(false);
            last_sel_ = frfile;
        } else {
            m_volumeData->setCurrentIndex(m_volumeData->findText(last_sel_));
            return;
        }
    } else last_sel_ = m_volumeData->currentText();

    // display particle info
    QFileInfo frfile(QDir::current().absoluteFilePath(m_volumeData->currentText()));
    if (frfile.exists()) {
        QFile fr(frfile.absoluteFilePath());
        if (fr.open(QIODevice::ReadOnly)) {
            unsigned int pcount = 0;
            fr.read(reinterpret_cast<char*>(&pcount), sizeof(unsigned int));
            float sample_dist = 0.0f;
            float density = 0.0f;
            if (pcount > 0 && 3*pcount*sizeof(float) + sizeof(unsigned int) < fr.size()) {
                // seek for sample distance
                fr.seek(fr.pos() + 3*pcount*sizeof(float));
                fr.read(reinterpret_cast<char*>(&sample_dist), sizeof(float));
                fr.read(reinterpret_cast<char*>(&density), sizeof(float));
            }
            QString strtpl(tr("<html><head/><body>"
                              "<p>Paritcle number: %1</p>"
                              "<p>Sample distance: %2</p>"
                              "<p>Density: %3</p>"
                              "</body></html>"));
            lb_info->setText(strtpl
                             .arg(pcount)
                             .arg((sample_dist>0.0 ? QString("%1").arg(sample_dist) : "N/A"))
                             .arg((sample_dist>0.0 ? QString("%1").arg(density) : "N/A")));
            fr.close();
            return;
        }
    }
    lb_info->setText("");
}

