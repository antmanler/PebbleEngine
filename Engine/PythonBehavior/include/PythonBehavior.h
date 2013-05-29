#pragma one

#include <string>
#include <Engine/EngineSDK.h>

#define PythonBehaviorName "PythonBehavior"

struct _object;
typedef _object PyObject;

namespace Pebble
{

extern char gContentDir[128];
extern IPluginManager *gPluginMan;

class PythonBehavior : public DefaultComponent<IComponent>
{
public:
    PythonBehavior(IPluginManager *manager);

    //implements
    virtual void EGCALL Destroy();
    virtual const char* EGCALL typeName() const {return PythonBehaviorName;}

    // virtual const char* EGCALL saveToXml();
    /** \breif load scene graph resource from a xml string
      * format : <PythonBehavior src='name or path of the script' />
      */
    virtual void EGCALL loadFromXml(const char* contentbuf, uint32 size);
private:
    IPluginManager *manager_;
    IEngineModules *eg_modules_;
};

typedef void proxy_init(void *self);
typedef void proxy_release(void *self);
typedef void proxy_start(void *self);
typedef void proxy_update(void *self);
typedef char* proxy_typeName(void *self);
typedef bool proxy_processMessage(void *self, char *evt, char *params);
//typedef char* proxy_saveToXml(void *self);
typedef void proxy_loadFromXml(void *self, char *buf, uint32 size);
typedef uint32 proxy_queryPropertyCount(void *self);
typedef char* proxy_queryPropertyNameOf(void *self, uint32 idx);
typedef Property proxy_getPropertyByIdx(void *self, uint32 idx);
typedef Property proxy_getPropertyByName(void *self, char *name);
typedef void proxy_setPropertyByIdx(void *self, uint32 idx, Property prop);
typedef void proxy_setPropertyByName(void *self, char *name, Property prop);

struct PyBehavior
{
    PyObject *pyobj;

    proxy_init *init;
	proxy_release *release;
    proxy_start *start;
    proxy_update *update;
    proxy_typeName *typeName;
    proxy_processMessage *processMessage;
    //proxy_saveToXml *saveToXml;
    proxy_loadFromXml *loadFromXml;
    proxy_queryPropertyCount *queryPropertyCount;
    proxy_queryPropertyNameOf *queryPropertyNameOf;
    proxy_getPropertyByIdx *getPropertyByIdx;
    proxy_getPropertyByName *getPropertyByName;
    proxy_setPropertyByIdx *setPropertyByIdx;
    proxy_setPropertyByName *setPropertyByName;
};

class PyBehaviorWrapper;
// ***** Utility functions for python side
bool isPyBehaviorWrapper(IBehavior *bheavior);
PyObject* castToPythonObject(PyBehaviorWrapper* wrapper);

class PyBehaviorWrapper : public DefaultComponent<IBehavior>
{
public:
    PyBehaviorWrapper(PyBehavior *proxy);
    PyBehaviorWrapper(IBehavior *behavior);
    virtual ~PyBehaviorWrapper();

    // implements
    EG_IMPLEMENTS
    virtual void EGCALL init();
    virtual void EGCALL start();
    virtual void EGCALL update();
    virtual const char* EGCALL typeName() const;
    virtual bool EGCALL processMessage(const char* evt, const char *params);
    //virtual const char* EGCALL saveToXml();
    virtual void EGCALL loadFromXml(const char* contentbuf, uint32 size);
    // property system
    virtual uint32 EGCALL queryPropertyCount() const;
    virtual const char* EGCALL queryPropertyNameOf(uint32 idx);
    virtual Property EGCALL getProperty(const char* name) const;
    virtual Property EGCALL getProperty(uint32 idx) const;
    virtual void EGCALL setProperty(const char* name, Property prop);
    virtual void EGCALL setProperty(uint32 idx,  Property prop);

	/** 
	 * This is an additional function for script object,
	 * and will be invoked before actually destroy the instance
	 */
	void pythonObjRelease();

    // python side
    IBehavior* gocBehavior(char *type_name);
    //bool addBehavior(PyBehaviorWrapper *behavior);
    bool removeBehavior(PyBehaviorWrapper *behavior);
    void sendMessage(char* evt_name, char* param);
//    /**
//      * \brief Broadcast message to the entities in 'world'
//      *
//      * This is a expensive method, that will send message for each entities' components
//      *
//      * @param evt_name name of the event
//      * @param param parameters encoded in JSON
//      */
//    void broadcastMessage(char* evt_name, char* param);

private:
    //IPluginManager *manager_;
    IBehavior *behavior_;
    PyBehavior *proxy_;
    friend PyObject* castToPythonObject(PyBehaviorWrapper* wrapper);
    friend bool isPyBehaviorWrapper(IBehavior *bheavior);
};

}




