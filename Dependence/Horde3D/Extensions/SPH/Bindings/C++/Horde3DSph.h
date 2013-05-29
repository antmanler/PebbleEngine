// *************************************************************************************************
//
// Bullet Physics Integration into Horde3D 
// --------------------------------------
// Copyright (C) 2007 Volker Wiendl
//
// Updated to Horde3D v1.0 beta4 by Afanasyev Alexei and Giatsintov Alexander
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// *************************************************************************************************

#pragma once

#include "Horde3D.h"

#ifndef DLL
#	if defined( WIN32 ) || defined( _WINDOWS )
#		define DLL extern "C" __declspec( dllimport )
#	else
#		if defined( __GNUC__ ) && __GNUC__ >= 4
#		  define DLLEXP extern "C" __attribute__ ((visibility("default")))
#   	else
#		  define DLLEXP extern "C"
#   	endif
#	endif
#endif

const int H3DEXT_NodeType_Sph = 300;

const int H3DEXT_ResType_Sph = 100;

typedef void ScaleFieldGenerateCallBack(const float* points, const int points_num,
	const float xmin, const float ymin, const float zmin,
	const float xmax, const float ymax, const float zmax,
	float* saclefiled, const int width, const int height, const int depth);

struct H3DEXTSph
{
  enum List
    {
        VolumeResI = 20000,
        MatResI
    };
};

struct H3DEXTSphRes
{
    enum List
    {
        ParticleElem = 10000,
        ParticleNumI,
        SampleDistanceF,
        DensityF,
        RigidI
    };
};

/* Function: h3dextCreate3DTexture
 create a 3D texture resource.
 
 Details:
 This function tries to create and add a Texture resource with the specified name to the resource
 manager. If a Texture resource with the same name is already existing, the function fails. The
 texture is initialized with the specified dimensions and the resource is declared as loaded. This
 function is especially useful to create dynamic textures (e.g. for displaying videos) or output buffers
 for render-to-texture.
 
 Parameters:
 name        - name of the resource
 width       - width of the texture image
 height      - height of the texture image
 depth       - depth of the texture image
 fmt         - texture format (see stream formats)
 flags       - flags used for creating the resource
 
 Returns:
 handle to the created resource or 0 in case of failure
 */

DLL H3DRes h3dextCreate3DTexture( const char *name, int width, int height, int depth, int fmt, int flags );

/* Function: h3dextAddSphNode
                Adds a Terrain node to the scene.

        Details:
                This function creates a new Sph node and attaches it to the specified parent node.

        Parameters:
                parent        - handle to parent node to which the new node will be attached
                name          - name of the node
                volDataRes    -
                materialRes   - handle to the Material resource used for rendering the terrain

        Returns:
                 handle to the created node or 0 in case of failure
*/
DLL H3DNode h3dextAddSphNode( H3DNode parent, const char *name, H3DRes volDataRes, H3DRes materialRes );

DLL H3DRes h3dextCreateSphMaterialResource(const char *name, int flags);

DLL bool h3dextSetSphScaleFieldGenerateCallback(H3DNode sphnode, ScaleFieldGenerateCallBack* callback);

DLL bool h3dextUpdateSphData(H3DRes sphres, const char *data, const int size);

DLL bool h3dextUpdateSphDataFromFile(H3DRes sphres, const char *filename);

DLL void h3dextUpdateSphNode(H3DNode sphnode) ;



