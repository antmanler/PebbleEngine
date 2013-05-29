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

#include <QtCore/qplugin.h>
#include <SceneEditorCore/QPropertyEditor/QPropertyEditorWidget.h>

#include <SceneEditorCore/PlugInManager.h>
#include "QSphNodePage.h"
#include "QSphNode.h"
#include "SphCustomTypes.h"
#include "ExtSphPlugIn.h"

ExtSphPlugIn::ExtSphPlugIn(QObject* parent /*= 0*/)
    : QObject(parent)
    , prop_editor_(0x0)
{
    SphCustomTypes::registerTypes();
}

ExtSphPlugIn::~ExtSphPlugIn()
{
    factory_->unregisterSceneNode( extensionName() );
}

void ExtSphPlugIn::setPlugInManager(PlugInManager *factory)
{
    factory_ = factory;
    if ( 0x0 != factory->propertyWidget() )
        onPorpertWidgetChanged(factory->propertyWidget());

    factory->registerSceneNode( extensionName(), QSphNode::loadNode, QSphNode::createNode );
    connect(factory, SIGNAL(propertyWidgetChanged(QPropertyEditorWidget*)), this, SLOT(onPorpertWidgetChanged(QPropertyEditorWidget*)));

    SphNodeUtils *utils = new SphNodeUtils(this);
    // create actions
    // 1
    QAction* dfAction = new QAction(tr("Create particles from SDF file"), this);
    dfAction->setEnabled(true);
    dfAction->setData("fromdf");
    connect(dfAction, SIGNAL(triggered()), utils, SLOT(createParticlesFromSDF()));
    actions_.push_back(dfAction);
    // 2
    dfAction = new QAction(tr("Create particles from model"), this);
    dfAction->setEnabled(false);
    connect(dfAction, SIGNAL(triggered()), utils, SLOT(createParticlesFromModle()));
    actions_.push_back(dfAction);
    // add to menu bar
    factory->registerToolActions(actions_);
    connect(factory, SIGNAL(sceneFileSet(SceneFile*)), this, SLOT(setSceneFile(SceneFile*)));
}

void ExtSphPlugIn::setSceneFile(SceneFile* scene)
{
    foreach(QAction* action, actions_) {
        if (action->data().toString() == "fromdf") {
            action->setEnabled(true);
            continue;
        }
        action->setEnabled(scene != 0);
    }
}


void ExtSphPlugIn::registerLuaFunctions(lua_State *lua)
{
}

void ExtSphPlugIn::onPorpertWidgetChanged(QPropertyEditorWidget* widget)
{
    if (0x0 !=prop_editor_) {
        prop_editor_->unregisterCustomPropertyCB(SphCustomTypes::createCustomProperty);
    }
    widget->registerCustomPropertyCB(SphCustomTypes::createCustomProperty);
}

Q_EXPORT_STATIC_PLUGIN(ExtSphPlugIn)
Q_EXPORT_PLUGIN2(extsphplugin, ExtSphPlugIn)
