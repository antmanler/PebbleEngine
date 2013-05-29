
#pragma once

#include <Engine/EngineSDK.h>
#include <Engine/YapsHost.h>

namespace Pebble {

class PluginManager : public Yaps::Manager<IPluginManager>
{
public:
    typedef Yaps::Host<IComponent> ComponetManager;

public :
    PluginManager();
    virtual ~PluginManager();

    virtual void EGCALL registerComponent(IComponentManager *com_man);
    virtual void EGCALL removeComponent(IComponentManager *com_man);
    virtual IComponent* EGCALL createComponent(const char* type_name);
    virtual IEngineModules* EGCALL getModules();

    void update();
    const char* componentNames() const ;
private:
    ComponetManager* com_man_;
};

}


