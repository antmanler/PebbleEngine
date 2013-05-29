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
#include "FindDialog.h"

FindDialog* FindDialog::m_instance = 0;

FindDialog* FindDialog::showFindDialog(QWidget* parent /*= 0*/)
{
	if (m_instance == 0)
		m_instance = new FindDialog(parent);
	m_instance->setVisible(true);
	return m_instance;
}

void FindDialog::releaseDialog()
{
	delete m_instance;
	m_instance = 0;
}

FindDialog::FindDialog(QWidget* parent /*= 0*/, Qt::WFlags flags /*= 0*/) : QDialog(parent, flags)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
}


FindDialog::~FindDialog()
{
	m_instance = 0;
}

void FindDialog::accept()
{
	emit find(m_searchText->text());
}



