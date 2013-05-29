
#include <fstream>
#include "utPlatform.h"
#include "egModules.h"
#include "egCom.h"
#include "egRenderer.h"
#include "sph_extension.h"
#include "egSph.h"

#include "utDebug.h"


// Internal extension interface
namespace Horde3DSph {

using namespace Horde3D;


const char *ExtSph::getName()
{
    return "Sph";
}

bool ExtSph::init()
{
    Modules::sceneMan().registerNodeType( SNT_SphNode, "Sph",
                                          SphNode::parsingFunc, SphNode::factoryFunc );
    Modules::renderer().registerRenderFunc( SNT_SphNode, SphNode::renderFunc );

    Modules::resMan().registerResType( RT_SphStaticData, "SphStaticData", 0x0, 0x0,
                                       SphStaticDataResource::factoryFunc );

    // override the Material Resource handler
    Modules::resMan().registerResType(ResourceTypes::Material, "Material", 0x0, 0x0,
                                      SphMaterialResouce::factoryFunc);
    // override the Code Resource handler
    Modules::resMan().registerResType( ResourceTypes::Code, "Code", 0x0, 0x0,
                                       SphCodeResource::factoryFunc );
    // override the Shader Resource handler
    Modules::resMan().registerResType( ResourceTypes::Shader, "Shader", 0x0, 0x0,
                                       SphShaderResource::factoryFunc );

    VertexLayoutAttrib attribs[1] = {
        {"vertPos", 0, 3, 0},
    };
    SphNode::vlSph = gRDI->registerVertexLayout( 1, attribs );
    // Upload default shader used for debug view
    Modules::renderer().createShaderComb(vsSphDebugView, fsSphDebugView, SphNode::debugViewShader);

    gInitGeoShader();

    return true;
}

void ExtSph::release()
{
    Modules::renderer().releaseShaderComb( SphNode::debugViewShader );
    glDeleteTextures(1, &gEdgeTableTex);
    glDeleteTextures(1, &gTriTableTex);
    gEdgeTableTex = 0;
    gTriTableTex = 0;
}

}  // namespace


// Public C API
namespace Horde3DSph {

inline const std::string safeStr( const char *str )
{
    if( str != 0x0 ) return str;
    else return "";
}

DLLEXP ResHandle h3dextCreate3DTexture( const char *name, int width, int height, int depth, int fmt, int flags )
{

    if (depth <= 1) {
        Modules::log().writeDebugInfo( "Failed to add resource in h3dextCreate3DTexture; the detph must greater than 1.", 0 );
        return 0;
    }

    flags &= ~ResourceFlags::TexCubemap;
    flags &= ~ResourceFlags::TexSRGB;
    flags |= ResourceFlags::NoTexCompression;
    flags |= ResourceFlags::NoTexMipmaps;

    TextureResource *texRes = new TextureResource( safeStr(name), (uint32)width,
                                                   (uint32)height, (uint32)depth, (TextureFormats::List)fmt, flags );

    ResHandle res = Modules::resMan().addNonExistingResource( *texRes, true );
    if( res == 0 ) {
        Modules::log().writeDebugInfo( "Failed to add resource in h3dCreateTexture; maybe the name is already in use?", res );
        delete texRes;
    }

    return res;
}

DLLEXP NodeHandle h3dextAddSphNode( NodeHandle parent, const char *name, ResHandle volDataRes, ResHandle materialRes )
{
    SceneNode *parentNode = Modules::sceneMan().resolveNodeHandle( parent );
    if( parentNode == 0x0 ) return 0;

    Resource *volRes= Modules::resMan().resolveResHandle( volDataRes );
    if( volRes == 0x0 || volRes->getType() != RT_SphStaticData ) return 0;

    Resource *matRes =  Modules::resMan().resolveResHandle( materialRes );
    if( matRes == 0x0 || matRes->getType() != ResourceTypes::Material ) return 0;

    Modules::log().writeInfo( "Adding Sph node '%s'", safeStr( name ).c_str() );

    SphNodeTpl tpl( safeStr( name ), (SphStaticDataResource *)volRes, (MaterialResource *)matRes);
    SceneNode *sn = Modules::sceneMan().findType( SNT_SphNode )->factoryFunc( tpl );
    return Modules::sceneMan().addNode( sn, *parentNode );
}

DLLEXP ResHandle h3dextCreateSphMaterialResource(const char *name, int flags)
{
    flags |= SphMaterialResouce::SPH_MTL_FLAG;
    return Modules::resMan().addResource(ResourceTypes::Material, name, flags, true);
}

DLLEXP bool h3dextSetSphScaleFieldGenerateCallback(NodeHandle sphnode, SphNode::ScaleFieldGenerateCallBack* callback)
{
    SceneNode *sn = Modules::sceneMan().resolveNodeHandle( sphnode );
    APIFUNC_VALIDATE_NODE( sn, "h3dGetNodeParent", false);
    if ( sn->getType() != SNT_SphNode ) {
        Modules::log().writeDebugInfo( "It's not a type of SphNode.", 0 );
        return false;
    }

    static_cast<SphNode*>(sn)->setScaleFiledGenerateCallBack(callback);
    return true;
}

DLLEXP bool h3dextUpdateSphData(ResHandle res, const char *data, const int size)
{
    Resource *resObj = Modules::resMan().resolveResHandle( res );
    APIFUNC_VALIDATE_RES( resObj, "h3dextUpdateSphData", false );
    if (resObj->getType() != RT_SphStaticData) {
        Modules::log().writeDebugInfo( "It's not a type of SphStaticDataResource.", 0 );
        return false;
    }

    bool ret =  static_cast<SphStaticDataResource*>(resObj)->updateData(data, size) ;
    if (!ret)
        Modules::log().writeDebugInfo( "Failed to update sph resource in h3dextUpdateSphData.", 0 );
    return ret;
}


DLLEXP bool h3dextUpdateSphDataFromFile(ResHandle sphres, const char *filename)
{
    using namespace std ;
    ifstream input(filename, ios::in|ios::binary);
    if(input == NULL) return false;
    size_t length = 0;
    input.seekg (0, ios::end);
    length = input.tellg();
    input.seekg (0, ios::beg);

    char *buf = new char[length];

    bool ret = (input.read(buf, length).rdstate() & ifstream::failbit) == 0 ;
    input.close() ;
    ret = ret && h3dextUpdateSphData(sphres, buf, length) ;
    delete []buf;
    return ret;
}

DLLEXP void h3dextUpdateSphNode(NodeHandle sphnode)
{
    SceneNode *sn = Modules::sceneMan().resolveNodeHandle( sphnode );
    APIFUNC_VALIDATE_NODE( sn, "h3dGetNodeParent", APIFUNC_RET_VOID );
    if ( sn->getType() != SNT_SphNode ) {
        Modules::log().writeDebugInfo( "It's not a type of SphNode.", 0 );
        return;
    }

    static_cast<SphNode*>(sn)->updateSphData();

}

}  // namespace
