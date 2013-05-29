
#pragma once
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <Engine/EngineWorld.h>

namespace Pebble
{
class Entity;

class World : public IWorld
{
    typedef std::vector<Entity*> Entities;
    typedef std::map<std::string, uint32> Name2Index;
    typedef std::stack<uint32> FreeList;

    friend class EngineModules;

public:

    /**
          * \brief Return the entity instance to a given name
          *
          * @param id the entity's name
          * @return pointer to the instance corresponding to this name or 0 if no such entity exists
          */
    virtual IEntity* EGCALL findEntity(const char* id);

    /**
          * \brief Return the entity instance to a given world ID
          *
          * @param id the entity's world ID
          * @return pointer to the instance corresponding to this name or 0 if no such entity exists
          */
    virtual IEntity* EGCALL getEntity(const uint32 id);

    virtual void EGCALL findEntityHasComponentOf(const char* com_type);
    virtual uint32 EGCALL getFindCount();
    virtual IEntity* EGCALL getFindeResult(const uint32 idx);

    /**
          * \brief Change the entity name
          * @param id the identifier of the entiy
          * @return 0 if succeed; has conflict return the postfix of new name, eg Name2; otherwise return -1
          */
    virtual uint32 EGCALL renameEntity(const uint32 id, const char* name);

    /**
          * \brief Creates a new entity
          *
          * Registers a new entity with the specified ID within the GameEngine's world. If
          * another entity with this name has been created before, the previous entity will be
          * returned and no new entity will be created.
          *
          * @param id a unique name for the new entity
          * @return the newly created entity or the entity that has been created (but not removed) before
          * using the same id (name)
          */
    virtual IEntity* EGCALL createEntity(const char* name);

    /**
          * \brief Removes an entity from the world
          *
          * Tries to find the specified entity and deletes it's instance if found
          * @param id the entity's index within the GameEngine's world
          * @see releaseEntity(const EntityID&)
          */
    virtual void EGCALL removeEntity(const uint32 id);

    virtual void EGCALL clear();

    virtual void EGCALL sendMessage(const char* evt_name, const char* param);

    // update the entites in world one by one
    void update() ;

    void initialize();
    
    /**
     * \brief Signals for entity modified
     */
    virtual IWorld::EntityModifySignal* EGCALL sigEntityAdded() {return &sig_entity_added_;}
    virtual IWorld::EntityModifySignal* EGCALL sigEntityRemoved() {return &sig_entity_removed_;}

private:
    // Default constructor
    World();
    World(const World&) {}
    ~World();

    std::string resolve_name(const char* name);

private:
    Entities    entities_;
    Entities    find_entities_;
    Name2Index  name2idx_;
    FreeList    free_list_;
    // signals
    signal1<IEntity*> sig_entity_added_;
    signal1<IEntity*> sig_entity_removed_;

};
}
