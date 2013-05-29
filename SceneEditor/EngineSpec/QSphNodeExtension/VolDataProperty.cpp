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

#include <QtCore/QDir>
//#include "CustomTypes.h"
#include "VolDataComboBox.h"
#include "VolDataProperty.h"

VolDataProperty::VolDataProperty(const QString& name /*= QString()*/, QObject* propertyObject /*= 0*/, QObject* parent /*= 0*/) : Property(name, propertyObject, parent)
{
    is_rigid_ = new Property("IsRigid", this, this);
    particle_size_ = new Property("ParticleSize", this, this);
    sample_dist_ = new Property("SampleDistance", this, this);
    com_ = new Property("CoM", this, this);
    density_ = new Property("Density", this, this);
    density_->setEditorHints("minimum=0;maximum=10000");   
}


VolDataProperty::~VolDataProperty()
{
}

QVariant VolDataProperty::value(int role) const
{
    QVariant data = Property::value();
    if (data.isValid() && role == Qt::DisplayRole)
        return data.value<VolData>().fileName();
    return data;
}

//void VolDataProperty::setValue(const QVariant &value)
//{
////    if (value.type() == QVariant::String) {
////    } else {
////        Property::setValue(value);
////        QVariant data = Property::value();
////        if (data.isValid()) {
////            density_->setProperty("Density", data.value<VolData>().density());
////            sample_dist_->setProperty("SampleDistance", data.value<VolData>().sampleDist());
////        }
////    }
//}

QWidget* VolDataProperty::createEditor(QWidget *parent, const QStyleOptionViewItem& /*option*/)
{
    VolDataComboBox* editor = new VolDataComboBox(parent);
    editor->init( QDir::currentPath() );
    return editor;
}

QVariant VolDataProperty::editorData(QWidget *editor)
{
    return QVariant::fromValue(static_cast<VolDataComboBox*>(editor)->volData());
}

bool VolDataProperty::setEditorData(QWidget *editor, const QVariant &data)
{
    VolDataComboBox* ed = qobject_cast<VolDataComboBox*>(editor);
    if (ed) static_cast<VolDataComboBox*>(editor)->setVolData(data.value<VolData>());
    return true;
}

int VolDataProperty::particle_size() const
{
    return value().value<VolData>().particelSize();
}

float VolDataProperty::sample_dist() const
{
    return value().value<VolData>().sampleDist();
}

bool VolDataProperty::is_rigid() const
{
    return value().value<VolData>().isRigid();
}

float VolDataProperty::density() const
{
    return value().value<VolData>().density();
}

void VolDataProperty::setDensity(const float value)
{
    VolData data (Property::value().value<VolData>().fileName());
    data.setDensity(value);
    Property::setValue(QVariant::fromValue(data));
}

QString VolDataProperty::com() const
{
    return value().value<VolData>().com();
}
