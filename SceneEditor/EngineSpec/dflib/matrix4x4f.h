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
#ifndef __MATRIX4X4F_H__
#define __MATRIX4X4F_H__

#include "vector3f.h"

#define DEGTORAD(degree) ((degree) * 0.01745329252f)
#define RADTODEG(radian) ((radian) * 57.295779513f)

class matrix4x4f {
public:

    union
    {
        float c[4][4];	// Column major order for OpenGL: c[column][row]
        float m[16];
    };

    matrix4x4f()  { identity(); }
    matrix4x4f(const float * mm) {
        for (int i=0; i<16; i++) {
            m[i]  = mm[i];
        }
    }

    matrix4x4f( float m0, float m4, float  m8, float m12,
                float m1, float m5, float  m9, float m13,
                float m2, float m6, float m10, float m14,
                float m3, float m7, float m11, float m15 );

    void identity(void);
    void translate(const vector3f &trans);
    void translate_x(const float &dist);
    void translate_y(const float &dist);
    void translate_z(const float &dist);
    void rotate(const float angle, vector3f &axis);
    void rotate_x(const float &angle);
    void rotate_y(const float &angle);
    void rotate_z(const float &angle);
    void scale(const vector3f &scale);
    float transformPoint( vector3f *vec ) const;
    float transformVector( vector3f *vec ) const;
    float getDeterminant() const;
    float getInfNorm() const;
    matrix4x4f transpose() const;

    void print() const;

    static matrix4x4f invertMatrix( const matrix4x4f *matIn );

    matrix4x4f operator + (const matrix4x4f &other);
    matrix4x4f operator - (const matrix4x4f &other);
    matrix4x4f operator * (const matrix4x4f &other);
    matrix4x4f operator * (const float scalar);
};


#endif

