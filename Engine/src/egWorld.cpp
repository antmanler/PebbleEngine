
#include <algorithm>
#include "egWorld.h"
#include "egEntity.h"
#include "egModule.h"
#include <iostream>
using namespace std;

namespace {

using namespace Pebble;
struct EntityUpdator {
    void operator () (Entity *ent) {
        ent->update();
    }
};

struct MessageBroadcastor {
    MessageBroadcastor(const char* evt, const char* val)
        : evt_(evt)
        , val_(val)
    {
        std::cout << evt << " " << evt_.c_str() << std::endl;
    }

    void operator () (Entity *ent) {
        // ensure all components and behaviours have been initialized
        ent->initComponents();
        ent->sendMessage(evt_.c_str(), val_.c_str());
    }
private:
    string evt_;
    string val_;
};

}

namespace Pebble {

World::World()
{
    // create a root entity
    createEntity("__root__");
}

World::~World()
{
    // clear the world
    clear();
    delete entities_.back();
    entities_.clear();
    EngineModules::engineLogger()->writeDebugInfo("World released.");
}

IEntity* EGCALL World::createEntity(const char* name)
{
    string ent_name = resolve_name(name);

    uint32 id = 0;
    Entity *ent = NULL;
    if (free_list_.empty()) {
        // create a new id
        id = entities_.size();
        ent = new Entity(id);
        entities_.push_back(ent);

    } else {
        // reuse an available id
        id = free_list_.top(); free_list_.pop();
        ent = new Entity(id);
        entities_[id] = ent;
    }

    if (ent) {
        name2idx_.insert(make_pair<string, uint32>(ent_name, id));
        ent->name_ = ent_name;
        EngineModules::engineLogger()->writeDebugInfo("Added an entity: %s", ent_name.c_str());
        // emit signal added
        sig_entity_added_.Emit(ent);
    }

    return ent;
}

void EGCALL World::removeEntity(const uint32 id)
{
    if ( id>0 && id<entities_.size() ) {
        IEntity *ent = entities_.at(id);
        Name2Index::iterator itr = name2idx_.find(string(ent->name()));
        name2idx_.erase(itr);
        // emit removed signal
        sig_entity_removed_.Emit(ent);
        // release the entity
        ent->Destroy();
        entities_[id] = 0x0;
        // add to free list
        free_list_.push(id);
    }
}

uint32 EGCALL World::renameEntity(const uint32 id, const char* name)
{
    IEntity *ent = getEntity(id);
    if ( ent && !strcmp(ent->name(), name)) {
        string new_name = resolve_name(name);
        // remove the old one
        Name2Index::iterator itr = name2idx_.find(ent->name());
        name2idx_.erase(itr);
        // add new one
        name2idx_.insert(make_pair<string, uint32>(string(new_name), id));
        EngineModules::engineLogger()->writeDebugInfo("Entity %s change name to %s", ent->name(), new_name.c_str());
        static_cast<Entity*>(ent)->name_ = new_name;
        // return the new postfix of new name
        if ( strcmp(new_name.c_str(), name) != 0 ) {
            // get the postfix ( name(n) )
            string ret = new_name.substr(strlen(name)+1, new_name.length() - strlen(name)-2);
            return atoi(ret.c_str());
        }
        return 0;
    }
    return -1;
}

void EGCALL World::clear()
{
    name2idx_.clear();
    while ( !free_list_.empty() ) {
        uint32 id = free_list_.top();
        free_list_.pop();
        entities_[id] = 0x0;
    }

    Entities::iterator itr = entities_.begin();
    for (; itr!=entities_.end(); ++itr) {
        delete *itr;
    }
    entities_.clear();
    // insert a root entity
    createEntity("__root__");
}

IEntity* EGCALL World::findEntity(const char* id)
{
    Name2Index::iterator itr = name2idx_.find(string(id)) ;
    if ( itr != name2idx_.end() )
        return getEntity(itr->second);
    return 0x0;
}

IEntity* EGCALL World::getEntity(const uint32 id)
{
    if (id<=entities_.size())
        return entities_.at(id) ;
    else
        return 0x0;
}

void EGCALL World::findEntityHasComponentOf(const char* com_type)
{
    find_entities_.clear();
    if ( 0 == strcmp(com_type, "") ) {
        Entities::iterator itr = entities_.begin();
        for(++itr; itr != entities_.end(); ++itr) {
            if (*itr != 0x0) find_entities_.push_back(*itr);
        }
    } else {
        Entities::iterator itr = entities_.begin();
        for(++itr; itr != entities_.end(); ++itr) {
            if (*itr != 0x0 && (*itr)->getComponent(com_type))
                find_entities_.push_back(*itr);
        }
    }
}

uint32 EGCALL World::getFindCount()
{
    return find_entities_.size();
}

IEntity* EGCALL World::getFindeResult(const uint32 idx)
{
    if (idx < find_entities_.size())
        return find_entities_.at(idx);
    return NULL;
}

void World::update()
{
    for_each(entities_.begin(), entities_.end(), EntityUpdator());
}

void EGCALL World::sendMessage(const char* evt_name, const char* param)
{
    for_each(entities_.begin(), entities_.end(), MessageBroadcastor(evt_name, param));
}

string World::resolve_name(const char* name)
{
    string ent_name(name);
    Name2Index::iterator itr = name2idx_.find(ent_name);

    if ( itr != name2idx_.end() ) {
        // reslove name confilict
        char new_name[128] = {'\0'};
        uint32 i = 1;
        for (; itr != name2idx_.end(); ++i) {
            sprintf(new_name, "%s(%d)", name, i);
            itr = name2idx_.find(new_name);
        }
        EngineModules::engineLogger()->writeWarning("Entity with name '%s' already exists, renaming to '%s'",
                                                    name, new_name);
        ent_name = new_name;
    }
    return ent_name;
}

}
