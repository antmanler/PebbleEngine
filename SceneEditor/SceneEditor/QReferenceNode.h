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

#ifndef QREFERENCENODE_H_
#define QREFERENCENODE_H_

#include <SceneEditorCore/QSceneNode.h>

class SceneTreeModel;

/**
 * A reference node item for the SceneTreeModel
 */
class QReferenceNode : public QSceneNode
{
    Q_OBJECT

    Q_CLASSINFO("QReferenceNode", "Reference Node Specific")
    Q_PROPERTY(QString Scene_Graph_File READ sceneGraph DESIGNABLE true USER true)

public:	
    static QSceneNode* loadNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parent);

    QReferenceNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parentNode);
    virtual ~QReferenceNode();

    void save();

    QString sceneGraph() const { return m_fileName; }

    SceneTreeModel* model() {return m_treeModel;}

    void addRepresentation();

    void activate();

    QString toolTip( int column );

protected:	

    SceneTreeModel*   m_treeModel;
    QString  m_fileName;
    QDomDocument   m_sceneGraphFile;

};
#endif

