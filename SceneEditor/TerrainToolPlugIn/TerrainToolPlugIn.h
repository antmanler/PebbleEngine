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

#ifndef TERRAINTOOLPLUGIN_H_
#define TERRAINTOOLPLUGIN_H_

#include <QtCore/QObject>

#include <SceneEditorCore/ToolPlugIn.h>

class QProgressDialog;

class TerrainToolPlugIn : public QObject, public ToolPlugIn
{
    Q_OBJECT
    Q_INTERFACES(ToolPlugIn)

public:
    TerrainToolPlugIn(QObject* parent = 0);
    virtual ~TerrainToolPlugIn();

    QString plugInName() const;

    QList<QAction*> actions() const;

    void setSceneFile(SceneFile* sceneFile);

private slots:
    void heightMapConverter();

private:
    QImage readPGM( const QString& fileName, QProgressDialog* dlg = 0 );
    QImage smoothInputImage( const QImage& image, QProgressDialog* dlg = 0 );
    void calculateNormalMap( QImage& image, QProgressDialog* dlg = 0 );

    QList<QAction*>      m_actions;
    SceneFile*           m_scene;
    int                  m_filterRadius;
};
#endif
