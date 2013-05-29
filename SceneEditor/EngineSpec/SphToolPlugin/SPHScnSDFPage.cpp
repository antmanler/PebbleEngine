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
#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>

#include <SceneEditorCore/EditorService.h>

#include "SPHScnSDFPage.h"

SPHScnSDFPage::SPHScnSDFPage(QWidget *parent, int pageid)
    : QWizardPage(parent)
    //, page_id_(pageid)
{
    setupUi(this);
    setTitle(tr("Create SPH particles"));
    setSubTitle(tr("Using this wizard you can generate simulation configuration files for current scene.\n"
                   "Please choose or gernerate signed distance field for static objects, "
                   "and specify the tags for boundaries fluid and rigid-bodies, "
                   "the tag is a prefix for generator to classify the different kind of objects. "
                   "Note: different part of the same object shared the same name, eg, "
                   "all the 'FluidA' will be tread as fluid but belong to object which named A."));
    // sdf setup
    registerField("is_chs_sdf", rb_chs_sdf_) ;
    registerField("is_gen_sdf", rb_gen_sdf_) ;
    // scene setup
    registerField("boundary_tag", le_boundary_tag_);
    registerField("fluid_tag", le_fluid_tag_);
    registerField("rigid_tag", le_rigid_tag_);
}

//void SPHScnSDFPage::initializePage()
//{
//   // if (page_id_ == 0) {
//   //     setSubTitle(tr("Create new SPH fluid particles from a model scene graph file"));
//   // } else {
//   //     setSubTitle(tr("Create new SPH fluid particles from a distance field file"));
//   // }
//   // sw_funcpanel_->setCurrentIndex(page_id_ % 2);
//}
//
//bool SPHScnSDFPage::validatePage()
//{
//    QDir dfDir = QDir::current();
//
//    if (0 == page_id_) {
//        QFileInfo model(dfDir.absoluteFilePath(le_modelfile_->text()));
//        if (!model.exists() || le_modelfile_->text().isEmpty()) {
//            QMessageBox::warning(this,
//                                 tr("Error"),
//                                 tr("The model scene file in %1 is invalid !\n please specify a valid file.")
//                                 .arg(le_modelfile_->text()),
//                                 QMessageBox::Ok);
//            return false;
//        }
//    } else {
//        QFileInfo model(dfDir.absoluteFilePath(le_dffile_->text()));
//        if (!model.exists() || le_dffile_->text().isEmpty()) {
//            QMessageBox::warning(this,
//                                 tr("Error"),
//                                 tr("The distance file file in %1 is invalid !\n please specify a valid file.")
//                                 .arg(le_dffile_->text()),
//                                 QMessageBox::Ok);
//            return false;
//        }
//    }
//
//    if ( (dsb_xmin_->value() >= dsb_xmax_->value()) ||
//         (dsb_xmin_->value() >= dsb_xmax_->value()) ||
//         (dsb_xmin_->value() >= dsb_xmax_->value()) ) {
//        QMessageBox::warning(this,
//                             tr("Error"),
//                             tr("Please check the value under scnen boundary!"),
//                             QMessageBox::Ok);
//        return false;
//    }
//
//    QFileInfo dfFile(dfDir.absoluteFilePath(le_target_->text()));
//    if (dfFile.exists() || le_target_->text().isEmpty()) {
//        QMessageBox::StandardButton ret = QMessageBox::warning(this,
//                                                               tr("Warning"),
//                                                               tr("The particle file in %1 already exists !\n would you like to overwrite it?")
//                                                               .arg(le_target_->text()),
//                                                               QMessageBox::Ok|QMessageBox::Cancel);
//        if (ret != QMessageBox::Ok) return false;
//    }
//
//    return true;
//}
//
//void SPHScnSDFPage::selectModel()
//{
//    QString newmodel = EditorService::getModelFile(QDir::currentPath(), this, tr("Select a SceneGraph file to generate sph particles"));
//
//    if (!newmodel.isEmpty()) {
//        le_modelfile_->setText(newmodel);
//        QFileInfo frFile(QDir::current().absoluteFilePath(newmodel));
//        le_target_->setText(QString("sphscene/%1.fr").arg(frFile.baseName()));
//    }
//}
//
//void SPHScnSDFPage::selectDf()
//{
//    QString filter = tr("Distance field files") + QString("(*.df)");
//    filter+= tr(";;All files (*.*)");
//    QString dffile = QFileDialog::getOpenFileName(0, tr("Select distance field"), QDir::currentPath(), filter);
//
//    if (!dffile.isEmpty()) {
//        le_dffile_->setText(dffile);
//        QFileInfo frFile(QDir::current().absoluteFilePath(dffile));
//        le_target_->setText(QString("sphscene/%1.fr").arg(frFile.baseName()));
//    }
//}
