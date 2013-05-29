
#include <set>
#include <utility>
#include <algorithm>
#include <iterator> 
#include <Engine/utXML.h>
#include <Engine/rapidxml_print.h>
#include <Horde3D/Horde3D.h>
#include <Horde3D/Horde3DUtils.h>
#include "Camera.h"
#include "Object3D.h"
#include "SceneGraph.h"

namespace
{
using namespace std;
using namespace Pebble;

struct RenderPartCompare {
    bool operator() (const Camera* lhs,  const Camera* rhs) {
        return lhs->depth() < rhs->depth() ;
    }
};
}

namespace Pebble
{

using namespace std;

class H3DRendererPrivate : public DefaultComponent<IRenderer>, public has_slots<>
{
    typedef std::set<Camera*, RenderPartCompare> CameraSet ;
    
public:
    H3DRendererPrivate(IEngineModules *eg_modules) 
    : eg_modules_(eg_modules)
    , w_(320)
    , h_(240)
    , update_(false)
    {}

    ~H3DRendererPrivate()
    {
        eg_modules_->engineLogger()->writeDebugInfo("Releasing Horde3D");
        //h3dClear();
        h3dRelease();
		eg_modules_->engineLogger()->writeDebugInfo("Horde3D released");
    }

    // implementations
    EG_IMPLEMENTS
    virtual const char* EGCALL typeName() const {return "H3DRenderer";}

    virtual void EGCALL start()
    {
        // init horde3d
        eg_modules_->engineLogger()->writeDebugInfo("Renderer start: init Horde3D");
        enable(h3dInit());
        eg_modules_->world()->sigEntityAdded()->connect(make_slot(this, &H3DRendererPrivate::newEntity));
        eg_modules_->world()->sigEntityRemoved()->connect(make_slot(this, &H3DRendererPrivate::entityRemoved));
    }

    virtual void EGCALL update()
    {
        vector<IEntity*>::iterator itr;
        
        if (entities_ready2check_.size() > 0) {
            itr = entities_ready2check_.begin();
            for (; itr != entities_ready2check_.end(); ++itr) {
                if ( (*itr)->getComponent(COM_TYPE_NAME(Camera)) ) {
                    entities_has_cam_.push_back(*itr);
                    update_ = true;
                }
            }
            entities_ready2check_.clear();
        }
        
        if (update_) {
            // update for cameras
            cameras_.clear();
            itr = entities_has_cam_.begin();
            for (; itr != entities_has_cam_.end(); ++itr) {
                // add active cameras
                if(static_cast<Object3D*>((*itr)->getComponent(COM_TYPE_NAME(Object3D)))->active()) {
                    cameras_.insert(static_cast<Camera*>((*itr)->getComponent(COM_TYPE_NAME(Camera)))) ;
                }
            }
            update_ = false;
        }
        // update for cameras
//        cameras_.clear();
//        IWorld *world = eg_modules_->world();
//        world->findEntityHasComponentOf(COM_TYPE_NAME(Camera));
//        uint32 cnt = world->getFindCount();
//        // save the find results locally  because h3dGetNodeFindResult() is not safe
//        IEntity** find_ret = static_cast<IEntity**>(alloca(cnt * sizeof(IEntity*)));
//        for (int i = 0; i < cnt; ++i) find_ret[i] = world->getFindeResult(i);
//        //eg_modules_->engineLogger()->writeDebugInfo("Find %d camera(s)", cnt);
//
//        for(int i=0; i<cnt; ++i) {
//            // add active cameras
//            if(static_cast<Object3D*>(find_ret[i]->getComponent(COM_TYPE_NAME(Object3D)))->active()) {
//                cameras_.insert(static_cast<Camera*>(find_ret[i]->getComponent(COM_TYPE_NAME(Camera)))) ;
//            }
//        }
    }

    virtual void EGCALL render()
    {
        CameraSet::iterator itr = cameras_.begin();
        // perform render for each camera
        for(; itr!=cameras_.end(); ++itr) {
            H3DNode cam = (*itr)->hordeId();
            if ( 0 == cam ) continue;
            (*itr)->preRender();
            h3dRender(cam);
            (*itr)->postRender();
//            h3dutDumpMessages();
        }
        // Finish rendering of frame
        h3dFinalizeFrame();
        // Remove all overlays
        h3dClearOverlays();
    }

    void resize(uint32 w, uint32 h)
    {
        CameraSet::iterator itr = cameras_.begin();
        // perform render for each camera
        for(; itr!=cameras_.end(); ++itr) {
            H3DNode cam = (*itr)->hordeId();
            if ( 0 == cam ) continue;
        }
        w_ = w; h_ = h;
        eg_modules_->engineLogger()->writeDebugInfo("Resize to %dx%d", w, h);
    }

    virtual uint32 EGCALL priority() const {return 0;}
    
    void newEntity(IEntity *ent)
    {
        entities_ready2check_.push_back(ent);
    }
    
    void entityRemoved(IEntity *ent)
    {
        vector<IEntity*>::iterator itr = find(entities_has_cam_.begin(), entities_has_cam_.end(), ent);
        if (itr != entities_has_cam_.end()) {
            entities_has_cam_.erase(itr);
            update_ = true;
            return;
        }
        itr = find(entities_ready2check_.begin(), entities_ready2check_.end(), ent);
        if (itr != entities_ready2check_.end()) {
            entities_ready2check_.erase(itr);
            update_ = true;
        }

    }

private:
    IEngineModules *eg_modules_;
    CameraSet cameras_;
    uint32 w_, h_;
    // cache for entites
    vector<IEntity*> entities_has_cam_;
    vector<IEntity*> entities_ready2check_;
    bool update_;
};

SceneGraph::SceneGraph(IPluginManager *manager)
    : DefaultComponent<IComponent>()
    , sg_src_("")
    , eg_modules_(manager->getModules())
    , manager_(manager)
    , h3drenderer_(0x0)
{
    //    eg_modules_->sigTest0()->connect( sigslot::make_slot(this, &SceneGraph::sigTest0 ));
    //    eg_modules_->sigTest1()->connect( sigslot::make_slot(this, &SceneGraph::sigTest1 ));
    //    eg_modules_->sigTest2()->connect( sigslot::make_slot(this, &SceneGraph::sigTest2 ));
}

SceneGraph::SceneGraph(const SceneGraph& ref)
{
    *this = ref;
}

SceneGraph::~SceneGraph()
{
    //Destroy();
}

void EGCALL SceneGraph::Destroy()
{
    if (h3drenderer_) {
        eg_modules_->removeRenderer(h3drenderer_);
        delete h3drenderer_;
        h3drenderer_ = 0x0;
    }
}

void EGCALL SceneGraph::start()
{
    if ( sg_src_.empty() ) return;

    if ( !h3drenderer_->isEnabled() ) {
        eg_modules_->engineLogger()->writeError("Horde3d init failed, make sure you have a valid OpenGL context");
        return;
    }

    // add the scene graph to H3DRoot
    H3DRes scn_res = h3dAddResource(H3DResTypes::SceneGraph, sg_src_.c_str(), 0);
    // load resources
    const char* res_path = eg_modules_->engineConfig()->getConfig("ResourceRepositry")->value();
    eg_modules_->engineLogger()->writeDebugInfo("Load resource from %s", res_path);
    if (h3dutLoadResourcesFromDisk(res_path)) {
        H3DNode scnid = h3dAddNodes(H3DRootNode, scn_res);
        addH3DNode(scnid);
        h3dutDumpMessages();
        eg_modules_->engineLogger()->writeDebugInfo("Add node (%d) to H3DRoot", scnid);
    } else {
        eg_modules_->engineLogger()->writeWarning("Load resource failed");
    }
}

void EGCALL SceneGraph::update()
{
    // TODO:
}

void SceneGraph::genEntities()
{
    if ( !uinit_h3dnodes_.empty() ) {
        while( !uinit_h3dnodes_.empty() ) {
            int nodes = h3dFindNodes( uinit_h3dnodes_.top(), "", H3DNodeTypes::Undefined );
            eg_modules_->engineLogger()->writeDebugInfo("Find nodes %d", nodes);

            // save the find results locally  because h3dGetNodeFindResult() is not safe
            int *find_ret = static_cast<int *>(alloca(nodes * sizeof(int)));
            for (int i = 0; i < nodes; ++i) find_ret[i] = h3dGetNodeFindResult(i);

            for (int i = 0; i < nodes; ++i) {
                H3DNode node = find_ret[i];
                // create a entity
                string name = h3dGetNodeParamStr(node, H3DNodeParams::NameStr);
                if ( name.empty() ) name = "Entity";
                IEntity *ent = eg_modules_->world()->createEntity(name.c_str());
                // setup the entity
                if ( ent ) {
                    Object3D *obj = static_cast<Object3D*>(manager_->createComponent(COM_TYPE_NAME(Object3D)));
                    if (obj) {
                        obj->setHordeId(node);
                        if (!ent->addComponent(obj)) obj->Destroy();
                    }
                    // add extra component
                    const char* buf = h3dGetNodeParamStr(node, H3DNodeParams::AttachmentStr);
                    if ( strlen(buf) > 0 ) setup_attachment(ent, buf, strlen(buf));
                } else {
                    eg_modules_->engineLogger()->writeWarning("Cannot make an entity for h3d node %d", node);
                }
            }
            uinit_h3dnodes_.pop();
        }
//        h3dutDumpMessages();
    }
}

void EGCALL SceneGraph::loadFromXml(const char* contentbuf, uint32 size)
{
    XMLDoc doc;
    doc.parseBuffer( contentbuf, size );
    if( doc.hasError() ) {
        eg_modules_->engineLogger()->writeError("SceneGraph::loadFromXml(): scene parsing failed, %d@%s", __LINE__, __FILE__);
        return;
    }

    XMLNode rootNode = doc.getRootNode();
    if ( !rootNode.isEmpty() && (strcmp(rootNode.getName(), "SceneGraph") == 0) ) {
        sg_src_ = rootNode.getAttribute("src", "");
        if ( sg_src_.empty() ) {
            eg_modules_->engineLogger()->writeError("SceneGraph::loadFromXml(): "
                                                    "it's not a valid scene graph path, %d@%s", __LINE__, __FILE__);
            return;
        }

        // create renderer and init Horde3D
        if ( 0x0 == h3drenderer_ ) {
            h3drenderer_ = new H3DRendererPrivate(eg_modules_);
            if( !eg_modules_->addRenderer(h3drenderer_) ) {
                eg_modules_->engineLogger()->writeWarning("Cannot create the Horde3D renderer");
                delete h3drenderer_;
                h3drenderer_ = 0x0;
            } else {
                eg_modules_->engineLogger()->writeDebugInfo("Add Horde3D renderer");
            }
        }

        // configure the horde engine
        XMLNode pathes = rootNode.getFirstChild("EnginePath");
        if (!pathes.isEmpty()) {
            h3dutSetResourcePath(H3DResTypes::SceneGraph, pathes.getAttribute("scenegraphpath", ""));
            h3dutSetResourcePath(H3DResTypes::Geometry, pathes.getAttribute("geometrypath", ""));
            h3dutSetResourcePath(H3DResTypes::Animation, pathes.getAttribute("animationpath", ""));
            h3dutSetResourcePath(H3DResTypes::Shader, pathes.getAttribute("shaderpath", ""));
            h3dutSetResourcePath(H3DResTypes::Code, pathes.getAttribute("shaderpath", ""));
            h3dutSetResourcePath(H3DResTypes::Texture, pathes.getAttribute("texturepath", ""));
            h3dutSetResourcePath(H3DResTypes::Material, pathes.getAttribute("materialpath", ""));
            h3dutSetResourcePath(H3DResTypes::ParticleEffect, pathes.getAttribute("effectspath", ""));
            h3dutSetResourcePath(H3DResTypes::Pipeline, pathes.getAttribute("pipelinepath", ""));
        }

        XMLNode settings = rootNode.getFirstChild("EngineConfig");
        if (!settings.isEmpty()) {
            eg_modules_->engineLogger()->writeDebugInfo("SceneGraph::loadFromXml(): loading horde engine options");
            h3dSetOption(H3DOptions::FastAnimation,
                         (_stricmp(settings.getAttribute("fastAnimation", "true"), "TRUE") == 0 ||
                          _stricmp(settings.getAttribute("fastAnimation", "1"), "1") == 0) ? 1.0f : 0.0f);
            h3dSetOption(H3DOptions::LoadTextures,
                         (_stricmp(settings.getAttribute("loadTextures", "true"), "TRUE") == 0 ||
                          _stricmp(settings.getAttribute("loadTextures", "1"), "1") == 0) ? 1.0f : 0.0f);
            h3dSetOption(H3DOptions::TexCompression,
                         (_stricmp(settings.getAttribute("texCompression", "true"), "TRUE") == 0 ||
                          _stricmp(settings.getAttribute("texCompression", "1"), "1") == 0) ? 1.0f : 0.0f);
            h3dSetOption(H3DOptions::SRGBLinearization,
                         (_stricmp(settings.getAttribute("sRGBLinearization", "true"), "TRUE") == 0 ||
                          _stricmp(settings.getAttribute("sRGBLinearization", "1"), "1") == 0) ? 1.0f : 0.0f);
            h3dSetOption(H3DOptions::MaxAnisotropy, atof(settings.getAttribute("maxAnisotropy", "4")));
            h3dSetOption(H3DOptions::ShadowMapSize, atoi(settings.getAttribute("shadowMapSize", "1024")));
            h3dSetOption(H3DOptions::MaxNumMessages, atoi(settings.getAttribute("maxNumMessages", "1024")));
            h3dSetOption(H3DOptions::SampleCount, min(atoi(settings.getAttribute("sampleCount", "0")), 4));
        }  else {
            eg_modules_->engineLogger()->writeDebugInfo("SceneGraph::loadFromXml(): set to default horde engine options");
            h3dSetOption(H3DOptions::LoadTextures, 1);
            h3dSetOption(H3DOptions::TexCompression, 1);
            h3dSetOption(H3DOptions::FastAnimation, 1);
            h3dSetOption(H3DOptions::TexCompression, 1);
            h3dSetOption(H3DOptions::SRGBLinearization, 0);
            h3dSetOption(H3DOptions::MaxAnisotropy, 4);
            h3dSetOption(H3DOptions::ShadowMapSize, 1024);
            h3dSetOption(H3DOptions::MaxNumMessages, 1024);
            h3dSetOption(H3DOptions::SampleCount, 0);
        }

    } else {
        eg_modules_->engineLogger()->writeError("SceneGraph::loadFromXml(): "
                                                "it's not a valid scene graph data, %d@%s", __LINE__, __FILE__);
    }
}

void SceneGraph::addH3DNode(int node)
{
    uinit_h3dnodes_.push(node);
}

void SceneGraph::setup_attachment(IEntity *ent, const char *buf, uint32 size)
{
    XMLDoc doc;
    doc.parseBuffer(buf, size);
    if( doc.hasError() ) {
        eg_modules_->engineLogger()->writeError("SceneGraph::setup_attachment(): scene parsing failed, %d@'%s...'", __LINE__, buf);
        return;
    }
    XMLNode xmlNode = doc.getRootNode() ;
    if( !xmlNode.isEmpty() && strcmp(xmlNode.getName(), "Attachment") == 0 ) {

        const char* type = xmlNode.getAttribute("type", "");
        if ( strcmp(type, "Engine") == 0 ) {
            XMLNode node = xmlNode.getFirstChild();
            while( !node.isEmpty() ) {
                IComponent *com = manager_->createComponent(node.getName());
                // add to entity
                if (com && ent->addComponent(com)) {
                    string com_xml = "";
                    rapidxml::print( back_inserter( com_xml ), *node.getRapidXMLNode(), 0 );
                    // set serialized data
                    char *txt = new char[com_xml.length()];
                    memcpy(txt, com_xml.c_str(), com_xml.length());
                    com->loadFromXml(txt, com_xml.length());
                    delete []txt;
                } else {
                    eg_modules_->engineLogger()->writeWarning("SceneGraph::setup_attachment():"
                                                              " can not handle the node of '%s'", node.getName());
                    if(com) com->Destroy();
                }
                node = node.getNextSibling();
            }
        }
    }
}

}
