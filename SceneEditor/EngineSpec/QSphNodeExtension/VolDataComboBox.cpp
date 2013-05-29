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

#include <QtGui/QMessageBox>
//#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QEvent>
//#include <QtGui/QProgressDialog>
//#include <QtGui/QWizard>

//#include <HordeSceneEditorCore/EditorService.h>
//#include <Horde3D/Horde3D.h>
//#include <Horde3D/Horde3DUtils.h>

//#include <dflib/mesh.h>
//#include <dflib/distancefield3d.h>
//#include <dflib/dfutils.h>

//#include "SPHGenerator.h"
#include "VolDataComboBox.h"


//namespace
//{
//// response for the distance3d notify
//struct DfNotifyResponder : public sigslot::has_slots<>
//{
//    DfNotifyResponder(QProgressDialog *pbd, DistanceField3D *dfgen)
//        : curr_procdlg_(pbd)
//        , curr_dfgen_(dfgen)
//    {}

//    void recvNotify(const char* stepname, int curr, int steps)
//    {
//        if (curr_procdlg_ && curr_dfgen_ && curr_procdlg_->wasCanceled()) {
//            curr_dfgen_->cancel();
//        } else if (curr_procdlg_) {
//            curr_procdlg_->setLabelText(QString("%1 (%2/%3) ...").arg(stepname).arg(curr).arg(steps));
//            curr_procdlg_->setMaximum(steps);
//            curr_procdlg_->setValue(curr);
//        }
//        QApplication::processEvents();
////        QApplication::sendPostedEvents(curr_procdlg_, 0);
//    }

//    QProgressDialog *curr_procdlg_;
//    DistanceField3D *curr_dfgen_;
//};

//}

VolDataComboBox::VolDataComboBox(QWidget* parent /*= 0*/)
    : QComboBox(parent)
//    , can_close_(true)
{	
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentChanged(int)));
}


VolDataComboBox::~VolDataComboBox()
{
//    qDebug("Closed");
}

//bool VolDataComboBox::event(QEvent *event)
//{
//    if (!can_close_) {
//        qDebug("type: %d", event->type());
//        event->ignore();
//    } else {
//        return QComboBox::event(event);
//    }
//    return true;
//}

void VolDataComboBox::init(const QString& resourcePath )
{
    clear();
    blockSignals(true);
    addItem(tr("No volume data"), QVariant((int) -1));
    m_resourcePath = resourcePath;
    if (!resourcePath.isEmpty()) addVolDataFiles(resourcePath, resourcePath);
//    addItem(tr("Import from df file"), QVariant((int) QVariant::UserType));
//    addItem(tr("Import from Repository"), QVariant((int) QVariant::UserType + 1));
    if (count() == 1) setCurrentIndex(-1);
    blockSignals(false);
}

QString VolDataComboBox::volFile() const
{
    if (currentIndex() == -1 || itemData(currentIndex(), Qt::UserRole).toInt() == -1)
        return QString();
    else
        return currentText();
}

void VolDataComboBox::setVolFile(const QString& filename)
{
    setVolData(VolData(filename));
}

VolData VolDataComboBox::volData() const
{
    return VolData(volFile());
}

void VolDataComboBox::setVolData(VolData vol)
{
    blockSignals(true);
    if ( vol.fileName().isEmpty() ) setCurrentIndex(0); // the no material entry
    else setCurrentIndex(findText(vol.fileName()));
    blockSignals(false);
    m_init = vol.fileName();
}

void VolDataComboBox::currentChanged(int index)
{
//    if (itemData(index).isValid() && itemData(index) == QVariant((int)QVariant::UserType)) {

//        QString filter = tr("Distance field files") + QString("(*.df)");
//        filter+= tr(";;All files (*.*)");
//        QString newmodel = QFileDialog::getOpenFileName(0, tr("Select distance field"), QDir::currentPath(), filter);

//        if (!newmodel.isEmpty()) {
//            if (findText(newmodel) == -1) {
//                // update combo box
//                blockSignals(true);
//                removeItem(index);
//                removeItem(findText(tr("Import from Repository")));
//                // add new particle file
//                addItem(newmodel);
//                addItem(tr("Import from df file"), QVariant(QVariant::UserType));
//                addItem(tr("Import from Repository"), QVariant((int) QVariant::UserType + 1));
//                setCurrentIndex(findText(newmodel));
//                blockSignals(false);
//            }
//        } else {
//            setCurrentIndex(findText(m_init));
//            return;
//        }

//    } else if ( itemData(index).isValid() && itemData(index) == QVariant((int)QVariant::UserType+1) ) {
//        QString newmodel = EditorService::getModelFile(m_resourcePath, this, tr("Select a SceneGraph file to generate sph particles"));
//        if (!newmodel.isEmpty()) {
//            QWizard wizard;
//            wizard.addPage(new SPHGenerator(&wizard));
//            if (wizard.exec() == QDialog::Accepted) {
//                // 1. generate distance field
//                /// Add model to scene and make it inacitve
//                // add resource for model
//                H3DRes modres = h3dAddResource( H3DResTypes::SceneGraph, qPrintable(newmodel), 0 );
//                // Load data
//                if (modres == 0 || !h3dutLoadResourcesFromDisk( "." )) {
//                    if( modres ) h3dRemoveResource(modres);
//                    // release unused resources within Horde3D
//                    h3dReleaseUnusedResources();
//                    setCurrentIndex(findText(m_init));
//                    return;
//                }
//                // add to scene graph
//                H3DNode model = h3dAddNodes( H3DRootNode, modres );
//                // make this node inacitve
//                h3dSetNodeFlags(model, H3DNodeFlags::Inactive, true);
//                can_close_ = false;
//                QProgressDialog dlg(tr("Processing scene nodes..."), tr("Abort"), 0, 2);
//                dlg.setMinimumDuration(0);
//                // convert to mesh
//                Mesh mesh;
//                mesh.beginGemo();
//                dfut::appendH3DGeomToMesh(model, mesh);
//                mesh.endGeom();
//                dlg.setValue(1);
//                // remove node and release resources
//                h3dRemoveNode(model);
//                h3dReleaseUnusedResources();
//                dlg.setValue(2);

//                dlg.setLabelText("Compute normals");
//                mesh.computeNormals();
//                //                mesh.save2Obj(QString("tmp.obj").toStdString().c_str());
//                // generate distance 3d
//                dlg.setLabelText("Start generating");

//                int nbx = wizard.field("nbx").toInt();
//                int nby = wizard.field("nby").toInt();
//                int nbz = wizard.field("nbz").toInt();
//                DistanceField3D generator(nbx, nby, nbz);
//                DfNotifyResponder responder(&dlg, &generator);
//                generator.sigNotify()->connect(make_slot(&responder, &DfNotifyResponder::recvNotify));

//                generator.meshToDistanceField(mesh);
//                can_close_ = true;
//                // check if user canceled
//                if (dlg.wasCanceled())  return;
//                //                generator.writeToFile(QString("tmp.df").toStdString().c_str());

//                // update combo box
//                blockSignals(true);
//                removeItem(index);
//                removeItem(findText(tr("Import from df file")));
//                // add new particle file
//                addItem(newmodel);
//                addItem(tr("Import from df file"), QVariant(QVariant::UserType));
//                addItem(tr("Import from Repository"), QVariant((int) QVariant::UserType + 1));
//                setCurrentIndex(findText(newmodel));
//                m_init = newmodel;
//                blockSignals(false);
//            }
//        } else {
//            setCurrentIndex(findText(m_init));
//            return;
//        }
//    }

    if (m_init != currentText()) {
        emit editFinished();
        emit voldataChanged();
    }
}

void VolDataComboBox::addVolDataFiles(const QDir& base, const QString dir)
{	
    QList<QFileInfo> vols = QDir(dir).entryInfoList(QStringList("*.fr"), QDir::Files | QDir::Readable);
    foreach(QFileInfo vol, vols) {
        QString text(base.relativeFilePath(vol.absoluteFilePath()));
        addItem(text);
#ifdef Q_WS_WIN
        setItemData(count()-1, text, Qt::ToolTipRole);
#endif

    }
    QList<QFileInfo> volDirs = QDir(dir).entryInfoList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot);
    foreach(QFileInfo directory, volDirs) {
        addVolDataFiles(base, directory.absoluteFilePath());
    }
}
