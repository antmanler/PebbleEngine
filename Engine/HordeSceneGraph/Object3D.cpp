
#include <Horde3D/Horde3D.h>
#include "Object3D.h"

namespace
{
using namespace Pebble;
Property make_property(Property::List type, const void *val)
{
    Property prop = {type, val};
    return prop;
}
}

namespace Pebble
{

Object3D::Object3D()
    : DefaultComponent<IBehavior>()
    , dirty_(false)
    , horde3d_id_(0) {}

Object3D::~Object3D()
{

}

Object3D::Object3D(const Object3D &object)
{
    // copy
    *(this) = object ;
}

void EGCALL Object3D::start()
{
}

void EGCALL Object3D::update()
{
    // transform the node
    /*if (dirty_ && horde3d_id_) {
        h3dSetNodeTransform(horde3d_id_,
                            translation_.x, translation_.y, translation_.z,
                            rotation_.x, rotation_.y, rotation_.z,
                            scale_.x, scale_.y, scale_.z);
    }*/
    // perform the actual update
    //update_implementation();

    // update the bounding box
    /*if (horde3d_id_) {
        h3dGetNodeAABB(horde3d_id_,
                       &bbox_.min.x, &bbox_.min.y, &bbox_.min.z,
                       &bbox_.max.x, &bbox_.max.y, &bbox_.max.z);

    }*/
}

const char* EGCALL Object3D::queryPropertyNameOf(uint32 idx)
{
    switch (idx) {
    case 0: return "horde_id";
    case 1: return "translation";
    case 2: return "rotation";
    case 3: return "scale";
    }
    return "";
}

Property EGCALL Object3D::getProperty(const char* name) const
{
    if ( 0 == strcmp(name, "horde_id") ) return getProperty(0u);
    else if ( 0 == strcmp(name, "translation") ) return getProperty(1u);
    else if ( 0 == strcmp(name, "rotation") ) return getProperty(2u);
    else if ( 0 == strcmp(name, "scale") ) return getProperty(3u);
    Property prop = {Property::PT_UNKNOWN, (0x0)};
    return prop;
}

Property EGCALL Object3D::getProperty(uint32 idx) const
{
    switch (idx) {
    case 0: return make_property(Property::PT_INT32, &horde3d_id_);
    case 1: return make_property(Property::PT_VEC3F, &translation_);
    case 2: return make_property(Property::PT_VEC3F, &rotation_);
    case 3: return make_property(Property::PT_VEC3F, &scale_);
    }
    Property prop = {Property::PT_UNKNOWN, (0x0)};
    return prop;
}

void EGCALL Object3D::setProperty(const char* name, Property prop)
{
    if ( 0 == strcmp(name, "translation") ) setProperty(1u, prop);
    else if ( 0 == strcmp(name, "rotation") ) setProperty(2u, prop);
    else if ( 0 == strcmp(name, "scale") ) setProperty(3u, prop);
}

void EGCALL Object3D::setProperty(uint32 idx, Property prop)
{
    // make a copy of value
    const void* val = prop.val;
    switch (idx) {
    case 1:
        {
            if (prop.p_type == Property::PT_VEC3F) setTranslation(*static_cast<const Vec3f*>(val));
        }
    case 2:
        {
            if (prop.p_type == Property::PT_VEC3F) setRotation(*static_cast<const Vec3f*>(val));
        }
    case 3:
        {
            if (prop.p_type == Property::PT_VEC3F) setScale(*static_cast<const Vec3f*>(val));
        }
    }
}

Vec3f Object3D::translation() const
{
    if ( 0 == hordeId() ) return Vec3f();
    float x=0.0f, y=0.0f, z=0.0f;
    h3dGetNodeTransform(horde3d_id_, &x, &y, &z, NULL, NULL, NULL, NULL, NULL, NULL);
    return Vec3f(x, y, z);
}

Vec3f Object3D::rotation() const
{
    if ( 0 == hordeId() ) return Vec3f();
    float x=0, y=0, z=0;
    h3dGetNodeTransform(horde3d_id_, NULL, NULL, NULL, &x, &y, &z, NULL, NULL, NULL);
    return Vec3f(x, y, z);
}

Vec3f Object3D::scale() const
{
    if ( 0 == hordeId() ) return Vec3f();
    float x=0, y=0, z=0;
    h3dGetNodeTransform(horde3d_id_, NULL, NULL, NULL, NULL, NULL, NULL, &x, &y, &z);
    return Vec3f(x, y, z);
}

void Object3D::setTranslation(const Vec3f& trans)
{
    if ( 0 == hordeId() ) return;
    Vec3f rotating = rotation();
    Vec3f scaling = scale();
    h3dSetNodeTransform(horde3d_id_, trans.x, trans.y, trans.z, rotating.x, rotating.y, rotating.z, scaling.x, scaling.y, scaling.z);
}

void Object3D::setRotation(const Vec3f& rotation)
{
    if ( 0 == hordeId() ) return;
    Vec3f pos = translation();
    Vec3f scaling = scale();
    h3dSetNodeTransform(horde3d_id_, pos.x, pos.y, pos.z, rotation.x, rotation.y, rotation.z, scaling.x, scaling.y, scaling.z);
}

void Object3D::setScale(const Vec3f& scale)
{
    if ( 0 == hordeId() ) return;
    Vec3f pos = translation();
    Vec3f rot = rotation();
    h3dSetNodeTransform(horde3d_id_, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, scale.x, scale.y, scale.z);
}

bool Object3D::active() const
{
    return (h3dGetNodeFlags(horde3d_id_) & H3DNodeFlags::Inactive) == 0;
}

void Object3D::setActive(bool val)
{
    h3dSetNodeFlags(horde3d_id_, val ? 0 : H3DNodeFlags::Inactive, true);
}


}
