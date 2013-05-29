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
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <SceneEditorCore/QPropertyEditor/Property.h>

#include "VolDataProperty.h"
#include "SphCustomTypes.h"

namespace SphCustomTypes
{
void registerTypes()
{
    //CustomTypes::registerTypes();
    static bool registered = false;
    if (!registered) {
        qRegisterMetaType<VolData>("VolData");
        registered = true;
    }
}

Property* createCustomProperty(const QString& name, QObject* propertyObject, Property* parent)
{
    int userType = propertyObject->property(qPrintable(name)).userType();
    if (userType == QMetaType::type("VolData")) return new VolDataProperty(name, propertyObject, parent);
    else return 0;
}

}

void VolData::setDensity(const float density)
{
    // update fr files
    QFileInfo frfile(QDir::current().absoluteFilePath(filename_));
    if (frfile.exists()) {
        QFile fr(frfile.absoluteFilePath());
        if (fr.open(QIODevice::ReadWrite)) {
            unsigned int pcount = 0;
            fr.read(reinterpret_cast<char*>(&pcount), sizeof(unsigned int));
            density_ = density;
            // seek for sample distance
            fr.seek(fr.pos() + 3*pcount*sizeof(float));
            fr.write(reinterpret_cast<char*>(&sample_dist_), sizeof(float));
            fr.write(reinterpret_cast<char*>(&density_), sizeof(float));
            fr.close();

            return;
        }
    }
}

void VolData::updateFrProperty()
{
    // display particle info
//    QVariant data = Property::value();
    QFileInfo frfile(QDir::current().absoluteFilePath(filename_));
    if (frfile.exists()) {
        QFile fr(frfile.absoluteFilePath());
        if (fr.open(QIODevice::ReadOnly)) {
            unsigned int pcount = 0;
            fr.read(reinterpret_cast<char*>(&pcount), sizeof(unsigned int));
            particel_size_ = pcount;
            sample_dist_ = 0.0f;
            density_ = 0.0f;
            is_rigid_ = (sizeof(int) + 2*3*pcount*sizeof(float) + 2*sizeof(float)) <= fr.size();

            com_ = QString("N/A");
            if (is_rigid_) {
                // update center of mass
                float *buf = new float[pcount*3];
                fr.read(reinterpret_cast<char*>(buf), 3*pcount*sizeof(float));

                float xacc = 0.f, yacc = 0.f, zacc = 0.f;
                for (int i = 0; i < pcount; ++ i) {
                    xacc += buf[i*3+0];
                    yacc += buf[i*3+1];
                    zacc += buf[i*3+2];
                }
                delete []buf;

                com_ = QString("%1, %2, %3").arg(xacc/float(pcount), 0, 'g', 4)
                       .arg(yacc/float(pcount), 0, 'g', 4)
                       .arg(zacc/float(pcount), 0, 'g', 4);
                fr.seek(sizeof(unsigned int));
            }

            if (pcount > 0 && 3*pcount*sizeof(float) + sizeof(unsigned int) < fr.size()) {
                // seek for sample distance
                fr.seek(fr.pos() + 3*pcount*sizeof(float));
                fr.read(reinterpret_cast<char*>(&sample_dist_), sizeof(float));
                fr.read(reinterpret_cast<char*>(&density_), sizeof(float));
            }
            fr.close();
            return;
        }
    }
    sample_dist_ =  0.0;
    density_ = 0.0;
}
