
#pragma once

#include "Platform.h"
#include "EngineEntity.h"
#include "EngineWorld.h"
#include "EngineCommon.h"
//#include "EngineController.h"
#include "EngineModules.h"

#define SDK_VERSION  1
#define COMPONET_HOST "ComponetManager"

#define COM_TYPE_NAME(type) #type

namespace Pebble
{

class IComponent : public Yaps::IExportInterface
{
public:
    /* \breif Initiailise the component act as a contrucotr.
      *
      * init is called first and should be used as the constructor.
      * So you can construct a object with init() and create a reference to another script.
      * Then start is called. Start is used if you want to delay the order of initialization
      * and pass info to the other script about the constructed object
      * and to do stuff that is not closely tied to instantiation.
      */
    virtual void EGCALL init() = 0;
    virtual void EGCALL start() = 0 ;
    
    /* \breif get type of this component, it must be a unique indentifier
            */
    virtual const char* EGCALL typeName() const = 0;
    //virtual const char* EGCALL name() const = 0;
    
    virtual IEntity* EGCALL owner() = 0;
    
    virtual void EGCALL setOwner(IEntity* owner) = 0;
    
    virtual void EGCALL update() = 0;
    
    /* \breif process the evnt
     * @param evt name of event
     * @param param string encoded in JSON
            */
    virtual bool EGCALL processMessage(const char* evt, const char* param) = 0;
    
    /* \breif This will be invoked when the plugin is going to enable.
            */
    virtual void EGCALL enable(bool enable) = 0 ;
    
    /* \breif Return true if this component is enabled.
            */
    virtual bool EGCALL isEnabled() = 0 ;
    
    /* \breif Serialize inernal states of the component
            */
    //virtual const char* EGCALL saveToXml() = 0;
    virtual void EGCALL loadFromXml(const char* contentbuf, uint32 size) = 0;
};

class IComponentManager : public Yaps::IFactory
{
public:
    virtual void EGCALL update() const = 0;
};

class IPluginManager : public Yaps::IManager
{
public:
    virtual void EGCALL registerComponent(IComponentManager *com_man) = 0;
    /**
    * \note this method will also release the factory
    */
    virtual void EGCALL removeComponent(IComponentManager *com_man) = 0;
    virtual IComponent* EGCALL createComponent(const char* type_name) = 0;
    virtual IEngineModules* EGCALL getModules() = 0;
};

class IRenderer : public IComponent
{
public:
    virtual void EGCALL render() = 0;
    //virtual void EGCALL resize(uint32 w, uint32 h) = 0;
    virtual uint32 EGCALL priority() const = 0;
};

struct ENGINE_EXPORT Property
{
    enum List {
        PT_UNKNOWN = 0,
        PT_BOOL,
        PT_INT32,
        PT_FLOAT,
        PT_VEC2F,
        PT_VEC3F,
        PT_VEC4F,
        PT_CSTRING,
        PT_COMPONENT,
        PT_CUSTORM_PRT
    } ;

    List p_type;
    const void* val;
};

class IBehavior : public IComponent
{
public:
    /* \breif Property system
        */
    virtual uint32 EGCALL queryPropertyCount() const = 0;
    virtual const char* EGCALL queryPropertyNameOf(uint32 idx) = 0;
    virtual  Property EGCALL getProperty(const char* name) const = 0;
    virtual  Property EGCALL getProperty(uint32 idx) const = 0;
    virtual void EGCALL setProperty(const char* name,  Property prop) = 0;
    virtual void EGCALL setProperty(uint32 idx,  Property prop) = 0;
};

#define EG_IMPLEMENTS \
    virtual void YAPSCALL Destroy() { \
    ::delete(this) ; \
}

template <typename ComponentType>
class DefaultComponent : public ComponentType
{
public:
    DefaultComponent():enabled_(true), owner_(0x0){}
    
    virtual IEntity* EGCALL owner() { return this->owner_;}
    virtual void EGCALL setOwner(IEntity* owner) {this->owner_ = owner;}
    
    virtual void EGCALL init() {}

    virtual void EGCALL start() {}

    virtual void EGCALL update() {}
    
    virtual bool EGCALL processMessage(const char*, const char*) {return true;}
    
    virtual void EGCALL enable(bool enable) {this->enabled_ = enable ;}
    
    virtual bool EGCALL isEnabled() {return this->enabled_;}
    
    // virtual const char* EGCALL saveToXml() {return "";}
    virtual void EGCALL loadFromXml(const char* contentbuf, uint32 size) {}
    
protected:
    bool enabled_ ;
    IEntity* owner_;
};

/**
        Utility template for making a default factory
    */
class DefaultComManager : public IComponentManager {
public:

    virtual void EGCALL update() const {}
};

}
