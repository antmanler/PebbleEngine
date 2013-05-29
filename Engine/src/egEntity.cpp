
#include <algorithm>
#include <Engine/EngineSDK.h>
#include "egModule.h"
#include "egEntity.h"


namespace {
using namespace Pebble;

struct ComponentOfType {
    std::string name;
    ComponentOfType(const char *type_name) : name(type_name) {}

    bool operator () (const IComponent* com) {
        return strcmp(com->typeName(), name.c_str()) == 0;
    }
};

struct ComponentUpdate {
    void operator () (IComponent* com) {
        if (com->isEnabled())
            com->update();
    }
};

struct ComponetInitializer {
    void operator () (IComponent* com) {
        com->start();
    }
};

}

namespace Pebble {

using namespace std;

Entity::Entity(uint32 id)
    :id_(id)
{
}

Entity::~Entity()
{
    EngineModules::engineLogger()->writeDebugInfo("%s release components", name());
    vector< IComponent* >::iterator itr = uninit_coms_.begin();
    for(; itr!=uninit_coms_.end(); ++itr)
        (*itr)->Destroy();
    uninit_coms_.clear();
    itr = components_.begin();
    for(; itr!=components_.end(); ++itr)
        (*itr)->Destroy();
    components_.clear();
    com_name_id_map_.clear();
}

void EGCALL Entity::update()
{
    if ( !uninit_coms_.empty() ) initComponents();
    sig_update_.Emit();
    for_each(components_.begin(), components_.end(), ComponentUpdate());
}

IComponent* EGCALL Entity::getComponent(const char* type_name)
{
//    vector< IComponent* >::iterator itr = find_if(components_.begin(), components_.end(), ComponentOfType(type_name));
//    if (itr != components_.end()) return *itr;
//    itr = find_if(uninit_coms_.begin(), uninit_coms_.end(), ComponentOfType(type_name));
//    if (itr != uninit_coms_.end()) return *itr;
    ComponetNameIDMap::iterator itr = com_name_id_map_.find(std::string(type_name));
    if ( itr != com_name_id_map_.end() ) {
        if ( itr->second >= 0 ) return components_[itr->second];
        else return uninit_coms_[-(itr->second+1)];
    }
    
    return NULL;
}

bool EGCALL Entity::addComponent(IComponent* com)
{
    if ( NULL == getComponent(com->typeName()) ) { 
        uninit_coms_.push_back(com);
        // instert to name_id map <0 in uninit_coms_
        com_name_id_map_.insert(std::make_pair(std::string(com->typeName()), -(uninit_coms_.size())));
        
        com->setOwner(this);
        // explicity invoke the init methode for some contructing stuff
        com->init();
        EngineModules::engineLogger()->writeDebugInfo("Add a componet of %s, and initialized by invoking init()", com->typeName());
        return true;
    }
    return false;
}

IComponent* EGCALL Entity::getComponent(const uint32 idx)
{
    if (idx<components_.size())
        return components_.at(idx);
    return NULL;
}

bool EGCALL Entity::removeComponent(IComponent *com, bool release)
{
    ComponetNameIDMap::iterator map_itr = com_name_id_map_.find(std::string(com->typeName()));
    com_name_id_map_.erase(map_itr);
    
    vector< IComponent* >::iterator itr = find_if(components_.begin(), components_.end(), ComponentOfType(com->typeName()));
    if ( itr != components_.end() ) {
        if (release) (*itr)->Destroy();
        components_.erase(itr);
        return true;
    }
    itr = find_if(uninit_coms_.begin(), uninit_coms_.end(), ComponentOfType(com->typeName()));
    if ( itr != uninit_coms_.end() ) {
        if (release) (*itr)->Destroy();
        uninit_coms_.erase(itr);
        return true;
    }
    return false;
}

void EGCALL Entity::sendMessage(const char* evt_name, const char* param)
{
    vector< IComponent* >::iterator itr = components_.begin();
    for ( ; itr != components_.end(); ++itr ) {
        IComponent *com = *itr;
        if ( !(com->processMessage(evt_name, param)) )
            // receive the block signal, and do not need pass down the event
            break;
    }
}

void Entity::initComponents()
{
    while (!uninit_coms_.empty()) {
        IComponent *com = uninit_coms_.back(); uninit_coms_.pop_back();
        components_.push_back(com);
        ComponetNameIDMap::iterator itr = com_name_id_map_.find(std::string(com->typeName()));
        itr->second = components_.size() - 1;
        
        com->start();
    }

}
}
