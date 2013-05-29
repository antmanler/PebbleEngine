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

#include "QReferenceNodePage.h"
#include "HordeModelDialog.h"

#include <Horde3D.h>
#include <QtGui/QFileDialog>
#include <QtCore/QDir>


QReferenceNodePage::QReferenceNodePage(QWidget* parent /*= 0*/) : QWizardPage(parent)
{
	setupUi(this);
	setTitle(tr("Add new model node"));
	setSubTitle(tr("Choose a name for the new element in the scene and select the scene file that contains the model's graphical representation"));
	connect(m_setFileName, SIGNAL(clicked()), this, SLOT(setFile()));
	registerField("name*", m_name);
	registerField("file*", m_fileName);
}


QReferenceNodePage::~QReferenceNodePage()
{
}

void QReferenceNodePage::setFile()
{
	QString fileName = HordeModelDialog::getModelFile( QDir::currentPath(), this, tr("Select scene file"));
	if (!fileName.isEmpty())
	{
		m_fileName->setText(QDir::current().relativeFilePath(fileName));
	}
}

bool QReferenceNodePage::validatePage()
{
	m_fileName->setText(QDir::current().relativeFilePath(m_fileName->text()));
	return true;
}
