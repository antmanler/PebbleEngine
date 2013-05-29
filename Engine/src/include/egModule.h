#pragma once

#include "egCommon.h"
#include "egWorld.h"
#include "egController.h"

namespace Pebble
{

class IEngineModules;

class EngineModules
{
public:
    static EngineConfig* engineConfig();
    static EngineLog* engineLogger();
    static World* world();
    static Controller* controller();
    static IEngineModules* module();

    static void  release();

private:
    static EngineConfig* config_;
    static EngineLog* logger_;
    static World* world_;
    static Controller* controller_;
    static IEngineModules *interface_inst_;
};

}
