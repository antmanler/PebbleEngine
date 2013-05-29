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
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>

#include "GeneratorWizard.h"

GeneratorWizard::GeneratorWizard(QWidget* parent /*= 0*/) : QWizardPage(parent)
{
    setupUi(this);
    setTitle(tr("Generate distance field for current scene"));
    setSubTitle(tr("Using this wizard you can generate distance field for current scene.\n"
                   "The generated distance field will contain the static geomerty data of current scene include Model Node and Terrain Node,"
                   "and will be saved relative to the scene's texture directory."));
    registerField("target*", le_target_);
    registerField("viewdf", cb_viewdf_);
    registerField("nbx", sb_nbx_);
    registerField("nby", sb_nby_);
    registerField("nbz", sb_nbz_);
    registerField("nfr", sb_filter_r_);
    connect(le_target_, SIGNAL(textEdited(QString)), this, SLOT(sdf_target_change(QString)));
}


GeneratorWizard::~GeneratorWizard()
{
}

bool GeneratorWizard::validatePage()
{
    QDir dfDir = QDir::current();
    QFileInfo dfFile(dfDir.absoluteFilePath(le_target_->text()));
    if ( dfFile.exists() ) {
       QMessageBox::StandardButton ret = QMessageBox::warning(this,
		   tr("Warning"), 
		   tr("The df file in %1 already exists !\n would you like to overwrite it?").arg(le_target_->text()),
		   QMessageBox::Ok|QMessageBox::Cancel);
       if (ret == QMessageBox::Ok) return true;
       return false;
    }
    return true;
}

void GeneratorWizard::sdf_target_change(QString text)
{
    if (!text.isEmpty()) {
        QFileInfo frFile(QDir::current().absoluteFilePath(text));
        int pos = le_target_->cursorPosition();
        le_target_->setText(QString("%1/%2.df").arg(frFile.dir().dirName()).arg(frFile.baseName()));
        le_target_->setCursorPosition(pos);
    }
}
