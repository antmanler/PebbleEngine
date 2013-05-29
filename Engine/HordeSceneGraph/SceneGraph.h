
#pragma once

#include <string>
#include <stack>
#include <Engine/EngineSDK.h>

#define ScnGraph "SceneGraph"

namespace Pebble
{
class H3DRendererPrivate;
class SceneGraph : public DefaultComponent< IComponent >//, public sigslot::has_slots<>
{
    friend class H3DRendererPrivate;
public:
    SceneGraph(IPluginManager* manager);
    SceneGraph(const SceneGraph& scn);
    virtual ~SceneGraph();

    // implementations
    virtual void EGCALL Destroy() ;
    virtual const char* EGCALL typeName() const {return COM_TYPE_NAME(SceneGraph);}
    virtual void EGCALL start();
    virtual void EGCALL update();
    /** \breif load scene graph resource from a xml string
      * format : <SceneGraph src='name or path of the scene' />
      */
    virtual void EGCALL loadFromXml(const char* contentbuf, uint32 size);

    void genEntities();
    void addH3DNode(int node);

//    void sigTest0() {printf("sigTest0()\n");}
//    void sigTest1(const char* str) {printf("sigTest1(\"%s\")\n", str);}
//    void sigTest2(uint32 w, uint32 h) { printf("sigTest2(%d, %d)\n", w, h);}

private:
    void setup_attachment(IEntity* ent, const char* buf, uint32 size);

private:
    IEngineModules *eg_modules_;
    std::string sg_src_;
    std::stack<int> uinit_h3dnodes_;
    IPluginManager* manager_;
    H3DRendererPrivate* h3drenderer_;
};
}
