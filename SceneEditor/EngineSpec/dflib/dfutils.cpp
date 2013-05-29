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

#include <map>
#include <vector>
#include <math.h>
#include <Horde3D/Horde3D.h>
#include <Horde3D/Horde3DTerrain.h>
#include "matrix4x4f.h"

#include "dfutils.h"

namespace dfut
{

void appendH3DGeomToMesh(int hid, Mesh &mesh)
{

    // filter mesh
    vector<int> geom_list;
    switch(h3dGetNodeType(hid)) {
    // add mesh and terrain directly
    case H3DNodeTypes::Mesh:
    case H3DEXT_NodeType_Terrain:
        geom_list.push_back(hid);
        break;
    case H3DNodeTypes::Model:
        // find each mesh
        {
            int nodes = h3dFindNodes(hid, "", H3DNodeTypes::Mesh );
            for (int i = 0; i < nodes; ++i) geom_list.push_back(h3dGetNodeFindResult(i));

        }
        break;
    default: return; // unknwon type
    }

    vector<int>::const_iterator itr = geom_list.begin();
    for(; itr!=geom_list.end(); ++itr) {
        H3DRes geoResource = 0;
        int vertexOffset = 0, indexOffset = 0;
        unsigned int numVertices = 0, numTriangleIndices = 0, vertRStart = 0;

        switch(h3dGetNodeType(*itr)) {
        case H3DNodeTypes::Mesh:
            geoResource = h3dGetNodeParamI(h3dGetNodeParent(*itr), H3DModel::GeoResI);
            numVertices = h3dGetNodeParamI(*itr, H3DMesh::VertREndI) - h3dGetNodeParamI(*itr, H3DMesh::VertRStartI) + 1;
            numTriangleIndices = h3dGetNodeParamI(*itr, H3DMesh::BatchCountI);
            vertRStart = h3dGetNodeParamI(*itr, H3DMesh::VertRStartI);
            vertexOffset = vertRStart * 3;
            indexOffset = h3dGetNodeParamI(*itr, H3DMesh::BatchStartI);
            break;
        case H3DEXT_NodeType_Terrain:
            geoResource = h3dextCreateTerrainGeoRes(*itr, h3dGetNodeParamStr( *itr, H3DNodeParams::NameStr ),
                                                    h3dGetNodeParamF( *itr, H3DEXTTerrain::MeshQualityF, 0) );
            if (0 == geoResource) {
                geoResource = h3dFindResource(H3DResTypes::Geometry, h3dGetNodeParamStr( *itr, H3DNodeParams::NameStr ));
                if(0 == geoResource) {
                    // failed to construct mesh from a terrain node
                    continue;
                }
            }
            numVertices = h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoVertexCountI);
            numTriangleIndices = h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexCountI);
            break;
        default : continue;
        }

        vector<long> idxBase;
        idxBase.reserve(numTriangleIndices);
        //Triangle indices, must cope with 16 bit and 32 bit
        if (h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndices16I)) {
            unsigned short* tb = (unsigned short*)h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexStream, true, false);
            tb += indexOffset;
            for(long i=0; i<numTriangleIndices; ++i) {
                idxBase.push_back(tb[i]);
            }
            h3dUnmapResStream(geoResource);
        } else {
            unsigned int* tb = (unsigned int*)h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexStream, true, false);
            tb += indexOffset;
            for(long i=0; i<numTriangleIndices; ++i) {
                idxBase.push_back(tb[i]);
            }
            h3dUnmapResStream(geoResource);
        }

        // get vertex base address
        float* vertexBase = (float*) h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoVertPosStream, true, false);
        h3dUnmapResStream(geoResource);
        if( vertexBase ) {
            vertexBase += vertexOffset;
            // get transform matrix
            const float* x = 0;
            h3dGetNodeTransMats(*itr, 0, &x);
            matrix4x4f mat(x);
            // assemble the vertices
            vector<float> verts; verts.reserve(numVertices*3);
            for(int i=0; i<numVertices; ++i) {
                vector3f vert(vertexBase[i*3], vertexBase[i*3+1], vertexBase[i*3+2]);
                mat.transformPoint(&vert);
                verts.push_back(vert.x); verts.push_back(vert.y); verts.push_back(vert.z);
            }

            mesh.pushGemo(&verts[0], numVertices, &idxBase[0], numTriangleIndices/3, vertRStart);
        }
    }
}

}
