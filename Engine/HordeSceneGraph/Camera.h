
#pragma once

#include <vector>
#include <Engine/EngineSDK.h>

namespace Pebble
{

class Camera : public DefaultComponent<IBehavior>, public has_slots<>
{
public:
    Camera (IEngineModules *eg_modules);
    virtual ~Camera ();
    Camera(const Camera& object);

    // implementations
    EG_IMPLEMENTS
    virtual const char* EGCALL typeName() const {return COM_TYPE_NAME(Camera);}
    virtual void EGCALL start();
    virtual void EGCALL update();
    // virtual const char* EGCALL saveToXml();
    virtual void EGCALL loadFromXml(const char* buf, uint32 size);
    //virtual bool EGCALL processMessage(const char *evt, void* pparam);

    // property system
    virtual uint32 EGCALL queryPropertyCount() const { return 2u;}
    virtual const char* EGCALL queryPropertyNameOf(uint32 idx);
    virtual  Property EGCALL getProperty(const char* name) const;
    virtual Property EGCALL getProperty(uint32 idx) const;
    virtual void EGCALL setProperty(const char* name, Property prop);
    virtual void EGCALL setProperty(uint32 idx, Property prop);

    void preRender();
    void postRender();

    void onResize(uint32 w, uint32 h);
    int hordeId() const {return horde3d_id_;}
    int depth() const {return depth_;}
    void setDepth(const int d) {depth_ = d;}
    float fov() const ;//{return fov_;}
    void setFov(const float fov);
    
protected:
    IEngineModules* eg_modules_;
    int depth_;
    int horde3d_id_;
    uint32 w_;
    uint32 h_;
    bool auto_resize_;
    //float woh_;
//    float fov_;
    char serial_data_[256];
};
}
