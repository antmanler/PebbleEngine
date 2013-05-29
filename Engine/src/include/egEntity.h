#pragma once

#include <vector>
#include <map>
#include <Engine/EngineEntity.h>

namespace Pebble
{
class IComponent;

class Entity : public IEntity
{
    friend class World;

public:
    Entity(uint32 id);
    virtual ~Entity();
    virtual void EGCALL Destroy() {::operator delete(this) ;}
    /**
          * \brief A unique name (string) of the entity within the EngineWorld
          *
          * @returns unique id
          */
    virtual const char* EGCALL name() const { return name_.c_str(); }

    /**
          * \brief the EngineWorld index for this entity, readonly
          * @return index of entity in world
          */
    virtual uint32 EGCALL id() const { return id_; }

    virtual void EGCALL update();

    /**
         * \brief Query specific component from the entity
         *
         * @param id the specific ID of one component
         * @return pointer to the IComponent instance or 0 if no such component is aggregated in the entity
         */
    virtual IComponent* EGCALL getComponent(const char* type_name);

    virtual uint32 EGCALL getComponentCount() const { return components_.size();}
    virtual IComponent* EGCALL getComponent(const uint32 idx);

    /**
         * \breif add a component instance to entity
         *
         * @param com the compont instance
         * @return ture if added scessfully, or false if there are confilicts
         */
    virtual bool EGCALL addComponent(IComponent* com);

    virtual bool EGCALL removeComponent(IComponent *com, bool release = true);

    /*
         * \breif populate an event, and dispatch it to all the components in this entity
         */
    virtual void EGCALL sendMessage(const char* evt_name, const char *param);

    virtual Signal* EGCALL sigUpdate() {return &sig_update_;}

    void initComponents();

private:
    typedef std::map<std::string, int> ComponetNameIDMap;
    uint32 id_;
    std::string name_;
//    Signal sig_update_;
    signal0<> sig_update_;
    std::vector<IComponent*> components_;
    std::vector<IComponent*> uninit_coms_;
    ComponetNameIDMap com_name_id_map_;

};
}
