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

#include "egPrerequisites.h"
#include "utMath.h"
#include "egMaterial.h"
#include "egShader.h"
#include "egTexture.h"
#include "egScene.h"


namespace Horde3DSph {

using namespace Horde3D;


const int SNT_SphNode = 300;
const int RT_SphStaticData = 100;

extern const char *vsSphDebugView;
extern const char *fsSphDebugView;
extern uint32 gEdgeTableTex;
extern uint32 gTriTableTex;

bool gInitGeoShader();

struct SphNodeParams
{
    enum List
    {
        VolumeResI = 20000,
        MatResI
    };
};

struct SphResParams
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

// Forward declaration
class SphNode;

/**
* \class Sph Particles resource
*/
class SphStaticDataResource : public Resource
{
public:
    static Resource *factoryFunc( const std::string &name, int flags )
    { return new SphStaticDataResource( name, flags ); }

    SphStaticDataResource( const std::string &name, int flags );
    ~SphStaticDataResource();

    void initDefault();
    void release();
    bool load( const char *data, int size );
    bool updateData( const char *data, int size );

    int getElemCount( int elem );
    int getElemParamI(int elem, int elemIdx, int param);
    float getElemParamF( int elem, int elemIdx, int param, int compIdx );

    void *mapStream( int elem, int elemIdx, int stream, bool read, bool write );
    void unmapStream();

    void updateBoundingBox();

private:
    bool raiseError( const std::string &msg, int line = -1 );

private:
    // bounding box
    Vec3f bbmin_, bbmax_;
    // center of mass
    Vec3f cm_;
    uint32 par_num_;
    float sample_dist_;
    float density_;
    float *buf_;
    bool isrigid_;
    bool write_mapped_;

    friend class SphNode;

};

typedef SmartResPtr< SphStaticDataResource > PSphStaticDataResource;

/**
* \class Sph code resource
*/
class SphCodeResource : public CodeResource
{
public:
    static Resource *factoryFunc( const std::string &name, int flags )
    { return new SphCodeResource( name, flags ); }

    SphCodeResource( const std::string &name, int flags )
        :CodeResource(name, flags) {}
    ~SphCodeResource() {}

    bool load( const char *data, int size );

private:
    bool raiseError( const std::string &msg );
};

/**
* \class Sph code resource
*/
class SphShaderResource : public ShaderResource
{
public:
	static Resource *factoryFunc( const std::string &name, int flags )
	{ return new SphShaderResource( name, flags ); }

	SphShaderResource( const std::string &name, int flags )
		:ShaderResource(name, flags) {}
	~SphShaderResource() {}

	bool load( const char *data, int size );
};


/**
* \class Sph material resource
*/
class SphMaterialResouce : public MaterialResource
{
public:
    enum {SPH_MTL_FLAG = 1 << 16};
    typedef std::pair<PCodeResource, uint32> GeomShaderPair;
    typedef std::map<std::string, GeomShaderPair > InjectedMap;

    static Resource *factoryFunc( const std::string &name, int flags )
    {
        if (flags & SPH_MTL_FLAG) return new SphMaterialResouce( name, flags );
        return MaterialResource::factoryFunc(name, flags);
    }

    SphMaterialResouce( const std::string &name, int flags );
    ~SphMaterialResouce();
    Resource *clone();

    void initDefault();
    void release();
    bool load( const char *data, int size );
    bool isInjected() const;
    bool isMcEnabled() const {return _mcEnabled;}
    /**
      * \brief Inject geometry shader according to user specified context and shader source
      * This method should be invoked when all the shader resources have been loaded, and
      * should be called once.
      */
    void injectGeometryShader();
    ShaderResource* getShaderRes() ;
    bool updateInjectedShader(CodeResource *codeRes) ;
    bool hasDependency( CodeResource *codeRes );

private:
    bool raiseError( const std::string &msg, int line = -1 );
    bool injectGeometryShader(ShaderContext *ctx, ShaderCombination *shcom, InjectedMap::iterator &itr);

private:
    bool          _mcEnabled;
    uint32        _cubeExtend;
    uint32        _scaleFieldExtend[3];
    bool          _injected;
    InjectedMap   _injectedMap;

    friend class SphNode;
	friend class SphShaderResource;
};


struct SphNodeTpl : public SceneNodeTpl
{
    PSphStaticDataResource   parRes;
    PMaterialResource        matRes;

    SphNodeTpl( const std::string &name, SphStaticDataResource *parRes, MaterialResource *matRes) :
        SceneNodeTpl( SNT_SphNode, name ), parRes( parRes ), matRes( matRes ){}
};

/**
* \class Sph node
*/
class SphNode : public SceneNode
{
public:
    typedef void ScaleFieldGenerateCallBack(const float* points, const int points_num,
                                            const float xmin, const float ymin, const float zmin,
                                            const float xmax, const float ymax, const float zmax,
                                            float* saclefiled, const int width, const int height, const int depth);
    ~SphNode();

    static SceneNodeTpl *parsingFunc( std::map< std::string, std::string > &attribs );
    static SceneNode *factoryFunc( const SceneNodeTpl &nodeTpl );
    static void renderFunc(uint32 firstItem, uint32 lastItem, const std::string &shaderContext, const std::string &theClass,
                           bool debugView, const Frustum *frust1, const Frustum *frust2, RenderingOrder::List order,
                           int occSet );

    bool canAttach( SceneNode &parent );
    int getParamI( int param );
    void setParamI( int param, int value );
    float getParamF( int param, int compIdx );
    void setParamF( int param, int compIdx, float value );

    bool checkIntersection( const Vec3f &rayOrig, const Vec3f &rayDir, Vec3f &intsPos ) const;

	void updateSphData();
    ResHandle createGeometryResource( const std::string &name, float lodThreshold );
    void setScaleFiledGenerateCallBack(ScaleFieldGenerateCallBack callback);

public:
    static uint32 vlSph;
    static ShaderCombination debugViewShader;

protected:
    SphNode( const SphNodeTpl &terrainTpl );

    void onPostUpdate();

    void prepareForGeometry(bool update_vertex_buffer);
    void prepareForMarchingCube();
    void recreateVertexBuffer(SphStaticDataResource &sphdata);

protected:
    PMaterialResource           _materialRes;
    PSphStaticDataResource      _sphData;
    uint32                      _parNum;
    uint32                      _vertexBuffer;
	uint32                      _mcBuffer;
    BoundingBox                 _localBBox;
    bool                        _mcEnabled;
    uint32                      _scaleTex3d;
    uint32                      _cubeExtend;
    Vec3f                       _cubeSetp;
    uint32                      _scaleFieldExtend[3];
    ScaleFieldGenerateCallBack* _sf_gen_cb;
};

}  // namespace
