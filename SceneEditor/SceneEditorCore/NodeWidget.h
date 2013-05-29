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

#ifndef NODEWIDGET_H_
#define NODEWIDGET_H_

#include <SceneEditorCore/QPropertyEditor/QPropertyEditorWidget.h>
#include "Config.h"

class QSceneNode;
class QXmlTreeNode;
class QPropertyEditorWidget;

/**
 * Container Widget for manipulating Lights, Cameras, Models and later Joints, Meshes, etc.
 */
class DLLEXP NodeWidget : public QPropertyEditorWidget
{
    Q_OBJECT
public:
    NodeWidget(QWidget* parent = 0);
    virtual ~NodeWidget();

public slots:

    void setCurrentNode(QXmlTreeNode* node);

    void moveObject(const float x, const float y, const float z);
    void rotateObject(const float rx, const float ry, const float rz);
    void scaleObject(const float sx, const float sy, const float sz);

signals:
    void materialChanged(const QString materialFileName);
    void modified(bool);

private:
    QXmlTreeNode*	m_currentNode;
    float			m_tx, m_ty, m_tz;
    float			m_rx, m_ry, m_rz;
    float			m_sx, m_sy, m_sz;
};
#endif

