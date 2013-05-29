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

#include "QLightNodePage.h"

#include <SceneEditorCore/HordeFileDialog.h>
#include "HordeSceneEditor.h"

#include <Horde3D.h>

QLightNodePage::QLightNodePage(QWidget* parent /*= 0*/) : QWizardPage(parent)
{
	setupUi(this);
	setTitle(tr("Add new light"));
	setSubTitle(tr("Create a new light source for the current scene"));
	registerField("lightfov", m_fov, "value", SIGNAL(valueChanged(double)));
	registerField("radius", m_radius, "value", SIGNAL(valueChanged(double)));
	registerField("lightname", m_name);
	registerField("shadowmapbias", m_shadowMapBias, "value", SIGNAL(valueChanged(double)));
	registerField("shadowcontext", m_shadowContext);
	registerField("lightingcontext", m_lightContext);	
	registerField("material", m_material, "materialFile", SIGNAL(materialChanged()));	
	registerField("slices", m_slices);
	registerField("lambda", m_lambda);
		
}


QLightNodePage::~QLightNodePage()
{
}

void QLightNodePage::initializePage()
{
	m_material->init( QDir::currentPath() );	
}



