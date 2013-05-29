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

#include "SPHScnVarPage.h"

SPHScnVarPage::SPHScnVarPage(QWidget *parent, int pageid)
    : QWizardPage(parent)
    //    , page_id_(pageid)
{
    setupUi(this);
    setTitle(tr("Create SPH particles"));
    setSubTitle(tr("On this page, you need to specify the arguments for simulation.\n"
                   "Note: target name must be specified, it is the path for output configuraion file"));
    // save file name
    registerField("target_name*", le_target_);
    registerField("timestep", dsb_timestep_, "value", SLOT(valueChanged()));
    registerField("smooth_length", dsb_smooth_length_, "value", SLOT(valueChanged()));

    // from model
    registerField("nbx", sb_nbx_);
    registerField("nby", sb_nby_);
    registerField("nbz", sb_nbz_);
    registerField("nfr", sb_filter_r_);
    registerField("dftarget", le_sdf_target_);
    // from df file
    registerField("dffile", le_sdf_file_);
    // connect signals to slots
    connect(tb_opendf_, SIGNAL(clicked()), this, SLOT(selectDf()));
    connect(le_sdf_target_, SIGNAL(textEdited(QString)), this, SLOT(sdf_target_change(QString)));

}

void SPHScnVarPage::initializePage()
{
    if (field("is_gen_sdf").toBool()) {
        sw_funcpanel_->setCurrentIndex(0);
    } else {
        sw_funcpanel_->setCurrentIndex(1);
    }
}

bool SPHScnVarPage::validatePage()
{
    QDir dfDir = QDir::current();

    if (0 == sw_funcpanel_->currentIndex()) {
        QFileInfo sdf(dfDir.absoluteFilePath(le_sdf_target_->text()));
        if (le_sdf_target_->text().isEmpty()) {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("The SDF file %1 is invalid !\n please specify a valid file.")
                                 .arg(le_sdf_target_->text()),
                                 QMessageBox::Ok);
            return false;
        }
        if ( sdf.exists() ) {
           QMessageBox::StandardButton ret = QMessageBox::warning(this,
                       tr("Warning"),
                       tr("The df file in %1 already exists !\n would you like to overwrite it?").arg(le_sdf_target_->text()),
                       QMessageBox::Ok|QMessageBox::Cancel);
           if (ret != QMessageBox::Ok) return false;
        }
    } else {
        QFileInfo sdf(dfDir.absoluteFilePath(le_sdf_file_->text()));
        if (!sdf.exists() || le_sdf_file_->text().isEmpty()) {
            QMessageBox::warning(this,
                                 tr("Error"),
                                 tr("The distance file file %1 is invalid !\n please specify a valid file.")
                                 .arg(le_sdf_file_->text()),
                                 QMessageBox::Ok);
            return false;
        }
    }

    QFileInfo sph_file(dfDir.absoluteFilePath(le_target_->text()));
    if (sph_file.exists() || le_target_->text().isEmpty()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this,
                                                               tr("Warning"),
                                                               tr("The configure file %1 already exists !\n would you like to overwrite it?")
                                                               .arg(le_target_->text()),
                                                               QMessageBox::Ok|QMessageBox::Cancel);
        if (ret != QMessageBox::Ok) return false;
    }

    return true;
}

void SPHScnVarPage::sdf_target_change(QString text)
{
    if (!text.isEmpty()) {
        QFileInfo frFile(QDir::current().absoluteFilePath(text));
        le_target_->setText(QString("simdata/%1.conf").arg(frFile.baseName()));
        int pos = le_sdf_target_->cursorPosition();
        le_sdf_target_->setText(QString("%1/%2.df").arg(frFile.dir().dirName()).arg(frFile.baseName()));
        le_sdf_target_->setCursorPosition(pos);
    }
}

//void SPHScnVarPage::selectModel()
//{
//    QString newmodel = EditorService::getModelFile(QDir::currentPath(), this, tr("Select a SceneGraph file to generate sph particles"));
//
//    if (!newmodel.isEmpty()) {
//        le_modelfile_->setText(newmodel);
//        QFileInfo frFile(QDir::current().absoluteFilePath(newmodel));
//        le_target_->setText(QString("sphscene/%1.fr").arg(frFile.baseName()));
//    }
//}

void SPHScnVarPage::selectDf()
{
    QString filter = tr("Distance field files") + QString("(*.df)");
    filter+= tr(";;All files (*.*)");
    QString dffile = QFileDialog::getOpenFileName(0, tr("Select distance field"), QDir::currentPath(), filter);

    if (!dffile.isEmpty()) {
        le_sdf_file_->setText(dffile);
        QFileInfo frFile(QDir::current().absoluteFilePath(dffile));
        le_target_->setText(QString("simdata/%1.conf").arg(frFile.baseName()));
    }
}
