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

#include "SPHGenerator.h"

SPHGenerator::SPHGenerator(QWidget *parent, int pageid)
    : QWizardPage(parent)
    , page_id_(pageid)
{
    setupUi(this);
    setTitle(tr("Create SPH particles"));
    // save file name
    registerField("target_name*", le_target_);
    registerField("sample_dist", dsb_sampledist_, "value", SLOT(valueChanged()));
    registerField("density", dsb_density_, "value", SLOT(valueChanged()));
    registerField("negative", cb_negative);
    registerField("xmin", dsb_xmin_, "value", SLOT(valueChanged()));
    registerField("xmax", dsb_xmax_, "value", SLOT(valueChanged()));
    registerField("ymin", dsb_ymin_, "value", SLOT(valueChanged()));
    registerField("ymax", dsb_ymax_, "value", SLOT(valueChanged()));
    registerField("zmin", dsb_zmin_, "value", SLOT(valueChanged()));
    registerField("zmax", dsb_zmax_, "value", SLOT(valueChanged()));
    registerField("rigid", cb_isrigid);
    // from model
    registerField("nbx", sb_nbx_);
    registerField("nby", sb_nby_);
    registerField("nbz", sb_nbz_);
    registerField("nfr", sb_filter_r_);
    registerField("modelscn", le_modelfile_);
    // from df file
    registerField("dffile", le_dffile_);
    // connect signals to slots
    connect(tb_openmodel_, SIGNAL(clicked()), this, SLOT(selectModel()));
    connect(tb_opendf_, SIGNAL(clicked()), this, SLOT(selectDf()));

}

void SPHGenerator::initializePage()
{
    if (page_id_ == 0) {
        setSubTitle(tr("Create new SPH fluid particles from a model scene graph file"));
    } else {
        setSubTitle(tr("Create new SPH fluid particles from a distance field file"));
    }
    sw_funcpanel_->setCurrentIndex(page_id_ % 2);
}

bool SPHGenerator::validatePage()
{
    QDir dfDir = QDir::current();

    if (0 == page_id_) {
        QFileInfo model(dfDir.absoluteFilePath(le_modelfile_->text()));
        if (!model.exists() || le_modelfile_->text().isEmpty()) {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("The model scene file in %1 is invalid !\n please specify a valid file.")
                                 .arg(le_modelfile_->text()),
                                 QMessageBox::Ok);
            return false;
        }
    } else {
        QFileInfo model(dfDir.absoluteFilePath(le_dffile_->text()));
        if (!model.exists() || le_dffile_->text().isEmpty()) {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("The distance file file in %1 is invalid !\n please specify a valid file.")
                                 .arg(le_dffile_->text()),
                                 QMessageBox::Ok);
            return false;
        }
    }

    if ( (dsb_xmin_->value() >= dsb_xmax_->value()) ||
         (dsb_xmin_->value() >= dsb_xmax_->value()) ||
         (dsb_xmin_->value() >= dsb_xmax_->value()) ) {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Please check the value under scnen boundary!"),
                             QMessageBox::Ok);
        return false;
    }

    QFileInfo dfFile(dfDir.absoluteFilePath(le_target_->text()));
    if (dfFile.exists() || le_target_->text().isEmpty()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this,
                                                               tr("Warning"),
                                                               tr("The particle file in %1 already exists !\n would you like to overwrite it?")
                                                               .arg(le_target_->text()),
                                                               QMessageBox::Ok|QMessageBox::Cancel);
        if (ret != QMessageBox::Ok) return false;
    }

    return true;
}

void SPHGenerator::selectModel()
{
    QString newmodel = EditorService::getModelFile(QDir::currentPath(), this, tr("Select a SceneGraph file to generate sph particles"));

    if (!newmodel.isEmpty()) {
        le_modelfile_->setText(newmodel);
        QFileInfo frFile(QDir::current().absoluteFilePath(newmodel));
        le_target_->setText(QString("sphscene/%1.fr").arg(frFile.baseName()));
    }
}

void SPHGenerator::selectDf()
{
    QString filter = tr("Distance field files") + QString("(*.df)");
    filter+= tr(";;All files (*.*)");
    QString dffile = QFileDialog::getOpenFileName(0, tr("Select distance field"), QDir::currentPath(), filter);

    if (!dffile.isEmpty()) {
        le_dffile_->setText(dffile);
        QFileInfo frFile(QDir::current().absoluteFilePath(dffile));
        le_target_->setText(QString("sphscene/%1.fr").arg(frFile.baseName()));
    }
}
