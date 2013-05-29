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

#ifndef SCENEWIZARD_H_
#define SCENEWIZARD_H_

#include <QtGui/QWizard>
#include <QtXml/qdom.h>

class SceneFile;
class QLineEdit;

/**
 * Different Wizards for creating a new SceneFile or new scene elements
 */
namespace SceneWizard
{
	/**
	* Displays a Wizard for creating a new SceneFile
	* @param parent optional parent widget the wizard will be shown in top of
	* @return SceneFile* pointer to a newly created SceneFile (take care of deleting it) or Null if the user canceled the wizard
    */
	SceneFile* createScene(QWidget* parent = 0);

	/**
	* Displays a Wizard for creating a QReferenceNode
	* @param parent pointer to a parent widget the wizard will be shown in top of
	* @return QDomElement the newly created Xml Node or QDomElement() (Null) if user canceled the wizard
    */
	QDomElement createReference(QWidget* parent);

	/**
	* Displays a Wizard for creating a QLightNode
	* @param parent pointer to a parent widget the wizard will be shown in top of	
	* @return QDomElement the newly created Xml Node or QDomElement() (Null) if user canceled the wizard
    */
	QDomElement createLight(QWidget* parent);

	/**
	* Displays a Wizard for creating a QCameraNode
	* @param parent pointer to a parent widget the wizard will be shown in top of
	* @return QDomElement the newly created Xml Node or QDomElement() (Null) if user canceled the wizard
    */
	QDomElement createCamera(QWidget* parent);

	/**
	* Displays a Wizard for creating a QGroupNode
	* @param parent pointer to a parent widget the wizard will be shown in top of
	* @return QDomElement the newly created Xml Node or QDomElement() (Null) if user canceled the wizard
    */
	QDomElement createGroup(QWidget* parent);

	/**
	 * Displays a Wizard for creating an OverlayNode
	 * @param parent pointer to a parent widget the wizard will be shown in top of
	 * @return QDomElement the newly created Xml Node or QDomElement() (Null) if user canceled the wizard 
	 */
	QDomElement createOverlay(QWidget* parent);


	/**
	* Displays a Wizard for creating an QEmitterNode
	* @param parent pointer to a parent widget the wizard will be shown in top of
	* @return QDomElement the newly created Xml Node or QDomElement() (Null) if user canceled the wizard
    */
	QDomElement createEmitter(QWidget* parent);



}

#endif

