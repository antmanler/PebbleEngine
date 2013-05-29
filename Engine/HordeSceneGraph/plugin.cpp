
#include <Engine/EngineSDK.h>
#include "Camera.h"
#include "Object3D.h"
#include "SceneGraph.h"


namespace Pebble
{
using namespace Yaps;

class CameraManager : public DefaultComManager
{
public:

    CameraManager(IPluginManager *manager) : manager_(manager) {}
    // implements
    EG_IMPLEMENTS
    virtual IExportInterface* EGCALL Create()
    {
        return new Camera(manager_->getModules());
    }
    virtual const char* EGCALL GetFactoryName() { return COM_TYPE_NAME(Camera); }
private:
    IPluginManager* manager_;
};

class Object3DManager : public DefaultComManager
{
public:
    // implements
    EG_IMPLEMENTS
    virtual IExportInterface* EGCALL Create()
    {
        return new Object3D();
    }
    virtual const char* EGCALL GetFactoryName() { return COM_TYPE_NAME(Object3D); }
};

class SceneGraphManager : public DefaultComManager
{
public:

    SceneGraphManager(IPluginManager *manager) : manager_(manager), inst_(0x0) {}

    virtual ~SceneGraphManager()
    {
        manager_->getModules()->engineLogger()->writeDebugInfo("SceneGraph: Release SceneGraphManager");
        if (inst_) {
            manager_->getModules()->engineLogger()->writeDebugInfo("SceneGraph: Release SceneGraph instance");
            delete inst_;
        }
    }

    // implements
    EG_IMPLEMENTS
    virtual IExportInterface* EGCALL Create()
    {
        if ( 0x0 == inst_ ) inst_ = new SceneGraph(manager_);
        return inst_;
    }

    virtual void EGCALL update() const
    {
        if (inst_) inst_->genEntities();
    }

    virtual const char* EGCALL GetFactoryName() { return COM_TYPE_NAME(SceneGraph); }

private:
    IPluginManager* manager_;
    SceneGraph* inst_;
};

// register to component system
#if defined (PLATFORM_IOS)
void YAPSCALL SceneGraph_register_plugin(IPluginManager *man) {
#else
DLLEXP void YAPSCALL register_plugin(IPluginManager *man) {
#endif
    man->registerComponent(new CameraManager(man));
    man->registerComponent(new Object3DManager());
    man->registerComponent(new SceneGraphManager(man));
}

}


