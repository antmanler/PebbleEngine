
#include <Engine/EngineModules.h>
#include "egModule.h"

namespace Pebble {

class egModulesPrivate : public IEngineModules
{
    friend class EngineModules;
public:
    egModulesPrivate() {}
    virtual void EGCALL Destroy() {::operator delete(this) ;}
    virtual IEngineConfig* EGCALL engineConfig() { return EngineModules::engineConfig(); }
    virtual IEngineLogger* EGCALL engineLogger() { return EngineModules::engineLogger(); }
    virtual IWorld* EGCALL world() { return EngineModules::world(); }

    virtual bool EGCALL addRenderer( IRenderer* renderer )
    {
        return EngineModules::controller()->addRenderer(renderer);
    }

    virtual void EGCALL removeRenderer( IRenderer* renderer )
    {
        EngineModules::controller()->removeRenderer(renderer);
    }

    virtual Signal* EGCALL sigReset() {return EngineModules::controller()->sigReset();}
    virtual Signal* EGCALL sigRender() {return EngineModules::controller()->sigRender();}
    virtual Signal* EGCALL sigUpdate() {return EngineModules::controller()->sigUpdate();}
    virtual Signal* EGCALL sigStartNewScene() {return EngineModules::controller()->sigStartNewScene();}
    virtual SceneLoadedSignal* EGCALL sigSceneLoaded() {return EngineModules::controller()->sigSceneLoaded();}
    virtual ParseSysArgvSignal* EGCALL sigParseSysArgv() {return EngineModules::controller()->sigParseSysArgv();}
    virtual ResizeSignal* EGCALL sigResize() {return EngineModules::controller()->sigResize();}

//    virtual TestSignal0* EGCALL sigTest0() {return EngineModules::controller()->sigTest0();}
//    virtual TestSignal1* EGCALL sigTest1() {return EngineModules::controller()->sigTest1();}
//    virtual TestSignal2* EGCALL sigTest2() {return EngineModules::controller()->sigTest2();}

private :
    ~egModulesPrivate() {}
    // disable copy constructor
    egModulesPrivate(const EngineModules&){}
};

EngineConfig* EngineModules::config_ = 0x0;
EngineLog* EngineModules::logger_ = 0x0;
World* EngineModules::world_ = 0x0;
Controller* EngineModules::controller_ = 0x0;
IEngineModules* EngineModules::interface_inst_ = 0x0;

EngineConfig* EngineModules::engineConfig()
{
    if (0x0 == config_) config_ = new EngineConfig();
    return config_;
}

EngineLog* EngineModules::engineLogger()
{
    if(0x0 == logger_) logger_ = new EngineLog();
    return logger_;
}

Controller* EngineModules::controller()
{
    if(0x0 == controller_) controller_ = new Controller();
    return controller_;
}

World* EngineModules::world()
{
    if(0x0 == world_) world_ = new World();
    return world_;
}

IEngineModules* EngineModules::module()
{
    if(0x0 == interface_inst_) interface_inst_ = new egModulesPrivate();
    return interface_inst_;
}

void EngineModules::release()
{
    delete world_; world_ = 0x0;
    delete controller_; controller_ = 0x0;
    interface_inst_->Destroy(); interface_inst_ = 0x0;
    //delete config_; config_ = 0x0;
    //delete logger_; logger_ = 0x0;
}

}
