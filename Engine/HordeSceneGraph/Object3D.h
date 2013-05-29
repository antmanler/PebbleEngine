
#pragma once

#include <vector>
#include <Engine/EngineSDK.h>

#define OBJECT3D "Object3D"

namespace Pebble
{

class Object3D : public DefaultComponent<IBehavior>
{
public:
    Object3D ();
    virtual ~Object3D ();
    Object3D(const Object3D& object);

    // implementations
    EG_IMPLEMENTS
    virtual const char* EGCALL typeName() const {return COM_TYPE_NAME(Object3D);}
    virtual void EGCALL start();
    virtual void EGCALL update();

    // property system
    virtual uint32 EGCALL queryPropertyCount() const { return 4u;}
    virtual const char* EGCALL queryPropertyNameOf(uint32 idx);
    virtual Property EGCALL getProperty(const char* name) const;
    virtual  Property EGCALL getProperty(uint32 idx) const;
    virtual void EGCALL setProperty(const char* name,  Property prop);
    virtual void EGCALL setProperty(uint32 idx,  Property prop);

    void setHordeId(const int id) {horde3d_id_=id; makeDirty();}
    int hordeId() const {return horde3d_id_;}

    // transform
    Vec3f translation() const;
    Vec3f rotation() const;
    Vec3f scale() const;
    void setTranslation(const Vec3f& trans);
    void setRotation(const Vec3f& rotation);
    void setScale(const Vec3f& scale);

    bool active() const;
    void setActive(bool val);

    void makeDirty() {dirty_ = true;}
    
protected:
    Vec3f  translation_;
    Vec3f  rotation_;
    Vec3f  scale_;

    bool  dirty_;
    int   horde3d_id_;

};
}
