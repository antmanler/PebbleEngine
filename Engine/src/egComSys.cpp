
#include <algorithm>
#include <vector>
#include <sstream>
#include "egComSys.h"
#include "egModule.h"

namespace {

using namespace Pebble;
struct ComponentsUpdator {
    void operator () (PluginManager::ComponetManager::FactoryType *factory) {
        IComponentManager *cm = static_cast<IComponentManager *>(factory->ptr());
        cm->update();
    }
};

char gComNames[1024] = "\0";

}

namespace Pebble {
using namespace  std;

PluginManager::PluginManager() :
    Yaps::Manager<IPluginManager>()
{
    com_man_ = new ComponetManager(COMPONET_HOST, SDK_VERSION);
    AddHost(com_man_);
}

PluginManager::~PluginManager()
{
    //if ( com_man_ )
        //delete com_man_;
    EngineModules::engineLogger()->writeDebugInfo("Releasing PluginManager");
    //Destory();
}

void EGCALL PluginManager::registerComponent(IComponentManager *comMan)
{
    com_man_->AddFactory(comMan, SDK_VERSION);
    EngineModules::engineLogger()->writeDebugInfo("ComSys:Add a new component factory: %s(v%d)", comMan->GetFactoryName(), SDK_VERSION);
}

void PluginManager::removeComponent(IComponentManager *com_man)
{
    if (com_man == static_cast<IComponentManager *>(com_man_->RemoveFactory(com_man)) ) {
		EngineModules::engineLogger()->writeDebugInfo("ComSys: component factory %s has been removed", com_man->GetFactoryName());
        com_man->Destroy();
    }
}

IComponent* EGCALL PluginManager::createComponent(const char* type_name)
{
    ComponetManager::FactoryType *factory = com_man_->GetFactory(type_name);
    if ( factory ) {
        IComponent * com = factory->Create();       
        return com;
    } else {
        EngineModules::engineLogger()->writeWarning("ComSys:Can not find a factory for %s", type_name);
    }
    return NULL;
}

IEngineModules* EGCALL PluginManager::getModules()
{
    return EngineModules::module();
}

void PluginManager::update()
{
    vector<ComponetManager::FactoryType* > mans ;
    mans.reserve(com_man_->GetFactoryCount());
    com_man_->GetAllFactories(mans);
    // update each component manager
    for_each(mans.begin(), mans.end(), ComponentsUpdator());
}

const char* PluginManager::componentNames() const
{
    memset(gComNames, 0, sizeof(gComNames));

    vector<ComponetManager::FactoryType* > mans ;
    mans.reserve(com_man_->GetFactoryCount());
    com_man_->GetAllFactories(mans);
    // get all names
    vector<ComponetManager::FactoryType* >::iterator itr = mans.begin();
    std::stringstream in;
    for(; itr != mans.end(); ++itr) {
        in << (*itr)->GetFactoryName() << ";";
    }
    sprintf(gComNames, "%s", in.str().substr(0, 1023).c_str());
    return gComNames;
}

}
