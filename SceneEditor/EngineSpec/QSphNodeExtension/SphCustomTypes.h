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

#include <QtCore/QVariant>
#include <SceneEditorCore/Config.h>

class Property;
class QObject;

struct VolData
{
    VolData() {}
    VolData(const QString& fileName)
        : filename_(fileName)
        , particel_size_(0)
        , sample_dist_(0)
        , density_(0)
    {
        updateFrProperty();
    }

    QString fileName() const {return filename_;}
    void setFileName(const QString& filename) { filename_ = filename; updateFrProperty(); }

    int particelSize() const {return particel_size_; }
    float sampleDist() const {return sample_dist_;}

    float density() const {return density_;}
    void setDensity(const float density);

    bool isRigid() const {return is_rigid_;}

    QString com() const {return com_;}

    bool operator == (const VolData& other) const {return filename_ == other.filename_ && density_ == other.density_;}
    bool operator != (const VolData& other) const {return filename_ != other.filename_ || density_ != other.density_;}

private:
    void updateFrProperty();

private:
    QString filename_;
    QString com_;
    int   particel_size_;
    float sample_dist_;
    float density_;
    bool  is_rigid_;

};
Q_DECLARE_METATYPE(VolData)

namespace SphCustomTypes
{
void registerTypes();
Property* createCustomProperty(const QString& name, QObject* propertyObject, Property* parent);

}




