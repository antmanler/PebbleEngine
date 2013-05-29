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
#ifndef SHADERDATA_H_
#define SHADERDATA_H_

#include <QtCore/QStringList>
#include "QUniform.h"
#include "Config.h"

struct DLLEXP ShaderFlag
{
    int		Flag;
    QString	Name;

    bool operator==(const ShaderFlag& flag ) const { return Flag == flag.Flag; }
};



class DLLEXP ShaderData : public QObject
{
public:
    ShaderData( QObject* parent = 0) : QObject(parent), m_valid( false ) {}
    ShaderData( const QByteArray& data, QObject* parent = 0 );

    ~ShaderData();

    bool isValid() const { return m_valid; }
    QString lastError() const { return m_lastError; }

    const QStringList& includeFiles() { return m_includedFiles; }
    const QStringList& textureFiles() { return m_textureFiles; }

    const QList<ShaderFlag>& flags() { return m_flags; }

private:
    bool loadShader( const QByteArray& data );
    bool loadCode( const QByteArray& data );
    bool loadFX( const QByteArray& data );

    bool raiseError( const QString& error );
    QList<ShaderFlag>			m_flags;
    QStringList					m_includedFiles;
    QStringList					m_textureFiles;
    QString						m_lastError;
    bool						m_valid;


};


#endif
