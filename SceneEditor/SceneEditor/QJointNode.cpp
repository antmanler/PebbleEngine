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

#include "QJointNode.h"

#include <QXmlTree/QXmlNodePropertyCommand.h>
#include "HordeSceneEditor.h"
#include <SceneEditorCore/AttachmentPlugIn.h>
#include <SceneEditorCore/SceneFile.h>
#include <SceneEditorCore/SceneTreeModel.h>
#include <SceneEditorCore/PlugInManager.h>

#include <QtCore/QTextStream>

#include <Horde3D.h>

QSceneNode* QJointNode::loadNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parent)
{
    return new QJointNode(xmlNode, row, model, parent);
}

QJointNode::QJointNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parentNode) : 
    QSceneNode(xmlNode, row, model, parentNode)
{
    setObjectName("QJointNode");
    QJointNode::addRepresentation();
}


QJointNode::~QJointNode()
{
}


void QJointNode::addRepresentation()
{		
    QSceneNode* parentNode = static_cast<QSceneNode*>(parent());
    unsigned int rootID = parentNode ? parentNode->hordeId() : H3DRootNode;

    m_hordeID = h3dAddJointNode(rootID, qPrintable(m_xmlNode.attribute("name", "ATTENTION No Node Name")), m_xmlNode.attribute("jointIndex").toInt());
    h3dSetNodeParamStr(m_hordeID, H3DNodeParams::TagStr, qPrintable(m_xmlNode.attribute("tag", tr(""))));
    float x, y, z, rx, ry, rz, sx, sy, sz;
    getTransformation(x,y,z,rx,ry,rz,sx,sy,sz);
    h3dSetNodeTransform(m_hordeID, x, y, z, rx, ry, rz, sx, sy, sz);

    // Attachment
    QDomElement attachment = m_xmlNode.firstChildElement("Attachment");
    SceneTreeModel* model = static_cast<SceneTreeModel*>(m_model);
    AttachmentPlugIn* plugIn = model->nodeFactory()->attachmentPlugIn();
    if (!attachment.isNull() &&  plugIn != 0)
        plugIn->initNodeAttachment(this);
}

int QJointNode::jointIndex() const
{
    return m_xmlNode.attribute("jointIndex").toInt();
}

