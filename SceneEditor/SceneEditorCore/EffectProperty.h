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

#ifndef EFFECTPROPERTY_H_
#define EFFECTPROPERTY_H_

#include <SceneEditorCore/Config.h>
#include "QPropertyEditor/Property.h"

class DLLEXP EffectProperty : public Property
{
	Q_OBJECT
public:
	EffectProperty(const QString& name = QString(), QObject* propertyObject = 0, QObject* parent = 0);
	virtual ~EffectProperty();

	QVariant value(int role = Qt::UserRole) const;

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option);
	
	QVariant editorData(QWidget *editor);
	bool setEditorData(QWidget *editor, const QVariant& data);


};
#endif
