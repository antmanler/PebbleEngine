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
#include "QTerrainNodePage.h"

#include <QtCore/QDir>

QTerrainNodePage::QTerrainNodePage(QWidget* parent /*= 0*/) : QWizardPage(parent)
{
    setupUi(this);
    setTitle(tr("Add new terrain"));
    setSubTitle(tr("Create a new terrain for the current scene"));
    registerField("name*", m_name);
    registerField("heightmap", m_heightMap, "textureFile", SIGNAL(textureChanged()));
    registerField("material", m_material, "materialFile", SIGNAL(materialChanged()));
}


QTerrainNodePage::~QTerrainNodePage()
{
}

void QTerrainNodePage::initializePage()
{
    m_material->init( QDir::currentPath() );
    m_heightMap->init( QDir::currentPath() );
}

