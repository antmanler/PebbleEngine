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


#ifndef TOOLPLUGIN_H_
#define TOOLPLUGIN_H_

#include <QtGui/QAction>
#include <QtCore/QList>
#include "Config.h"

class SceneFile;

/**
* \brief This class provides an interface for tool plugins integrateable in the editor
* 
* 
*/
class DLLEXP ToolPlugIn 
{	
public:

    /**
         * Destructor
         */
    virtual ~ToolPlugIn() {}

    /**
         * \brief returns the name of the plugin.
         *
         * @return QString name of the plugin
         */
    virtual QString plugInName() const = 0;

    /**
         * \brief Actions provided by this plugin
         * @return QList<QAction*>
         */
    virtual QList<QAction*> actions() const = 0;

    /**
         * \brief Sets the currently loaded scene
         *
         */
    virtual void setSceneFile(SceneFile* file) = 0;

};

Q_DECLARE_INTERFACE(ToolPlugIn,	"HordeSceneEditor.ToolPlugIn/1.0")

#endif
