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

#include "PipelineProperty.h"
#include "PipelineComboBox.h"
#include "CustomTypes.h"

#include <QtCore/QDir>

PipelineProperty::PipelineProperty(const QString& name /*= QString()*/, QObject* propertyObject /*= 0*/, QObject* parent /*= 0*/) : Property(name, propertyObject, parent)
{	
}


PipelineProperty::~PipelineProperty()
{
}

QVariant PipelineProperty::value(int role) const
{
	QVariant data = Property::value();
	if (data.isValid() && role == Qt::DisplayRole)	
		return data.value<Pipeline>().FileName;	
	return data;
}

QWidget* PipelineProperty::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/)
{
	PipelineComboBox* editor = new PipelineComboBox(parent);
	editor->init( QDir::currentPath() );
	return editor;
}

QVariant PipelineProperty::editorData(QWidget *editor)
{
	return QVariant::fromValue(static_cast<PipelineComboBox*>(editor)->pipeline());
}

bool PipelineProperty::setEditorData(QWidget *editor, const QVariant &data)
{
	PipelineComboBox* ed = qobject_cast<PipelineComboBox*>(editor);
	if (ed)
		static_cast<PipelineComboBox*>(editor)->setPipeline(data.value<Pipeline>());
	return true;
}
