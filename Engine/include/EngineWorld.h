
#pragma once

#include "Platform.h"

namespace Pebble
{
class IEntity;

class IWorld
{
public:
    typedef ISignal1<IEntity*> EntityModifySignal;
    
public:
    /**
          * \brief Return the entity instance to a given name
          *
          * @param id the entity's name
          * @return pointer to the instance corresponding to this name or 0 if no such entity exists
          */
    virtual IEntity* EGCALL findEntity(const char* id) = 0;

    /**
          * \brief Return the entity instance to a given world ID
          *
          * @param id the entity's world ID
          * @return pointer to the instance corresponding to this name or 0 if no such entity exists
          */
    virtual IEntity* EGCALL getEntity(const uint32 id) = 0;

    virtual void EGCALL findEntityHasComponentOf(const char* com_type) = 0;
    virtual uint32 EGCALL getFindCount() = 0;
    virtual IEntity* EGCALL getFindeResult(const uint32 idx) = 0;

    /**
          * \brief Change the entity name
          * @param id the identifier of the entiy
          * @return 0 if succeed; has conflict return the postfix of new name, eg Name2; otherwise return -1
          */
    virtual uint32 EGCALL renameEntity(const uint32 id, const char* name) = 0;

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
    virtual IEntity* EGCALL createEntity(const char* name) = 0;

    /**
          * \brief Removes an entity from the world
          *
          * Tries to find the specified entity and deletes it's instance if found
          * @param id the entity's index within the GameEngine's world
          * @see releaseEntity(const EntityID&)
          */
    virtual void EGCALL removeEntity(const uint32 id) = 0;

    virtual void EGCALL sendMessage(const char* evt_name, const char* param) = 0;

    virtual void EGCALL clear() = 0;
    
    /**
     * \brief Signals for entity modified
     */
    virtual EntityModifySignal* EGCALL sigEntityAdded() = 0;
    virtual EntityModifySignal* EGCALL sigEntityRemoved() = 0;
};
}
