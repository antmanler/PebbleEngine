
#pragma once


#include <set>
#include <Engine/EngineSDK.h>
#include "egEntity.h"
#include "egComSys.h"

namespace Pebble
{
class IComponent;
struct RenderPartCompare ;

class Controller
{
    friend class EngineModules;
    typedef std::set<IRenderer*, RenderPartCompare> RendererSet ;
public:
    Controller() ;
    bool loadPlugin(const char* filename);

    bool changeScene(const char* buf, uint32 size);
    void update();
    IComponent* createComponent(const char* com_type, const IEntity *owner);

    void parseArgv(int argc, char** argv);
    void render();
    void resize(uint32 w, uint32 h);
    bool addRenderer( IRenderer* renderer );
    void removeRenderer( IRenderer* renderer );

    PluginManager *pluginMan() {return manager_;}

    IEngineModules::Signal* sigReset() {return &sig_reset_;}
    IEngineModules::Signal* sigRender() {return &sig_render_;}
    IEngineModules::Signal* sigUpdate() {return &sig_update_;}
    IEngineModules::Signal* sigStartNewScene(){return &sig_start_new_scn_;}
    IEngineModules::SceneLoadedSignal* sigSceneLoaded() {return &sig_load_scene_;}
    IEngineModules::ParseSysArgvSignal* sigParseSysArgv() {return &sig_parse_argv_;}
    IEngineModules::ResizeSignal* sigResize() {return &sig_resize_;}

//    IEngineModules::TestSignal0* EGCALL sigTest0() {return &sig_t0_;}
//    IEngineModules::TestSignal1* EGCALL sigTest1() {return &sig_t1_;}
//    IEngineModules::TestSignal2* EGCALL sigTest2() {return &sig_t2_;}

private:
    Controller(const Controller&) {}
    ~Controller() ;

private:
//    IEngineModules::Signal sig_reset_;
//    IEngineModules::Signal sig_render_;
//    IEngineModules::Signal sig_update_;
//    IEngineModules::LoadSceneSignal sig_load_scene_;
//    IEngineModules::ParseSysArgvSignal sig_parse_argv_;
//    IEngineModules::ResizeSignal sig_resize_;
    signal0<> sig_reset_;
    signal0<> sig_render_;
    signal0<> sig_update_;
    signal0<> sig_start_new_scn_;
    signal2<const char*, uint32>  sig_load_scene_;
    signal2<int, char** > sig_parse_argv_;
    signal2<uint32, uint32> sig_resize_;
//    sigslot::signal0<> sig_t0_;
//    sigslot::signal1<const char*> sig_t1_;
//    sigslot::signal2<uint32, uint32> sig_t2_;


    RendererSet *renderer_set_ ;
    PluginManager *manager_;
};

struct RenderPartCompare {
    bool operator() (const IRenderer* lhs,  const IRenderer* rhs) {
        return lhs->priority() > rhs->priority() ;
    }
};

}
