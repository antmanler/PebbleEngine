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

#include "EffectProperty.h"
#include "EffectComboBox.h"
#include "CustomTypes.h"

#include <QtCore/QDir>

#include <Horde3D.h>
#include <Horde3DUtils.h>

EffectProperty::EffectProperty(const QString& name /*= QString()*/, QObject* propertyObject /*= 0*/, QObject* parent /*= 0*/) : Property(name, propertyObject, parent)
{	
}


EffectProperty::~EffectProperty()
{
}

QVariant EffectProperty::value(int role) const
{
	QVariant data = Property::value();
	if (data.isValid() && role == Qt::DisplayRole)	
		return data.value<Effect>().FileName;	
	return data;
}

QWidget* EffectProperty::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/)
{
	EffectComboBox* editor = new EffectComboBox(parent);
	editor->init( QDir::currentPath() );
	return editor;
}

QVariant EffectProperty::editorData(QWidget *editor)
{
	return QVariant::fromValue(static_cast<EffectComboBox*>(editor)->effect());
}

bool EffectProperty::setEditorData(QWidget *editor, const QVariant &data)
{
	EffectComboBox* ed = qobject_cast<EffectComboBox*>(editor);
	if (ed)
		static_cast<EffectComboBox*>(editor)->setEffect(data.value<Effect>());
	return true;
}
