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
#pragma once

#include <SceneEditorCore/QPropertyEditor/Property.h>
#include <SceneEditorCore/Config.h>

class VolDataProperty : public Property
{
    Q_OBJECT
    Q_PROPERTY(int ParticleSize READ particle_size DESIGNABLE true USER true)
    Q_PROPERTY(float SampleDistance READ sample_dist DESIGNABLE true USER true)
    Q_PROPERTY(bool IsRigid READ is_rigid DESIGNABLE true USER true)
    Q_PROPERTY(QString CoM READ com DESIGNABLE true USER true)
    Q_PROPERTY(float Density READ density WRITE setDensity DESIGNABLE true USER true)
//    Q_CLASSINFO("Density", "minimum=0;maximum=10000")

public:
    VolDataProperty(const QString& name = QString(), QObject* propertyObject = 0, QObject* parent = 0);
    virtual ~VolDataProperty();

    QVariant value(int role = Qt::UserRole) const;
//    void setValue(const QVariant& value);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option);

    QVariant editorData(QWidget *editor);
    bool setEditorData(QWidget *editor, const QVariant& data);

    int particle_size() const;
    float sample_dist() const ;
    bool is_rigid() const;
//    void setSampleDist(const float);

    float density() const;
    void setDensity(const float value);

    QString com() const;

private:
    Property* particle_size_;
    Property* sample_dist_;
    Property* is_rigid_;
    Property* density_;
    Property* com_;
};
