
#pragma once

#include "Platform.h"

namespace Pebble
{
class IEngineLogger;
class IEngineConfig;
class IWorld;
class IRenderer;

class IEngineModules
{
public:
//    typedef Signal0<> Signal;
//    typedef Signal2<int, char** > ParseSysArgvSignal;
//    typedef Signal2<const char*, uint32> LoadSceneSignal;
//    typedef Signal2<uint32, uint32> ResizeSignal;
    typedef ISignal0 Signal;
    typedef ISignal2<int, char** > ParseSysArgvSignal;
    typedef ISignal2<const char*, uint32> SceneLoadedSignal;
    typedef ISignal2<uint32, uint32> ResizeSignal;
//    typedef sigslot::ISignal0 TestSignal0;
//    typedef sigslot::ISignal1<const char *> TestSignal1;
//    typedef sigslot::ISignal2<uint32, uint32> TestSignal2;

public:
    virtual void EGCALL Destroy() = 0;
    virtual IEngineConfig* EGCALL engineConfig() = 0;
    virtual IEngineLogger* EGCALL engineLogger() = 0;
    virtual IWorld* EGCALL world() = 0;
    virtual bool EGCALL addRenderer( IRenderer* renderer ) = 0;
    virtual void EGCALL removeRenderer( IRenderer* renderer ) = 0;

    // access to signals
    virtual Signal* EGCALL sigReset() = 0;
    virtual Signal* EGCALL sigRender() = 0;
    virtual Signal* EGCALL sigUpdate() = 0;
    virtual Signal* EGCALL sigStartNewScene() = 0;
    virtual SceneLoadedSignal* EGCALL sigSceneLoaded() = 0;
    virtual ParseSysArgvSignal* EGCALL sigParseSysArgv() = 0;
    virtual ResizeSignal* EGCALL sigResize() = 0;
//    virtual TestSignal0* EGCALL sigTest0() = 0;
//    virtual TestSignal1* EGCALL sigTest1() = 0;
//    virtual TestSignal2* EGCALL sigTest2() = 0;
};

}
