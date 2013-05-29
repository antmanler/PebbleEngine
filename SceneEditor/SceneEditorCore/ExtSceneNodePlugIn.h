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


#ifndef ExtSceneNodePlugIn_H_
#define ExtSceneNodePlugIn_H_

#include <QtCore/QObject>
#include <QtXml/qdom.h>
#include "Config.h"

struct lua_State;
class SceneTreeView;
class PlugInManager;

/**
* \brief This class provides an interface for plugins managing Horde3D SceneNode Extensions
* 
* Extensions can be used to enhence the available scene nodes in Horde3D 
* Using this plugin interface the editor can be enhanced with these extensions.
*/
class DLLEXP ExtSceneNodePlugIn
{	
public:
    /**
         * Destructor
         */
    virtual ~ExtSceneNodePlugIn() {}

    /**
         * \brief returns the name of the node this plugin handles.
         * @return QString name of the node
         */
    virtual QString extensionName() const = 0;


    /**
         * Provides the PlugInManager instance for the editor that loaded this plugin
         */
    virtual void setPlugInManager(PlugInManager* factory) = 0;

    /**
         * \brief Method for registering the plugin's lua functions
         *
         * If the plugin's functionallity can be controlled by lua, it can implement
         * the registration of these functions on the provided lua stack within this method.
         * @param lua pointer to an existing lua stack used by the editor's LuaController
         */
    virtual void registerLuaFunctions(lua_State* /*lua*/) {}

};

Q_DECLARE_INTERFACE(ExtSceneNodePlugIn,	"HordeSceneEditor.ExtSceneNodePlugIn/1.0")

#endif
