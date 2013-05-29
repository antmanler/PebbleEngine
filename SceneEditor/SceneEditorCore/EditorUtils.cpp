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

#include <vector>
#include <string>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <Horde3D/Horde3D.h>
#include <Horde3D/Horde3DUtils.h>
#include "EditorUtils.h"

namespace
{
using namespace std;
string cleanPath( string path )
{
    // Remove spaces at the beginning
    int cnt = 0;
    for( int i = 0; i < (int)path.length(); ++i ) {
        if( path[i] != ' ' ) break;
        else ++cnt;
    }
    if( cnt > 0 ) path.erase( 0, cnt );

    // Remove slashes, backslashes and spaces at the end
    cnt = 0;
    for( int i = (int)path.length() - 1; i >= 0; --i ) {
        if( path[i] != '/' && path[i] != '\\' && path[i] != ' ' ) break;
        else ++cnt;
    }

    if( cnt > 0 ) path.erase( path.length() - cnt, cnt );

    return path;
}
}

namespace SceneEditor
{

bool DLLEXP load_h3dres_from_file_qrc( const char* contentDir )
{
    using namespace std;
    bool result = true;

    string dir;
    vector< string > dirs;

    // Split path string
    char *c = (char *)contentDir;
    do {
        if( *c != '|' && *c != '\0' )
            dir += *c;
        else {
            dir = cleanPath( dir );
            if( dir != "" ) dir += '/';
            dirs.push_back( dir );
            dir = "";
        }
    } while( *c++ != '\0' );

    // Get the first resource that needs to be loaded
    int res = h3dQueryUnloadedResource( 0 );

    char *dataBuf = 0;
    int bufSize = 0;

    while( res != 0 ) {

        // Loop over search paths and try to open files
        QFileInfo fileinfo ;
        for( unsigned int i = 0; i < dirs.size(); ++i ) {
            string fileName = dirs[i] + string(h3dutGetResourcePath(h3dGetResType(res))) + "/" + h3dGetResName( res );
            fileinfo = QFileInfo(fileName.c_str());
            if( fileinfo.exists() ) break;
        }

        // find in qrc
        if (!fileinfo.exists())
            fileinfo = QFileInfo(":/h3dres/" + QString::fromAscii(h3dGetResName(res)));
//        QString filename = ":/h3dres/" + QString::fromAscii(h3dGetResName(res))  ;

        QFile inf(fileinfo.absoluteFilePath());

        // Open resource file
        if( inf.open(QIODevice::ReadOnly) ) {
            // Find size of resource file
            int fileSize = inf.size();

            if( bufSize < fileSize  ) {
                delete[] dataBuf;
                dataBuf = new char[fileSize];
                if( !dataBuf ) {
                    bufSize = 0;
                    continue;
                }
                bufSize = fileSize;
            }
            if( fileSize == 0 )	continue;
            // Copy resource file to memory
            inf.read( dataBuf, fileSize );
            inf.close();
            // Send resource data to engine
            result &= h3dLoadResource( res, dataBuf, fileSize );
        } else  {
            // Tell engine to use the dafault resource by using NULL as data pointer
            h3dLoadResource( res, 0x0, 0 );
            result = false;
        }
        // Get next unloaded resource
        res = h3dQueryUnloadedResource( 0 );
    }
    delete[] dataBuf;

    return result;
}

}

