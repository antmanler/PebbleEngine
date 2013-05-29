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


#include <QXmlTree/QXmlNodePropertyCommand.h>

#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QWizard>

#include <Horde3D.h>
#include <Horde3DUtils.h>
#include <Horde3DSph.h>

#include <SceneEditorCore/PlugInManager.h>
#include <SceneEditorCore/AttachmentPlugIn.h>
#include <SceneEditorCore/SceneFile.h>
#include <SceneEditorCore/SceneTreeModel.h>
#include <SceneEditorCore/EditorUtils.h>

#include "QSphNodePage.h"
#include "QSphNode.h"

QDomElement QSphNode::createNode(QWidget* parent)
{
    QDomDocument sphNode;
    QWizard wizard(parent);
    wizard.addPage(new QSphNodePage(&wizard));
//	wizard.adjustSize();
    if (wizard.exec() == QDialog::Accepted)
    {
        sphNode.setContent( QString( "<Sph/>" ) );
        sphNode.documentElement().setAttribute("name", wizard.field("name").toString());
        sphNode.documentElement().setAttribute("material", wizard.field("material").toString());
        sphNode.documentElement().setAttribute("volumeData", wizard.field("voldatafile").toString());
    }
    return sphNode.documentElement();
}

QSceneNode* QSphNode::loadNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parentNode)
{
    return new QSphNode(xmlNode, row, model, parentNode);
}

QSphNode::QSphNode(const QDomElement& xmlNode, int row, SceneTreeModel* model, QSceneNode* parentNode) :
    QSceneNode(xmlNode, row, model, parentNode), m_volDataID(0), m_materialID(0)
{
    setObjectName("Sph");
    addRepresentation();
}

QSphNode::~QSphNode()
{
    if (m_volDataID != 0)
        h3dRemoveResource(m_volDataID);
    if (m_materialID != 0)
        h3dRemoveResource(m_materialID);
}


Material QSphNode::material() const
{
    return Material(m_xmlNode.attribute("material"));
}

void QSphNode::setMaterial(const Material& material)
{
    if (signalsBlocked())
    {
        m_xmlNode.setAttribute("material", material.FileName);
        h3dRemoveResource(m_materialID);
        m_materialID = h3dextCreateSphMaterialResource( qPrintable(material.FileName), 0 );
		//m_materialID = h3dAddResource( H3DResTypes::Material, qPrintable(material.FileName), 0 );
//        h3dutLoadResourcesFromDisk(".");
        SceneEditor::load_h3dres_from_file_qrc(".");
        h3dSetNodeParamI(m_hordeID, H3DEXTSph::MatResI, m_materialID);
    }
    else if (material != QSphNode::material())
        m_model->undoStack()->push(new QXmlNodePropertyCommand("Set Material", this, "Material", QVariant::fromValue(material), SPH_VOLDATA_ID));
}

VolData QSphNode::volData() const
{
    return vol_data_;
}

void QSphNode::setVolData(const VolData& vol)
{
    if (signalsBlocked()) {
        m_xmlNode.setAttribute("volumeData", vol.fileName());
        if (vol.fileName() == vol_data_.fileName() && vol.density() != vol_data_.density())
            vol_data_.setDensity(vol.density());
        vol_data_ = VolData(m_xmlNode.attribute("volumeData"));

        h3dRemoveResource(m_volDataID);
        h3dReleaseUnusedResources(H3DResTypes::Undefined);
        m_volDataID = h3dAddResource( H3DEXT_ResType_Sph, qPrintable(vol.fileName()), 0 );
//        h3dutLoadResourcesFromDisk(".");
        SceneEditor::load_h3dres_from_file_qrc(".");
        h3dSetNodeParamI(m_hordeID, H3DEXTSph::VolumeResI, m_volDataID);

    }
    else if (vol != QSphNode::volData())
        m_model->undoStack()->push(new QXmlNodePropertyCommand("Set VolumeData", this, "VolumeData", QVariant::fromValue(vol), SPH_VOLDATA_ID));
}

void QSphNode::addRepresentation()
{
    m_volDataID = h3dAddResource( H3DEXT_ResType_Sph, qPrintable(m_xmlNode.attribute("volumeData")), 0 );
//    m_materialID = h3dAddResource(H3DResTypes::Material, qPrintable(m_xmlNode.attribute("material")), 0);
    m_materialID = h3dextCreateSphMaterialResource( qPrintable(m_xmlNode.attribute("material")), 0 );

    // Load resource immediately since a later call to loadResourceFromDisk results in a bad behaviour of the Horde3D engine
    QDir volDir( QDir::current() );
    QString frfilename = volDir.absoluteFilePath(m_xmlNode.attribute("volumeData"));
    QFile file(frfilename);
    if (!file.open(QIODevice::ReadOnly)) qWarning("Error opening resource %s", qPrintable(frfilename));

    // Stupid return value, if false it can also be the case that the resource was loaded before instead of that their was an error
    h3dLoadResource(m_volDataID, file.readAll().constData(), file.size());
    file.close();
    vol_data_ = VolData(m_xmlNode.attribute("volumeData"));

    QSceneNode* parentNode = static_cast<QSceneNode*>(parent());
    unsigned int rootID = parentNode ? parentNode->hordeId() : H3DRootNode;

    m_hordeID = h3dextAddSphNode(rootID, qPrintable(m_xmlNode.attribute("name", "ATTENTION No Node Name")), m_volDataID, m_materialID);
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

