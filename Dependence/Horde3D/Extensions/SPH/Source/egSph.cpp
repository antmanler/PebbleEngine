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

#include <limits>
#include "egSph.h"
#include "egModules.h"
#include "egCom.h"
#include "egRenderer.h"
#include "egMaterial.h"
#include "egCamera.h"
#include "utXML.h"
#include "utDebug.h"

// for geometry shaders
namespace
{

using namespace Horde3DSph;
using namespace std;

// GL_EXT_geometry_shader4
#define GL_GEOMETRY_SHADER_EXT                             0x8DD9

#define GL_GEOMETRY_VERTICES_OUT_EXT                       0x8DDA
#define GL_GEOMETRY_INPUT_TYPE_EXT                         0x8DDB
#define GL_GEOMETRY_OUTPUT_TYPE_EXT                        0x8DDC

#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT             0x8C29
#define GL_MAX_GEOMETRY_VARYING_COMPONENTS_EXT              0x8DDD
#define GL_MAX_VERTEX_VARYING_COMPONENTS_EXT                0x8DDE
#define GL_MAX_VARYING_COMPONENTS_EXT                       0x8B4B
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT              0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT                 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT         0x8DE1

#define GL_LINES_ADJACENCY_EXT                              0xA
#define GL_LINE_STRIP_ADJACENCY_EXT                         0xB
#define GL_TRIANGLES_ADJACENCY_EXT                          0xC
#define GL_TRIANGLE_STRIP_ADJACENCY_EXT                     0xD

// GL_EXT_texture_integer
#define GL_ALPHA16I_EXT                                    0x8D8A
#define GL_ALPHA_INTEGER_EXT                               0x8D97

// GL_ARB_texture_float
#define GL_ALPHA32F_ARB                                    0x8816


//void ProgramParameteriEXT(uint program, enum pname, int value);
typedef void (GLAPIENTRYP PFNGLPROGRAMPARAMETERIEXTPROC) (GLuint program, GLenum pname, GLint value);
PFNGLPROGRAMPARAMETERIEXTPROC glProgramParameteriEXT = 0x0;

bool gIsGeoShaderSupported = false;
bool gIsInitialised = false;

bool isExtensionSupported( const char *extName )
{
    if( glExt::majorVersion < 3 )
    {
        const char *extensions = (char *)glGetString( GL_EXTENSIONS );
        size_t nameLen = strlen( extName );
        const char *pos;
        while( ( pos = strstr( extensions, extName ) ) != 0x0 )
        {
            char c = pos[nameLen];
            if( c == ' ' || c == '\0' ) return true;
            extensions = pos + nameLen;
        }
    }
    else
    {
        GLint numExts;
        glGetIntegerv( GL_NUM_EXTENSIONS, &numExts );
        for( int i = 0; i < numExts; ++i )
        {
            if( strcmp( extName, (char *)glGetStringi( GL_EXTENSIONS, i ) ) == 0 )
                return true;
        }
    }

    return false;
}

void *platGetProcAddress( const char *funcName )
{
#if defined( PLATFORM_WIN )
    return (void *)wglGetProcAddress( funcName );
#elif defined( PLATFORM_WIN_CE )
    return (void *)eglGetProcAddress( funcName );
#elif defined( PLATFORM_MAC )
    CFStringRef functionName = CFStringCreateWithCString( kCFAllocatorDefault, funcName, kCFStringEncodingASCII );
    CFURLRef bundleURL = CFURLCreateWithFileSystemPath(
                             kCFAllocatorDefault, CFSTR( "/System/Library/Frameworks/OpenGL.framework" ), kCFURLPOSIXPathStyle, true );
    CFBundleRef bundle = CFBundleCreate( kCFAllocatorDefault, bundleURL );

    void *function = CFBundleGetFunctionPointerForName( bundle, functionName );

    CFRelease( bundle );
    CFRelease( bundleURL );
    CFRelease( functionName );

    return function;
#else
    return (void *)glXGetProcAddressARB( (const GLubyte *)funcName );
#endif
}



// hack for update shader combination after injection
vector<pair<int, string> > gGetShaderCombCustomUniformNames(Horde3D::ShaderCombination &sc)
{
    map<int, string> name_map;
    uint32 program_id = gRDI->getShaderNativeHandler(sc.shaderObj);

    gRDI->bindShader( sc.shaderObj );

    int total = -1;
    glGetProgramiv( program_id, GL_ACTIVE_UNIFORMS, &total );
    for(int i=0; i<total; ++i)  {
        int name_len=-1, num=-1;
        GLenum type = GL_ZERO;
        char name[100];
        glGetActiveUniform( program_id, GLuint(i), sizeof(name)-1, &name_len, &num, &type, name );
        name[name_len] = 0;
        GLuint location = glGetUniformLocation( program_id, name );
        name_map.insert(make_pair<int, string>(location, name));
    }
    gRDI->bindShader( 0 );

    vector<pair<int, string> > ret;
    vector<int>::const_iterator itr = sc.customSamplers.begin();
    for (int cnt = 0; itr != sc.customSamplers.end(); ++itr, ++cnt) {
        if (*itr < 0) continue;
        map<int, string>::iterator i = name_map.find(*itr);
        if ( i!=name_map.end() )
            ret.push_back( make_pair<int, string>(cnt, i->second) );
    }

    itr = sc.customUniforms.begin();
    for (int cnt = 0; itr != sc.customUniforms.end(); ++itr, ++cnt) {
        if (*itr < 0) continue;
        map<int, string>::iterator i = name_map.find(*itr);
        if ( i!=name_map.end() )
            ret.push_back( make_pair<int, string>(-cnt, i->second) );
    }

    return ret;

}

bool gUpdateShaderComb(Horde3D::ShaderCombination &sc, const vector<pair<int, string> >& old_state )
{
    using namespace Horde3D;
    // Create shader program
    uint32 shdObj = sc.shaderObj;
    if( shdObj == 0 ) return false;

    gRDI->bindShader( shdObj );

    // Set standard uniforms
    int loc = gRDI->getShaderSamplerLoc( shdObj, "shadowMap" );
    if( loc >= 0 ) gRDI->setShaderSampler( loc, 12 );

    // Misc general uniforms
    sc.uni_frameBufSize = gRDI->getShaderConstLoc( shdObj, "frameBufSize" );

    // View/projection uniforms
    sc.uni_viewMat = gRDI->getShaderConstLoc( shdObj, "viewMat" );
    sc.uni_viewMatInv = gRDI->getShaderConstLoc( shdObj, "viewMatInv" );
    sc.uni_projMat = gRDI->getShaderConstLoc( shdObj, "projMat" );
    sc.uni_viewProjMat = gRDI->getShaderConstLoc( shdObj, "viewProjMat" );
    sc.uni_viewProjMatInv = gRDI->getShaderConstLoc( shdObj, "viewProjMatInv" );
    sc.uni_viewerPos = gRDI->getShaderConstLoc( shdObj, "viewerPos" );

    // Per-instance uniforms
    sc.uni_worldMat = gRDI->getShaderConstLoc( shdObj, "worldMat" );
    sc.uni_worldNormalMat = gRDI->getShaderConstLoc( shdObj, "worldNormalMat" );
    sc.uni_nodeId = gRDI->getShaderConstLoc( shdObj, "nodeId" );
    sc.uni_customInstData = gRDI->getShaderConstLoc( shdObj, "customInstData[0]" );
    sc.uni_skinMatRows = gRDI->getShaderConstLoc( shdObj, "skinMatRows[0]" );

    // Lighting uniforms
    sc.uni_lightPos = gRDI->getShaderConstLoc( shdObj, "lightPos" );
    sc.uni_lightDir = gRDI->getShaderConstLoc( shdObj, "lightDir" );
    sc.uni_lightColor = gRDI->getShaderConstLoc( shdObj, "lightColor" );
    sc.uni_shadowSplitDists = gRDI->getShaderConstLoc( shdObj, "shadowSplitDists" );
    sc.uni_shadowMats = gRDI->getShaderConstLoc( shdObj, "shadowMats" );
    sc.uni_shadowMapSize = gRDI->getShaderConstLoc( shdObj, "shadowMapSize" );
    sc.uni_shadowBias = gRDI->getShaderConstLoc( shdObj, "shadowBias" );

    // Particle-specific uniforms
    sc.uni_parPosArray = gRDI->getShaderConstLoc( shdObj, "parPosArray" );
    sc.uni_parSizeAndRotArray = gRDI->getShaderConstLoc( shdObj, "parSizeAndRotArray" );
    sc.uni_parColorArray = gRDI->getShaderConstLoc( shdObj, "parColorArray" );

    // Overlay-specific uniforms
    sc.uni_olayColor = gRDI->getShaderConstLoc( shdObj, "olayColor" );

    vector<pair<int, string> >::const_iterator itr = old_state.begin();
    for (; itr != old_state.end(); ++itr) {
        if ( itr->first > 0 ) {
            // custom sampler
            sc.customSamplers[itr->first] = gRDI->getShaderConstLoc( shdObj, itr->second.c_str() );
        } else {
            // custom uniform
            sc.customUniforms[-itr->first] = gRDI->getShaderConstLoc( shdObj, itr->second.c_str() );
        }
    }

    gRDI->bindShader( 0 );
    return true;
}

// generate marching cube sample points in swizzled style
void gSwizzledWalk(int &n, float *gridData, int pos[3], int size[3], const Horde3D::Vec3f &cubeSize){
    if(size[0]>1) {
        int newSize[3] = { size[0]/2, size[1]/2, size[2]/2 };

        gSwizzledWalk(n, gridData, pos, newSize, cubeSize);

        int pos1[3] =  {pos[0] + newSize[0], pos[1], pos[2]};
        gSwizzledWalk(n, gridData, pos1, newSize, cubeSize);

        int pos2[3] =  {pos[0], pos[1] + newSize[1], pos[2]};
        gSwizzledWalk(n, gridData, pos2, newSize, cubeSize);

        int pos3[3] =  {pos[0] + newSize[0], pos[1] + newSize[1], pos[2]};
        gSwizzledWalk(n, gridData, pos3, newSize, cubeSize);

        int pos4[3] =  {pos[0], pos[1], pos[2] + newSize[2]};
        gSwizzledWalk(n, gridData, pos4, newSize, cubeSize);

        int pos5[3] =  {pos[0] + newSize[0], pos[1], pos[2] + newSize[2]};
        gSwizzledWalk(n, gridData, pos5, newSize, cubeSize);

        int pos6[3] =  {pos[0], pos[1] + newSize[1], pos[2] + newSize[2]};
        gSwizzledWalk(n, gridData, pos6, newSize, cubeSize);

        int pos7[3] =  {pos[0] + newSize[0], pos[1] + newSize[1], pos[2] + newSize[2]};
        gSwizzledWalk(n, gridData, pos7, newSize, cubeSize);
    } else {
        gridData[n]=(pos[0]/cubeSize.x)*2.0f-1.0f;
        gridData[n+1]=(pos[1]/cubeSize.y)*2.0f-1.0f;
        gridData[n+2]=(pos[2]/cubeSize.z)*2.0f-1.0f;
        n+=3;
    }
}

// default scaled field generator for generating density field from points
void gDefScaleFiledGenerator (const float* points, const int points_num,
                              const float xmin, const float ymin, const float zmin,
                              const float xmax, const float ymax, const float zmax,
                              float* saclefiled, const int width, const int height, const int depth)
{
    using namespace Horde3D;

    const float iso_radius = 0.03811f;
#ifndef M_PI
    const float M_PI=3.14159265358979323846f;
#endif
    const float mass = 0.00005f ;
    const float kernel = mass*315.0f/(64.0f*M_PI*pow(iso_radius, 9));
    const int   mc_range = 1;

    // clear data field
    memset(saclefiled, 0, width*height*depth*sizeof(float));

    // compute the stride
    Vec3f extends(xmax - xmin, ymax - ymin, zmax - zmin);
    Vec3f stride3f(extends.x/(width -2.0f*mc_range - 6.0f),
                   extends.y/(height - 2.0f*mc_range - 6.0f),
                   extends.z/(depth - 2.0f*mc_range - 6.0f));

    float stride = stride3f.x;
    stride = maxf(stride, stride3f.y);
    stride = maxf(stride, stride3f.z);
    float inv_stride = 1.0f/stride;

    int min_x = 0;
    int min_y = 0;
    int min_z = 0;

    //    int max_x = extends.x * inv_stride;
    //    int max_y = extends.y * inv_stride;
    //    int max_z = extends.x * inv_stride;

    //    vol_width = max_x + r->mc_range*2+ 6;
    //    vol_height = max_y + r->mc_range*2 + 6;
    //    vol_depth = max_z + r->mc_range*2 + 6;

    Vec3f volpos(xmin - stride, ymin - stride, zmin - stride);
    // position in volume
    Vec3f piv;
    const Vec3f* pos = reinterpret_cast<const Vec3f *>(points);
    for (int i = 0; i < points_num; ++i) {

        piv = (pos[i] - volpos) * inv_stride;

        for (int z = -mc_range; z <= mc_range; z++) {
            for (int y = -mc_range; y <= mc_range; y++) {
                for (int x = -mc_range; x <= mc_range; x++) {

                    float dx = ((int)piv.x)*stride + volpos.x - pos[i].x + stride*x;
                    float dy = ((int)piv.y)*stride + volpos.y - pos[i].y + stride*y;
                    float dz = ((int)piv.z)*stride + volpos.z - pos[i].z + stride*z;

                    float h2_r2 = maxf(iso_radius*iso_radius - (dx*dx + dy*dy + dz*dz), 0.0f);

                    float v = kernel*h2_r2*h2_r2*h2_r2;


                    int vindex = (int)(piv.x + x - min_x + 2)
                                 + ((int)(piv.y + y - min_y + 2))*width
                                 + ((int)(piv.z + z - min_z + 2))*width*height;

                    if ((vindex < 0) || (vindex > width*height*depth)) continue;

                    saclefiled[vindex] += v;
                }
            }
        }
    }

}

///---------------Marching cubes' constant resources---------------
const int gEdgeTable[256] = {
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

const int gTriTable[256][16] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

}


namespace Horde3DSph {

using namespace Horde3D;
using namespace std;

uint32 gEdgeTableTex = 0;
uint32 gTriTableTex = 0;

void gPrepareMcConstantTex()
{
	if ( gEdgeTableTex && gTriTableTex ) return;

	// save current binding states for restore
	// this ensure not to damage the inner state maintained by H3D
	GLenum tartget = GL_TEXTURE_2D;
	GLint texture = 0;

	GLint old_active_tex = 0;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &old_active_tex);
	// use the last texutre slot
	glActiveTexture(GL_TEXTURE15);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);

    // if (0 == texture) {
    //  glGetIntegerv(GL_TEXTURE_BINDING_1D, &texture);
    //  if (texture) tartget = GL_TEXTURE_1D;
    // }

	if (0 == texture) {
		glGetIntegerv(GL_TEXTURE_BINDING_3D, &texture);
		if (texture) tartget = GL_TEXTURE_3D;
	}

	if (0 == texture) {
		glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &texture);
		if (texture) tartget = GL_TEXTURE_BINDING_CUBE_MAP;
	}

	// generate mc constant textures
	if (0 == gEdgeTableTex) {
		glGenTextures(1, &gEdgeTableTex);

		glBindTexture(GL_TEXTURE_2D, gEdgeTableTex);
		//Integer textures must use nearest filtering mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		//We create an integer texture with new GL_EXT_texture_integer formats
		glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 256, 1, 0, GL_ALPHA_INTEGER_EXT, GL_INT, &gEdgeTable);
	}

	if (0 == gTriTableTex) {
		glGenTextures(1, &gTriTableTex);

		glBindTexture(GL_TEXTURE_2D, gTriTableTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 16, 256, 0, GL_ALPHA_INTEGER_EXT, GL_INT, &gTriTable);
	}

	// restore old state
	glBindTexture(tartget, texture);
	glActiveTexture(old_active_tex);
}

uint32 gGenOrUploadScaled3DTexture(const uint32 tex3D = 0, const float *field = 0x0, const int w = 0, const int h = 0, const int d = 0)
{
	// save current binding states for restore
	// this ensure not to damage the inner state maintained by H3D
	GLenum tartget = GL_TEXTURE_3D;
	GLint texture = 0;

	GLint old_active_tex = 0;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &old_active_tex);
	// use the last texutre slot
	glActiveTexture(GL_TEXTURE15);
	glGetIntegerv(GL_TEXTURE_BINDING_3D, &texture);

    // if (0 == texture) {
    //  glGetIntegerv(GL_TEXTURE_BINDING_1D, &texture);
    //  if (texture) tartget = GL_TEXTURE_1D;
    // }

	if (0 == texture) {
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
		if (texture) tartget = GL_TEXTURE_2D;
	}

	if (0 == texture) {
		glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &texture);
		if (texture) tartget = GL_TEXTURE_BINDING_CUBE_MAP;
	}

	GLuint datafield_tex = 0;
	if (0 == tex3D) {
		glGenTextures(1, &datafield_tex);
		glBindTexture(GL_TEXTURE_3D, datafield_tex);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	} else {
		datafield_tex = tex3D;
		glBindTexture(GL_TEXTURE_3D, datafield_tex);
		glTexImage3D( GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, w, h, d, 0, GL_ALPHA, GL_FLOAT, field);
	}

	// restore old state
	glBindTexture(tartget, texture);
	glActiveTexture(old_active_tex);

	return datafield_tex;
}

const char *vsSphDebugView =
        "uniform mat4 viewProjMat;\n"
        "uniform mat4 worldMat;\n"
        "attribute vec3 vertPos;\n"
        "void main() {\n"
        "	gl_PointSize = 3.0;\n"
        "	gl_Position = viewProjMat * worldMat * vec4( vertPos, 1.0 );\n"
        "}";

const char *fsSphDebugView =
        "uniform vec4 color;\n"
        "void main() {\n"
        "	gl_FragColor = color;\n"
        "}\n";

uint32 SphNode::vlSph;
ShaderCombination SphNode::debugViewShader;

bool gInitGeoShader()
{
    if (gIsInitialised && gIsGeoShaderSupported) return true;
    else if (gIsInitialised && !gIsGeoShaderSupported) return false;

    gIsGeoShaderSupported = isExtensionSupported( "GL_EXT_geometry_shader4" )
                            && isExtensionSupported( "GL_EXT_texture_integer" )
                            && isExtensionSupported( "GL_ARB_texture_float" );
    if (gIsGeoShaderSupported) {
        gIsGeoShaderSupported = (glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC) platGetProcAddress( "glProgramParameteriEXT" )) != 0x0;
    }
    gIsInitialised = true;
    return gIsGeoShaderSupported;
}


///---------------SPH Static Data Resource---------------

SphStaticDataResource::SphStaticDataResource( const string &name, int flags )
    : Resource( RT_SphStaticData, name, flags )
    , par_num_(0)
    , sample_dist_(0.0f)
    , density_(0.0f)
    , isrigid_(false)
    , write_mapped_(false)
{
    initDefault();
}


SphStaticDataResource::~SphStaticDataResource()
{
    release();
}


void SphStaticDataResource::initDefault()
{
    par_num_ = 0;
    buf_ = 0x0;
    static const float max_float = std::numeric_limits<float>::max();
    static const float min_float = std::numeric_limits<float>::min();
    bbmin_.x = bbmin_.y = bbmin_.z = max_float;
    bbmax_.x = bbmax_.y = bbmax_.z = min_float;
}


void SphStaticDataResource::release()
{
    if (buf_)  {
        delete []buf_;
        buf_ = 0x0;
    }
}


bool SphStaticDataResource::raiseError( const string &msg, int line )
{
    // Reset
    release();
    initDefault();

    if( line < 0 )
        Modules::log().writeError( "SphStaticData resource '%s': %s", _name.c_str(), msg.c_str() );
    else
        Modules::log().writeError( "SphStaticData resource '%s' in line %i: %s", _name.c_str(), line, msg.c_str() );

    return false;
}

bool SphStaticDataResource::load( const char *data, int size )
{
    if( !Resource::load( data, size ) ) return false;
    return updateData(data, size);
}

bool SphStaticDataResource::updateData( const char *data, int size )
{
    uint32 nbparticles = reinterpret_cast<const uint32*>(data)[0] ;

    if(!nbparticles) return false;

    data += sizeof(uint32);

    isrigid_ = (sizeof(uint32) + 2*3*nbparticles*sizeof(float) + 2*sizeof(float)) <= size;

    if (nbparticles > par_num_) {
        if (buf_) delete []buf_;
        buf_ = new float[nbparticles*3*2];
    }

    par_num_ = nbparticles;
    uint32 pcount = sizeof(float)*par_num_*3;
    memcpy(buf_, data, pcount) ;

    // read the extra info
    if (pcount > 0 && pcount + sizeof(unsigned int) < size) {
        data += pcount;
        sample_dist_ = reinterpret_cast<const float*>(data)[0];
        data += sizeof(float);
        density_ = reinterpret_cast<const float*>(data)[0];
        data += sizeof(float);
    }

    if (isrigid_) {
        // copy normals
        memcpy(buf_ + 3*par_num_, data, pcount) ;
//        // update center of mass
//        float xacc = 0.f, yacc = 0.f, zacc = 0.f;
//        for (int i = 0; i < par_num_; ++ i) {
//            xacc += buf_[i*3+0];
//            yacc += buf_[i*3+1];
//            zacc += buf_[i*3+2];
//        }
//        cm_ = Vec3f(xacc/float(par_num_), yacc/float(par_num_), zacc/float(par_num_));
    }

    // update bounding box
    updateBoundingBox();

    //dirty_ = true;

    return true;
}

void SphStaticDataResource::updateBoundingBox()
{
    if (0 == par_num_) return ;
    Vec3f *p = reinterpret_cast<Vec3f*>(buf_);
    // reset bounding box
    static const float max_float = std::numeric_limits<float>::max();
    static const float min_float = std::numeric_limits<float>::min();
    bbmin_.x = bbmin_.y = bbmin_.z = max_float;
    bbmax_.x = bbmax_.y = bbmax_.z = min_float;
    for (uint32 i=0; i<par_num_; ++i) {

        // update bounding box
        bbmin_.x = bbmin_.x > p[i].x ? p[i].x : bbmin_.x;
        bbmin_.y = bbmin_.y > p[i].y ? p[i].y : bbmin_.y;
        bbmin_.z = bbmin_.z > p[i].z ? p[i].z : bbmin_.z;

        bbmax_.x = bbmax_.x < p[i].x ? p[i].x : bbmax_.x;
        bbmax_.y = bbmax_.y < p[i].y ? p[i].y : bbmax_.y;
        bbmax_.z = bbmax_.z < p[i].z ? p[i].z : bbmax_.z;
    }
}


int SphStaticDataResource::getElemCount( int elem )
{
    switch (elem) {
    case SphResParams::ParticleElem :
        return 1;
    }
    return Resource::getElemCount( elem );
}

int SphStaticDataResource::getElemParamI(int elem, int elemIdx, int param)
{

    if (elem == SphResParams::ParticleElem) {
        switch (param) {
        case SphResParams::ParticleNumI :
            return par_num_;
        case SphResParams::RigidI :
            return isrigid_ ? 1:0;
        }
    }
    return Resource::getElemParamI(elem, elemIdx, param);
}


float SphStaticDataResource::getElemParamF( int elem, int elemIdx, int param, int compIdx )
{

    if (elem == SphResParams::ParticleElem) {
        switch (param) {
        case SphResParams::DensityF:
            return density_;
        case SphResParams::SampleDistanceF :
            return sample_dist_;
        }
    }
    return Resource::getElemParamF( elem, elemIdx, param, compIdx );
}

void *SphStaticDataResource::mapStream(int elem, int elemIdx, int stream, bool read, bool write)
{
    if( read || write )
    {
        write_mapped_ = write;
        switch( elem )
        {
        case SphResParams::ParticleElem:
            return buf_;
        }
    }
    return Resource::mapStream(elem, elemIdx, stream, read, write);
}

void SphStaticDataResource::unmapStream()
{
    if (write_mapped_) {
        // update bounding box
        updateBoundingBox();
        //dirty_ = true;
		write_mapped_ = false;
    }
}

///---------------SPH Code Resource---------------
bool SphCodeResource::load(const char *data, int size)
{
    if ( CodeResource::load(data, size) ) {

        // notify the sph material resource to update injection
        for( uint32 i = 0; i < Modules::resMan().getResources().size(); ++i ) {
            Resource *res = Modules::resMan().getResources()[i];

            if( res != 0x0 && res->getType() == ResourceTypes::Material
                && (res->getFlags()&SphMaterialResouce::SPH_MTL_FLAG) ) {

                // check whether it depends this
                SphMaterialResouce *mtl = static_cast<SphMaterialResouce*>(res);
                if (mtl->hasDependency(this)) {
                    // update the injection
                    mtl->updateInjectedShader(this);
                }
            }
        }
        return true;
    }
    return false;
}

bool SphCodeResource::raiseError(const string &msg)
{
    // Reset
    release();
    initDefault();

    Modules::log().writeError( "SphCode resource '%s': %s", _name.c_str(), msg.c_str() );

    return false;
}

///---------------SPH Shader Resource---------------
bool SphShaderResource::load( const char *data, int size )
{
	if ( ShaderResource::load(data, size) ) {

		// notify the sph material resource to update injection
		for( uint32 i = 0; i < Modules::resMan().getResources().size(); ++i ) {
			Resource *res = Modules::resMan().getResources()[i];
			if( res != 0x0 && res->getType() == ResourceTypes::Material
				&& (res->getFlags()&SphMaterialResouce::SPH_MTL_FLAG) ) {
					// check whether it depends this
					SphMaterialResouce *mtl = static_cast<SphMaterialResouce*>(res);
					if (mtl->getShaderRes() == this) mtl->_injected =false;
			}
		}
		return true;
	}
	return false;
}

///---------------SPH Material---------------
SphMaterialResouce::SphMaterialResouce(const string &name, int flags)
    : MaterialResource(name, flags)
    , _cubeExtend(0)
{
    initDefault();
}

SphMaterialResouce::~SphMaterialResouce()
{
    release();
}

Resource *SphMaterialResouce::clone()
{
    SphMaterialResouce *res = new SphMaterialResouce( "", _flags );

    *res = *this;
    return res;
}

void SphMaterialResouce::initDefault()
{
    _mcEnabled = false;
    _injected = false;
    _cubeExtend = 32;
    _scaleFieldExtend[0] = 128;
    _scaleFieldExtend[1] = 128;
    _scaleFieldExtend[2] = 128;
    MaterialResource::initDefault();
}

void SphMaterialResouce::release()
{
    // release related shader objects
    InjectedMap::iterator itr = _injectedMap.begin();
    for ( ; itr!=_injectedMap.end(); ++itr ) {
        if ( 0 != itr->second.second ) {
            ShaderResource *shader_res = getShaderRes();
            ShaderContext *ctx =  shader_res->findContext(itr->first);
            ctx->compiled = false;
            shader_res->compileContexts();
            glDeleteShader(itr->second.second);
            itr->second.second = 0;
        }
    }
    _injectedMap.clear();
    MaterialResource::release();
}

bool SphMaterialResouce::load(const char *data, int size)
{
    if (MaterialResource::load(data, size)) {
        XMLDoc doc;
        doc.parseBuffer( data, size );
        if( doc.hasError() )
            return raiseError( "XML parsing error" );

        // get the ShaderFlag named _F16_GeometryShaderInjected
        XMLNode node1 = doc.getRootNode().getFirstChild( "ShaderFlag" );

        while (!node1.isEmpty()) {
            if (0 == _stricmp("_F16_McGeometryShaderInjected", node1.getAttribute("name", ""))) break;
            node1 = node1.getNextSibling( "ShaderFlag" );
        }

        // this mtl call for gemoetry shader inject
        if (!node1.isEmpty()) {
            XMLNode inject_node = node1.getFirstChild("GeometryShaderInject");
            while (!inject_node.isEmpty()) {
                string ctx_name = inject_node.getAttribute("contextName", "");
                if (!ctx_name.empty()) {
                    string gshader_src = inject_node.getAttribute("geomSrc", "");
                    if (gshader_src.empty()) return raiseError("geomSrc Must be specified!");
                    // insert into inject map
                    ResHandle res =  Modules::resMan().addResource( ResourceTypes::Code, gshader_src, 0, false );
                    CodeResource *codeRes = (CodeResource *)Modules::resMan().resolveResHandle( res );
                    _injectedMap.insert(make_pair<string, GeomShaderPair > (
                                            ctx_name,
                                            make_pair<PCodeResource, uint32> (codeRes, 0)
                                            )
                                        );
                }
                inject_node = inject_node.getNextSibling("GeometryShaderInject");
            }
            _cubeExtend = atoi(node1.getAttribute("cubeExtend", "32"));
            _scaleFieldExtend[0] = atoi(node1.getAttribute("scaleFieldExtendX", "128"));
            _scaleFieldExtend[1] = atoi(node1.getAttribute("scaleFieldExtendY", "128"));
            _scaleFieldExtend[2] = atoi(node1.getAttribute("scaleFieldExtendZ", "128"));
        }
        _mcEnabled = _injectedMap.size() > 0;
        return true;
    }
    return false;
}

bool SphMaterialResouce::isInjected() const
{
    return _injected;
}

void SphMaterialResouce::injectGeometryShader()
{
    if (_injected) return;
    InjectedMap::iterator itr = _injectedMap.begin();
    for ( ; itr!=_injectedMap.end(); ++itr ) {
        ShaderResource *shader_res = getShaderRes();
        ShaderContext *ctx = shader_res->findContext(itr->first);
        if ( ctx != 0x0 ) {
            // find shader combination
            ShaderCombination *shcom = shader_res->getCombination(*ctx, getCombinationMask());
            if ( shcom != 0x0 ) injectGeometryShader(ctx, shcom, itr);
        }
    }
    _injected = true;
}

ShaderResource *SphMaterialResouce::getShaderRes()
{
    int shader_res = getElemParamI(MaterialResData::MaterialElem, 0, MaterialResData::MatShaderI);
    if (0 == shader_res) return 0x0;
    return (ShaderResource *)Modules::resMan().resolveResHandle( shader_res );
}

bool SphMaterialResouce::updateInjectedShader(CodeResource *codeRes)
{
    InjectedMap::iterator itr = _injectedMap.begin();
    for ( ; itr!=_injectedMap.end(); ++itr ) {
        if ( codeRes == itr->second.first ) {
            ShaderResource *shader_res = getShaderRes();
            ShaderContext *ctx =  shader_res->findContext(itr->first);
            if ( ctx != 0x0 ) {
                // find shader combination
                ShaderCombination *shcom = shader_res->getCombination(*ctx, getCombinationMask());
                if ( shcom != 0x0 ) return injectGeometryShader(ctx, shcom, itr);
            }
            break;
        }
    }
    return false;
}

bool SphMaterialResouce::hasDependency(CodeResource *codeRes)
{
    InjectedMap::iterator itr = _injectedMap.begin();
    for ( ; itr!=_injectedMap.end(); ++itr ) {
        if ( itr->second.first->hasDependency(codeRes ) ) return true;
    }
    return false;
}

bool SphMaterialResouce::raiseError(const string &msg, int line)
{
    // Reset
    release();
    initDefault();

    if( line < 0 ) Modules::log().writeError( "SphMaterial resource '%s': %s", _name.c_str(), msg.c_str() );
    else Modules::log().writeError( "SphMaterial resource '%s' in line %i: %s", _name.c_str(), line, msg.c_str() );

    return false;
}

bool SphMaterialResouce::injectGeometryShader(ShaderContext *ctx, ShaderCombination *shcom, InjectedMap::iterator &itr)
{ 
    if ( 0 == shcom->shaderObj ) return false;
	Modules::log().writeDebugInfo("Start injecting GeometryShader...");
    // recompile the shader combination
    ctx->compiled = false;
	shcom->customSamplers.clear();
	shcom->customUniforms.clear();
    getShaderRes()->compileContexts();

    // save custom uniforms for this shader
    vector<pair<int, string> > old_state = gGetShaderCombCustomUniformNames(*shcom);

    GLuint shader = glCreateShader( GL_GEOMETRY_SHADER_EXT );
    if ( itr->second.second != 0 ) {
        // clear the old geometry shader
        glDeleteShader(itr->second.second);
        itr->second.second = shader;
    }

    // get geometry shader source and compile
    string finalcode = itr->second.first->assembleCode();
    const char *src_srt = finalcode.c_str();
    glShaderSource(shader, 1, &src_srt, 0x0);
	itr->second.second = shader;
    glCompileShader(shader);

    // check compile status
    string shader_log;
    int status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if( !status ) {
        // Get info
        int infologLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
        if( infologLength > 1 ) {
            int charsWritten = 0;
            char *infoLog = new char[infologLength];
            glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
            shader_log =  string("[Geometry Shader]\n") + infoLog;
            delete[] infoLog; infoLog = 0x0;
        }
    } else {
        // compiled successfully, relink the programm to inject the geometry shader
        uint32 gl_obj = gRDI->getShaderNativeHandler(shcom->shaderObj);
        glAttachShader(gl_obj, shader);
        //                GLint temp;
        //                glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
        //                std::cout<<"Max GS output vertices:"<<temp<<"\n";

        // 2. setup geometry shader
        // set POINTS primitives as INPUT
        glProgramParameteriEXT(gl_obj, GL_GEOMETRY_INPUT_TYPE_EXT , GL_POINTS );
        // set TRIANGLE STRIP as OUTPUT
        glProgramParameteriEXT(gl_obj, GL_GEOMETRY_OUTPUT_TYPE_EXT , GL_TRIANGLE_STRIP);
        // set maximum number of vertices to be generated by Geometry Shader to 16
        // 16 is the maximum number of vertices a marching cube configuration can own
        // this parameter is very important and have an important impact on Shader performances
        // its value must be chosen closer as possible to real maximum number of vertices
        glProgramParameteriEXT(gl_obj, GL_GEOMETRY_VERTICES_OUT_EXT, 16);

        // relink whole program object (Geometry+Vertex+Fragment)
        glLinkProgram(gl_obj);
        int infologLength = 0;
        glGetProgramiv(gl_obj, GL_INFO_LOG_LENGTH, &infologLength);
        if( infologLength > 1 ) {
            int charsWritten = 0;
            char *infoLog = new char[infologLength];
            glGetProgramInfoLog( gl_obj, infologLength, &charsWritten, infoLog );
            shader_log =  string("[Linking]\n") + infoLog;
            delete[] infoLog; infoLog = 0x0;
        }

        glGetProgramiv( gl_obj, GL_LINK_STATUS, &status );
        if( status ) {
            if( !gUpdateShaderComb(*shcom, old_state) ) status = 0;
        }
    }

	Modules::log().writeDebugInfo("GeometryShader injected...");

    if (!status) {
        Modules::log().writeError( "SphMaterial resource GeometryShader error: %s", shader_log.c_str());
        return false;
    }
    return true;
}

///---------------SPH Node--------------
SphNode::SphNode( const SphNodeTpl &sphTpl ) :
    SceneNode( sphTpl ),
    _materialRes( sphTpl.matRes ),
    _sphData(sphTpl.parRes),
    _parNum( 0 ),
    _vertexBuffer( 0 ),
	_mcBuffer( 0 ),
    _mcEnabled(false),
    _scaleTex3d(0),
    _cubeExtend(0),
    _cubeSetp(),
    _sf_gen_cb(0x0) {

    _scaleFieldExtend[0] = _scaleFieldExtend[1] = _scaleFieldExtend[2] = 0;

    if( sphTpl.parRes != 0x0 ) {
        _sphData = sphTpl.parRes;
        prepareForGeometry(true);
    } else {
        _localBBox.min = Vec3f( 0, 0, 0 );
        _localBBox.max = Vec3f( 1, 1, 1 );
        _renderable = false;
    }

}


SphNode::~SphNode()
{
    gRDI->destroyBuffer( _vertexBuffer );
    if (_scaleTex3d) glDeleteTextures(1, &_scaleTex3d);
}


SceneNodeTpl *SphNode::parsingFunc( map< string, string > &attribs )
{
    map< string, string >::iterator itr;
    SphNodeTpl *sphTpl = new SphNodeTpl( "", 0x0, 0x0);

    itr = attribs.find( "volumeData" );
    if( itr != attribs.end() ) {
        uint32 res = Modules::resMan().addResource( RT_SphStaticData, itr->second, 0, false );
        if( res != 0 )
            sphTpl->parRes = (SphStaticDataResource*)Modules::resMan().resolveResHandle( res );
    }
    itr = attribs.find( "material" );
    if( itr != attribs.end() ) {
        uint32 res = Modules::resMan().addResource( ResourceTypes::Material, itr->second, SphMaterialResouce::SPH_MTL_FLAG, false );
        if( res != 0 )
            sphTpl->matRes = (MaterialResource *)Modules::resMan().resolveResHandle( res );
    }

    return sphTpl;
}


SceneNode *SphNode::factoryFunc( const SceneNodeTpl &nodeTpl )
{
    if( nodeTpl.type != SNT_SphNode ) return 0x0;

    return new SphNode( *(SphNodeTpl *)&nodeTpl );
}


void SphNode::renderFunc(uint32 firstItem, uint32 lastItem, const string &shaderContext, const string &theClass,
                         bool debugView, const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order,
                         int occSet)
{
    CameraNode *curCam = Modules::renderer().getCurCamera();
    if( curCam == 0x0 ) return;

    const RenderQueue &renderQueue = Modules::sceneMan().getRenderQueue();

    // Loop through sph queue
    // Loop through terrain queue
    for( uint32 i = firstItem; i <= lastItem; ++i ) {
        SphNode *sph = (SphNode *)renderQueue[i].node;

        bool use_gemoshader = false;
        SphMaterialResouce *mtl = 0x0;
        if ( sph->_materialRes->getFlags() & SphMaterialResouce::SPH_MTL_FLAG ) {
            if ( gIsGeoShaderSupported ) {
                mtl = static_cast<SphMaterialResouce*>(sph->_materialRes.getPtr());
                SphMaterialResouce::InjectedMap::iterator itr = mtl->_injectedMap.find(shaderContext);
                use_gemoshader = mtl->isMcEnabled() && itr!=mtl->_injectedMap.end();
                if (use_gemoshader && !mtl->isInjected()) {
                    // inject geometry shader
                    mtl->injectGeometryShader();
                }
            } else {
                // geometry shader is not supported, use debug shader instead
                debugView = true;
            }
        }

        int tex_slots[3] = {-1, -1, -1};
        if( !debugView ) {
            if( !sph->_materialRes->isOfClass( theClass ) ) continue;
            if( !Modules::renderer().setMaterial( sph->_materialRes, shaderContext ) ) continue;

            if (use_gemoshader) {
                // get free texture unit of current shader
                ShaderCombination *curShader = Modules::renderer().getCurShader();
                vector< int > &samplers =  curShader->customSamplers;
                vector< int >::const_iterator itr = samplers.begin();
                uint32 globj = gRDI->getShaderNativeHandler(curShader->shaderObj);

                bool free_list[16] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
                for (; itr != samplers.end(); itr++) {
                    if (*itr < 0) continue;
                    int tex_unit = 0;
                    glGetUniformiv(globj, *itr, &tex_unit);
                    free_list[tex_unit] = false;
                }

                int count = 0;
                for ( int i=0; i<sizeof(free_list)/sizeof(bool) && count < 3; ++i ) {
                    if (free_list[i]) tex_slots[count++] = i;
                }

                if ( count < 3 ) {
                    use_gemoshader = false;
                    debugView = true;
                    Modules::log().writeError("No free texture unit for marching cube!");
                }
            }
        }

        if (debugView) {
            Modules::renderer().setShaderComb( &debugViewShader );
            Modules::renderer().commitGeneralUniforms();
            int loc = gRDI->getShaderConstLoc( debugViewShader.shaderObj, "color" );
            float color[4] = { 0.5f, 0.75f, 1, 1 };
            gRDI->setShaderConst( loc, CONST_FLOAT4, color );
        }

        // Set uniforms
        ShaderCombination *curShader = Modules::renderer().getCurShader();
        if( curShader->uni_worldMat >= 0 ) {
            gRDI->setShaderConst( curShader->uni_worldMat, CONST_FLOAT44, &sph->_absTrans.x[0] );
        }
        if( curShader->uni_worldNormalMat >= 0 ) {
            Matrix4f normalMat4 = sph->_absTrans.inverted().transposed();
            float normalMat[9] = { normalMat4.x[0], normalMat4.x[1], normalMat4.x[2],
                                   normalMat4.x[4], normalMat4.x[5], normalMat4.x[6],
                                   normalMat4.x[8], normalMat4.x[9], normalMat4.x[10] };
            gRDI->setShaderConst( curShader->uni_worldNormalMat, CONST_FLOAT33, normalMat );
        }
        if( curShader->uni_nodeId >= 0 ) {
            float id = (float)sph->getHandle();
            gRDI->setShaderConst( curShader->uni_nodeId, CONST_FLOAT, &id );
        }
        int uni_pointsCount = gRDI->getShaderConstLoc( curShader->shaderObj, "pointsCount" );
        if( uni_pointsCount >= 0 )  gRDI->setShaderConst( uni_pointsCount, CONST_FLOAT, &sph->_parNum );

        if (use_gemoshader) {
            // assign marching cube texture
            gPrepareMcConstantTex();
            // edgeTeble
            glActiveTexture(GL_TEXTURE0 + tex_slots[1]);
            glBindTexture(GL_TEXTURE_2D, gEdgeTableTex);
            int uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "edgeTableTex" );
            if (uniform_loc > 0) gRDI->setShaderSampler(uniform_loc, tex_slots[1]);
            // triTeble
            glActiveTexture(GL_TEXTURE0 + tex_slots[2]);
            glBindTexture(GL_TEXTURE_2D, gTriTableTex);
            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "triTableTex" );
            if (uniform_loc > 0) gRDI->setShaderSampler(uniform_loc, tex_slots[2]);
            // data field
            glActiveTexture(GL_TEXTURE0 + tex_slots[0]);
            glBindTexture(GL_TEXTURE_3D, sph->_scaleTex3d);
            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "dataFieldTex" );
            if (uniform_loc > 0) gRDI->setShaderSampler(uniform_loc, tex_slots[0]);

            // step in data 3D texture for gradient computation (lighting)
            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "dataStep" );
            if (uniform_loc > 0) {
                glUniform3f(uniform_loc,
                            1.0f/float(sph->_scaleFieldExtend[0]),
                            1.0f/float(sph->_scaleFieldExtend[1]),
                            1.0f/float(sph->_scaleFieldExtend[2]));
            }

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[0]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, .0f, .0f, .0f);

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[1]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, sph->_cubeSetp.x, .0f, .0f);

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[2]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, sph->_cubeSetp.x, sph->_cubeSetp.y, .0f);

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[3]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, .0f, sph->_cubeSetp.y, .0f);

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[4]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, .0f, .0f, sph->_cubeSetp.z);

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[5]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, sph->_cubeSetp.x, .0f, sph->_cubeSetp.z);

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[6]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, sph->_cubeSetp.x, sph->_cubeSetp.y, sph->_cubeSetp.z);

            uniform_loc = gRDI->getShaderConstLoc( curShader->shaderObj, "vertDecals[7]" );
            if (uniform_loc > 0) glUniform3f(uniform_loc, .0f, sph->_cubeSetp.y, sph->_cubeSetp.z);
        }

		// get geometry ready
		sph->prepareForGeometry(false);

        // GL hack
		if (use_gemoshader) {
			uint32 points_num =  sph->_cubeExtend * sph->_cubeExtend * sph->_cubeExtend;

			gRDI->setVertexBuffer( 0, sph->_mcBuffer, 0, 12 );
			//gRDI->setVertexBuffer( 0, sph->_vertexBuffer, 0, 12 );
			gRDI->setVertexLayout( vlSph );
			//glPolygonMode(GL_FRONT_AND_BACK , GL_LINE );
			gRDI->draw(PRIM_PTLIST, 0, points_num);
			//glPolygonMode(GL_FRONT_AND_BACK , GL_FILL );
		} else {
			uint32 points_num = sph->_parNum;
			gRDI->setVertexBuffer( 0, sph->_vertexBuffer, 0, 12 );
			gRDI->setVertexLayout( vlSph );
#if !defined(PLATFORM_ES2)
                        glEnable(GL_POINT_SPRITE);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
			gRDI->draw(PRIM_PTLIST, 0, points_num);
#if !defined(PLATFORM_ES2)
                        glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
                        glDisable(GL_POINT_SPRITE);
#endif
		}

        Modules::stats().incStat( EngineStats::BatchCount, 1.0f );
        Modules::stats().incStat( EngineStats::TriCount, sph->_parNum);

        gRDI->setVertexLayout( 0 );
    }
}


bool SphNode::canAttach( SceneNode &parent )
{
    return true;
}


int SphNode::getParamI( int param )
{
    switch( param ) {
    case SphNodeParams::MatResI:
        return _materialRes != 0x0 ? _materialRes->getHandle() : 0;
    case SphNodeParams::VolumeResI:
        return _sphData != 0x0 ? _sphData->getHandle() : 0;
    }

    return SceneNode::getParamI( param );
}


void SphNode::setParamI( int param, int value )
{
    Resource *res;
    switch( param ) {
    case SphNodeParams::VolumeResI:
        res = Modules::resMan().resolveResHandle( value );
        if( res != 0x0 && res->getType() == RT_SphStaticData ){
            SphStaticDataResource *parRes = static_cast<SphStaticDataResource*>(res);
            if (parRes != 0x0) {
                _sphData = parRes;
                // force to update the data
                _scaleFieldExtend[0] = 0;
                prepareForGeometry (true);
                return;
            }
        }
        Modules::setError( "Invalid volume data in h3dSetNodeParamI for H3DEXTsph::VolumeResI" );
        return;
    case SphNodeParams::MatResI:
        res = Modules::resMan().resolveResHandle( value );
        if( res != 0x0 && res->getType() == ResourceTypes::Material ) {
            _materialRes = (MaterialResource *)res;
            // adjust SPH node behavior accroding to materail type
            prepareForGeometry (false);

        } else Modules::setError( "Invalid handle in h3dSetNodeParamI for H3DEXTsph::MatResI" );
        return;
    }

    SceneNode::setParamI( param, value );
}


float SphNode::getParamF( int param, int compIdx )
{
    switch( param ) {
    //            case SphNodeParams::MeshQualityF:
    //                return 1.0f / _lodThreshold;
    //            case SphNodeParams::SkirtHeightF:
    //                return _skirtHeight;
    }

    return SceneNode::getParamF( param, compIdx );
}


void SphNode::setParamF( int param, int compIdx, float value )
{
    switch( param ) {
    //            case SphNodeParams::MeshQualityF:
    //                _lodThreshold = 1.0f / value;
    //                return;
    //            case SphNodeParams::SkirtHeightF:
    //                _skirtHeight = value;
    //                return;
    }

    SceneNode::setParamF( param, compIdx, value );
}


bool SphNode::checkIntersection( const Vec3f &rayOrig, const Vec3f &rayDir, Vec3f &intsPos ) const
{
    if( !rayAABBIntersection( rayOrig, rayDir, _bBox.min, _bBox.max ) ) return false;

    // Transform ray to local space
    Matrix4f m = _absTrans.inverted();
    Vec3f orig = m * rayOrig;
    Vec3f dir = m * (rayOrig + rayDir) - orig;

    return true;
}

void SphNode::updateSphData()
{
	if (_sphData != 0x0 /*&& _sphData->dirty_*/) {
		// force to update the data
		_scaleFieldExtend[0] = 0;
		prepareForGeometry(true);
	}
}

ResHandle SphNode::createGeometryResource( const string &name, float lodThreshold )
{
    if( name == "" ) return 0;

    //        Resource *resObj = Modules::resMan().findResource( ResourceTypes::Geometry, name );
    //        if (resObj != 0x0)
    //        {
    //            Modules::log().writeDebugInfo( "Resource name specified in h3dextCreatesphGeoRes already exists" );
    //            return 0;
    //        }
    //
    ////        uint32 blockCount = calculateGeometryBlockCount( lodThreshold, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f );
    //        // Calculate number of vertices
    //        const uint32 streamSize = blockCount * getVertexCount();
    //        // Calculate size of elements in stream
    //        const uint32 streamElementSize = sizeof( float ) * 3;
    //
    //        const uint32 indexCount = (blockCount * (_blockSize + 1) * (_blockSize + 1) * 6);
    //
    //        // Horde Flag (4) + Version (sizeof(uint32)) + Joint Count (sizeof(uint32)) + number of streams (sizeof(uint32)) +
    //        // + streamsize (sizeof(uint32)) + stream id (sizeof(uint32)) + stream element size (sizeof(uint32))
    //        // + stream (streamsize * stream element size) +  index count (sizeof(uint32)) +
    //        // + indices (sizeof(uint32) * index count) + number of morph targets (sizeof(uint32))
    //        const uint32 size = 4 + sizeof( uint32 ) * 8 + streamSize * streamElementSize + indexCount * sizeof( uint32 );
    //
    //        // Create geometry data block
    //        char *data = new char[size];
    //
    //        char *pData = data;
    //        // Write Horde flag
    //        pData[0] = 'H'; pData[1] = '3'; pData[2] = 'D'; pData[3] = 'G'; pData += 4;
    //        // Set version to 5
    //        *( (uint32 *)pData ) = 5; pData += sizeof( uint32 );
    //        // Set joint count (zero for sphs)
    //        *( (uint32 *)pData ) = 0; pData += sizeof( uint32 );
    //        // Set number of streams
    //        *( (uint32 *)pData ) = 1; pData += sizeof( uint32 );
    //        // Write size of each stream
    //        *( (uint32 *)pData ) = streamSize; pData += sizeof( uint32 );
    //
    //        // Beginning of stream data
    //        // Stream ID
    //        *( (uint32 *)pData ) = 0; pData += sizeof( uint32 );
    //        // set stream element size
    //        *( (uint32 *)pData ) = streamElementSize; pData += sizeof( uint32 );
    //
    //        uint32 index = 0;
    //        float *vertexData = (float *)pData;
    //        //const unsigned int* const refIndexData = (unsigned int*)(pData + streamSize * streamElementSize + sizeof( uint32 ));
    //        uint32 *indexData = (uint32 *)(pData + streamSize * streamElementSize + sizeof( uint32 ));
    //
    //        createGeometryVertices( lodThreshold, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, vertexData, indexData, index );
    //
    //        // Skip vertex data
    //        pData += streamSize * streamElementSize;
    //
    //        // Set number of indices
    //        *( (uint32 *) pData ) = indexCount; pData += sizeof( uint32 );
    //
    //        // Skip index data
    //        pData += indexCount * sizeof( uint32 );
    //
    //        // Set number of morph targets
    //        *( (uint32 *) pData ) = 0;	pData += sizeof( uint32 );
    //
    //        ResHandle res = Modules::resMan().addResource( ResourceTypes::Geometry, name, 0, true );
    //        resObj = Modules::resMan().resolveResHandle( res );
    //        if ( resObj ) resObj->load( data, size );
    //        delete[] data;
    //
    return 0;
}

void SphNode::setScaleFiledGenerateCallBack(ScaleFieldGenerateCallBack callback)
{
    _sf_gen_cb = callback;
}

void SphNode::onPostUpdate()
{
    _bBox = _localBBox;
    _bBox.transform( _absTrans );
}

void SphNode::prepareForGeometry(bool update_vertex_buffer)
{
    _localBBox.min = _sphData->bbmin_;
    _localBBox.max = _sphData->bbmax_;
    _renderable = true;

	if (update_vertex_buffer) {
		recreateVertexBuffer(*_sphData) ;
	}

    // adjust SPH node behavior according to material type
    if ( _materialRes->getFlags() & SphMaterialResouce::SPH_MTL_FLAG ) {
        _mcEnabled = static_cast<SphMaterialResouce*>(_materialRes.getPtr())->isMcEnabled();
        if (_mcEnabled) {
            prepareForMarchingCube();
        }
    }
}

void SphNode::prepareForMarchingCube()
{
    SphMaterialResouce *mtl = static_cast<SphMaterialResouce*>(_materialRes.getPtr());

    if ( _scaleFieldExtend[0] != mtl->_scaleFieldExtend[0]
         || _scaleFieldExtend[1] != mtl->_scaleFieldExtend[1]
         || _scaleFieldExtend[2] != mtl->_scaleFieldExtend[2] ) {

        _scaleFieldExtend[0] = mtl->_scaleFieldExtend[0];
        _scaleFieldExtend[1] = mtl->_scaleFieldExtend[1];
        _scaleFieldExtend[2] = mtl->_scaleFieldExtend[2];

        // build scale field
        float *scalefiled = new float[_scaleFieldExtend[0]*_scaleFieldExtend[1]*_scaleFieldExtend[2]];

        if (_sf_gen_cb) {
            // use user defined callback to generate scale field
            _sf_gen_cb(_sphData->buf_, _parNum,
				//_sphData->bbmin_.x, _sphData->bbmin_.y, _sphData->bbmin_.z,
				//_sphData->bbmax_.x, _sphData->bbmax_.y, _sphData->bbmax_.z,
				0.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 1.0f,
                       scalefiled,
                       _scaleFieldExtend[0], _scaleFieldExtend[1], _scaleFieldExtend[2]
                       );

        } else {
            // use default one
            gDefScaleFiledGenerator(_sphData->buf_, _parNum,
                                    //_sphData->bbmin_.x, _sphData->bbmin_.y, _sphData->bbmin_.z,
                                    //_sphData->bbmax_.x, _sphData->bbmax_.y, _sphData->bbmax_.z,
									0.0f, 0.0f, 0.0f,
									1.0f, 1.0f, 1.0f,
                                    scalefiled,
                                    _scaleFieldExtend[0], _scaleFieldExtend[1], _scaleFieldExtend[2]
                                    );
        }

        // upload to tex3D
        if ( 0 == _scaleTex3d ) _scaleTex3d = gGenOrUploadScaled3DTexture();
        gGenOrUploadScaled3DTexture(_scaleTex3d, scalefiled, _scaleFieldExtend[0], _scaleFieldExtend[1], _scaleFieldExtend[2]);
        delete []scalefiled;
    }

    if ( _cubeExtend != mtl->_cubeExtend) {
        // build cube grid
        _cubeExtend = mtl->_cubeExtend;
        Vec3f cube_size( _cubeExtend, _cubeExtend, _cubeExtend);
       //_cubeSetp = (_sphData->bbmax_ - _sphData->bbmin_) + Vec3f(0.001f, 0.001f, 0.001f) ;
       //_cubeSetp.x /= cube_size.x;
       //_cubeSetp.y /= cube_size.y;
       //_cubeSetp.z /= cube_size.z;
        _cubeSetp.x = 2.0 / cube_size.x;
        _cubeSetp.y = 2.0 / cube_size.y;
        _cubeSetp.z = 2.0/ cube_size.z;

        float *grid_data=new float[_cubeExtend*_cubeExtend*_cubeExtend*3];
        //        int ii=0;
        //        for(float k=_sphData->bbmin_.z; k<_sphData->bbmax_.z; k+=_cubeSetp.z)
        //            for(float j=_sphData->bbmin_.y; j<_sphData->bbmax_.y; j+=_cubeSetp.y)
        //                for(float i=_sphData->bbmin_.x; i<_sphData->bbmax_.x; i+=_cubeSetp.x){
        //                    grid_data[ii]= i;
        //                    grid_data[ii+1]= j;
        //                    grid_data[ii+2]= k;
        //                    ii+=3;
        //                }

        //Swizzled Walk
         int n=0; int pos[3] = {0, 0, 0}; int cube_size_i[3] = {_cubeExtend, _cubeExtend, _cubeExtend};
         gSwizzledWalk(n, grid_data, pos, cube_size_i, cube_size);

        // recreate the vertex buffer
        gRDI->destroyBuffer( _mcBuffer );
        _mcBuffer = gRDI->createVertexBuffer(_cubeExtend*_cubeExtend*_cubeExtend*3*sizeof(float), grid_data);
        delete []grid_data;
    }
}

void SphNode::recreateVertexBuffer(SphStaticDataResource &sphdata)
{
    gRDI->destroyBuffer( _vertexBuffer );

    if (sphdata.par_num_ > 0 && sphdata.buf_ != 0x0) {
        _parNum = sphdata.par_num_;
        _vertexBuffer = gRDI->createVertexBuffer( _parNum * sizeof( float ) * 3, sphdata.buf_);
        //sphdata.dirty_ = false;
    } else {
        // Init default data
        _parNum = 32;
        float *dummy = new float[_parNum*3];
        memset( dummy, 0, _parNum*3*sizeof(float) );
        _vertexBuffer = gRDI->createVertexBuffer( _parNum * sizeof( float ) * 3, dummy);
        delete [] dummy;
    }
    _sphData = &sphdata;

}


}  // namespace




