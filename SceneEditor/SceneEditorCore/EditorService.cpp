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
// is a flexible graphics engine.
//
// ****************************************************************************************
// 
#include "HordeFileDialog.h"
#include "EditorService.h"

IEditorService *EditorService::provider_ = NULL;

void EditorService::registerProvider(IEditorService *provider)
{
    provider_ = provider;
//    qDebug("set value\n");
//    qDebug("provider_ != NULL %s\n", provider_ == NULL ? "false" : "true");
}

QString EditorService::getResourceFile(H3DResTypes::List resourceType, const QString &targetPath, QWidget *parent, const QString &caption)
{
    return HordeFileDialog::getResourceFile(resourceType, targetPath, parent, caption);
}


QString EditorService::getModelFile(const QString &targetPath, QWidget *parent, const QString &caption)
{
    if (provider_ != NULL)
        return provider_->getModelFile(targetPath, parent, caption);
//    qDebug("provider_ != NULL %s\n", provider_ == NULL ? "false" : "true");
    return QString();
}

SceneFile *EditorService::currentScene()
{
    if (provider_ != NULL)
        return provider_->currentScene();
    return NULL;
}
