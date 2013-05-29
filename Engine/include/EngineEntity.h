
#pragma once

#include "Platform.h"

namespace Pebble
{
class IComponent;

class IEntity
{
public:
    /* \breif public signals */
    //typedef Signal0<> Signal;
    typedef ISignal0 Signal;

public:
    virtual void EGCALL Destroy() = 0;
    /**
          * \brief A unique name (string) of the entity within the EngineWorld
          *
          * @returns unique id
          */
    virtual const char* EGCALL name() const = 0;

    //virtual void EGCALL setName(const char* name) = 0;

    /**
          * \brief the EngineWorld index for this entity, readonly
          * @return index of entity in world
          */
    virtual uint32 EGCALL id() const = 0;

    virtual void EGCALL update() = 0;

    /**
         * \brief Query specific component from the entity
         *
         * @param id the specific ID of one component
         * @return pointer to the IComponent instance or 0 if no such component is aggregated in the entity
         */
    virtual IComponent* EGCALL getComponent(const char* type_name) = 0;

    /**
         * \breif add a component instance to entity
         *
         * @param com the compont instance
         * @return ture if added scessfully, or false if there are confilicts
         */
    virtual bool EGCALL addComponent(IComponent* com) = 0;

    virtual bool EGCALL removeComponent(IComponent *com, bool release = true) = 0;

    virtual uint32 EGCALL getComponentCount() const = 0;
    virtual IComponent* EGCALL getComponent(const uint32 idx) = 0;

    /*
         * \breif populate an event, and dispatch it to all the components in this entity
         */
    virtual void EGCALL sendMessage(const char* evt_name, const char* param) = 0;

    virtual Signal* EGCALL sigUpdate() = 0;
};
}
