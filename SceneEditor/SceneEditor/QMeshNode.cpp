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

#include "QMeshNode.h"

#include <QXmlTree/QXmlNodePropertyCommand.h>
#include "HordeSceneEditor.h"
#include "GLWidget.h"
#include <SceneEditorCore/AttachmentPlugIn.h>
#include <SceneEditorCore/SceneFile.h>
#include <SceneEditorCore/SceneTreeModel.h>
#include <SceneEditorCore/PlugInManager.h>
#include <SceneEditorCore/EditorUtils.h>

#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <Horde3D.h>
#include <Horde3DUtils.h>

QSceneNode* QMeshNode::loadNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parent)
{
    return new QMeshNode(xmlNode, row, model, parent);
}

QMeshNode::QMeshNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parentNode) : 
    QSceneNode(xmlNode, row, model, parentNode)
{
    setObjectName("Mesh");
    QMeshNode::addRepresentation();
}


QMeshNode::~QMeshNode()
{
    if (m_resourceID != 0)
    {
        h3dRemoveResource(m_resourceID);
        m_resourceID = 0;
    }
}


void QMeshNode::addRepresentation()
{
    m_resourceID = h3dAddResource(H3DResTypes::Material, qPrintable(m_xmlNode.attribute("material")), 0);

    QSceneNode* parentNode = static_cast<QSceneNode*>(parent());
    unsigned int rootID = parentNode ? parentNode->hordeId() : H3DRootNode;

    int primeType = m_xmlNode.attribute("primType").isEmpty() ? 0 : m_xmlNode.attribute("primType").toInt();
    m_hordeID = h3dAddMeshNode(
                    rootID,
                    qPrintable(m_xmlNode.attribute("name")),
                    m_resourceID,
                    primeType,
                    m_xmlNode.attribute("batchStart").toUInt(),
                    m_xmlNode.attribute("batchCount").toUInt(),
                    m_xmlNode.attribute("vertRStart").toUInt(),
                    m_xmlNode.attribute("vertREnd").toUInt()
                    );
    h3dSetNodeParamStr(m_hordeID, H3DNodeParams::TagStr, qPrintable(m_xmlNode.attribute("tag", tr(""))));
    // load transformation from file...
    float x, y, z, rx, ry, rz, sx, sy, sz;
    getTransformation(x,y,z,rx,ry,rz,sx,sy,sz);
    // ...and update scene representation
    h3dSetNodeTransform(m_hordeID, x, y, z, rx, ry, rz, sx, sy, sz);
    h3dSetNodeParamI( m_hordeID, H3DMesh::LodLevelI, lodLevel() );
    // Attachment
    QDomElement attachment = m_xmlNode.firstChildElement("Attachment");
    SceneTreeModel* model = static_cast<SceneTreeModel*>(m_model);
    AttachmentPlugIn* plugIn = model->nodeFactory()->attachmentPlugIn();
    if (!attachment.isNull() &&  plugIn != 0)
        plugIn->initNodeAttachment(this);

}

Material QMeshNode::material() const
{
    return Material(m_xmlNode.attribute("material"));
}

void QMeshNode::setMaterial(const Material &material)
{
    if (signalsBlocked())
    {
        m_xmlNode.setAttribute("material", material.FileName);
        H3DRes resourceID = h3dFindResource(H3DResTypes::Material, qPrintable(QMeshNode::material().FileName));
        if (resourceID != 0)
            h3dRemoveResource(resourceID);
        resourceID = h3dAddResource( H3DResTypes::Material, qPrintable(material.FileName), 0 );
        h3dSetNodeParamI(m_hordeID, H3DMesh::MatResI, resourceID);
        //        h3dutLoadResourcesFromDisk(".");
        SceneEditor::load_h3dres_from_file_qrc(".");
    }
    else if (material != QMeshNode::material())
    {
        m_model->undoStack()->push(new QXmlNodePropertyCommand("Set Material", this, "Material", QVariant::fromValue(material), MeshMaterialID));
        emit materialChanged( material.FileName );
    }
}

int QMeshNode::batchCount() const
{
    return m_xmlNode.attribute("batchCount").toInt();
}

int QMeshNode::batchStart() const
{
    return m_xmlNode.attribute("batchStart").toInt();
}

int QMeshNode::vertRStart() const
{
    return m_xmlNode.attribute("vertRStart").toInt();
}


int QMeshNode::vertREnd() const
{
    return m_xmlNode.attribute("vertREnd").toInt();
}

int QMeshNode::lodLevel() const
{
    return m_xmlNode.attribute("lodLevel", "0").toInt();
}

void QMeshNode::setLodLevel( int lodLevel )
{
    if (signalsBlocked())
    {
        m_xmlNode.setAttribute( "lodLevel", lodLevel );
        h3dSetNodeParamI( m_hordeID, H3DMesh::LodLevelI, lodLevel );
    }
    else if (lodLevel != QMeshNode::lodLevel())
        m_model->undoStack()->push(new QXmlNodePropertyCommand("Set Lod Level", this, "Lod_Level", QVariant::fromValue(lodLevel), LodLevelID ));

}

void QMeshNode::activate()
{
    float minX, minY, minZ, maxX, maxY, maxZ;
    h3dGetNodeAABB(m_hordeID, &minX, &minY, &minZ, &maxX, &maxY, &maxZ);

    unsigned int cameraID = HordeSceneEditor::instance()->glContext()->activeCam();
    float leftPlane = h3dGetNodeParamF(cameraID, H3DCamera::LeftPlaneF, 0);
    float rightPlane = h3dGetNodeParamF(cameraID, H3DCamera::RightPlaneF, 0);
    float bottomPlane = h3dGetNodeParamF(cameraID, H3DCamera::BottomPlaneF, 0);
    float topPlane = h3dGetNodeParamF(cameraID, H3DCamera::TopPlaneF, 0);
    float nearPlane = h3dGetNodeParamF(cameraID, H3DCamera::NearPlaneF, 0);

    const float* camera = 0;
    H3DNode parentNode = h3dGetNodeParent(cameraID);
    h3dGetNodeTransMats(parentNode, 0, &camera);
    if ( !camera) return;

    /**
         * (maxX - minX)                           =   width of the bounding box
         * rightPlane / (rightPlane - leftPlane)   =   right fraction of the viewing frustum
         */
    float offsetX = (maxX - minX) * rightPlane / (rightPlane - leftPlane);
    float offsetY = (maxY - minY) * topPlane / (topPlane - bottomPlane);
    QMatrix4f newCamTrans = QMatrix4f::TransMat(maxX - offsetX, maxY - offsetY, maxZ);
    newCamTrans.translate(0, 0, qMax(nearPlane * offsetX / rightPlane, nearPlane * offsetY / topPlane));
    h3dSetNodeTransMat(cameraID, (QMatrix4f(camera).inverted() * newCamTrans).x);
}
