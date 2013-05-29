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

#include "QPipelineSetupNode.h"

#include "QPipelineRenderTargetNode.h"
#include "QPipelineNode.h"

QPipelineSetupNode::QPipelineSetupNode(const QDomElement& xmlNode, int row, QXmlTreeModel* model, QXmlTreeNode* parent /*= 0*/) : 
QXmlTreeNode(xmlNode, row, model, parent)
{
	m_knownNodeNames << "RenderTarget";
	// Create special render target for main render buffer
	m_xmlNode.insertBefore( m_xmlNode.ownerDocument().createElement("RenderTarget"), QDomElement());
	setProperty("Type", QPipelineNode::PIPELINESETUPNODE);
}


QPipelineSetupNode::~QPipelineSetupNode()
{
}


QXmlTreeNode* QPipelineSetupNode::createChild(const QDomElement& childNode, int row)
{
	// since known nodes only contains RenderTarget, we can assume that the childNode is a RenderTarget node
	return new QPipelineRenderTargetNode(childNode, row, m_model, this);
}