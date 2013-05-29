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
#include "ExtTerrainPlugIn.h"

#include <SceneEditorCore/PlugInManager.h>
#include "QTerrainNode.h"

#include <QtCore/qplugin.h>

ExtTerrainPlugIn::ExtTerrainPlugIn(QObject* parent /*= 0*/) : QObject(parent)
{

}

ExtTerrainPlugIn::~ExtTerrainPlugIn()
{
    m_factory->unregisterSceneNode( extensionName() );
}

void ExtTerrainPlugIn::setPlugInManager(PlugInManager *factory)
{
    m_factory = factory;
    factory->registerSceneNode( extensionName(), QTerrainNode::loadNode, QTerrainNode::createNode );
}

void ExtTerrainPlugIn::registerLuaFunctions(lua_State *lua)
{
}

Q_EXPORT_STATIC_PLUGIN(ExtTerrainPlugIn)
Q_EXPORT_PLUGIN2(extterrainplugin, ExtTerrainPlugIn)
