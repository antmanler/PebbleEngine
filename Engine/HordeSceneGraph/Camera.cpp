
#include <Horde3D/Horde3D.h>
#include <Engine/utXML.h>
#include <cmath>
#include "Object3D.h"
#include "Camera.h"

namespace
{
using namespace Pebble;
Property make_property(Property::List type, const void *val)
{
    Property prop = {type, val};
    return prop;
}

const char* gDefaultParams = "{\"kwargv\": {}, \"argv\": []}";

}

namespace Pebble
{

Camera::Camera(IEngineModules *eg_modules)
    : eg_modules_(eg_modules)
    , DefaultComponent<IBehavior>()
    , horde3d_id_(0)
    , depth_(0)
    , w_(320)
    , h_(240)
//    , fov_(45.0)
{
    memset(serial_data_, 0 , sizeof(serial_data_));
    // connect to resize signal
    //    eg_modules->sigResize()->Connect(this, &Camera::onResize);
    eg_modules->sigResize()->connect(make_slot(this, &Camera::onResize));
}

Camera::~Camera()
{
    //    eg_modules_->sigResize()->Disconnect(this, &Camera::onResize);
    eg_modules_->sigResize()->disconnect(this);
}

Camera::Camera(const Camera &object)
{
    // copy
    *(this) = object ;
}

void EGCALL Camera::start()
{
    // update h3d id
    if (owner()) {
        IComponent * com = owner()->getComponent(COM_TYPE_NAME(Object3D));
        if (com) {
            Object3D *obj = static_cast<Object3D*>(com);
            //eg_modules_->engineLogger()->writeDebugInfo("Camera id %d", obj->hordeId());
            horde3d_id_ = obj->hordeId();
            // allocate viewport
            if (!auto_resize_) {
                auto_resize_ = true;
                onResize(w_, h_);
                auto_resize_ = false;
            } else {
                Vec2i sz = eg_modules_->engineConfig()->getConfig("WindowSize")->cast<Vec2i>();
                onResize(sz.x, sz.y);
            }
        }
    }
}

void EGCALL Camera::update()
{
//    if ( !isEnabled() ) return;

}

/*bool EGCALL Camera::processMessage(const char *evt, void *pparam)
{
    if ( 0==strcmp(evt, "Resize") && horde3d_id_ > 0 ) {
        Vec3f *sz = static_cast<Vec3f*>(pparam);
//        Vec3f sz = param->cast<Vec3f>();
        w_ = ftoi_r(sz->x);
        h_ = ftoi_r(sz->y);
        h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportXI, 0 );
        h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportYI, 0 );
        h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportWidthI, w_);
        h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportHeightI, h_ );
        H3DRes pipeline = h3dGetNodeParamI(horde3d_id_, H3DCamera::PipeResI ) ;
        h3dResizePipelineBuffers(pipeline, w_, h_);

        setFov(fov());
    }
}*/

void Camera::preRender()
{
    if (owner()) owner()->sendMessage("preCamRender", gDefaultParams);
}

void Camera::postRender()
{
    if (owner()) owner()->sendMessage("postCamRender", gDefaultParams);
}

void Camera::onResize(uint32 w, uint32 h)
{
    if (!auto_resize_) return;
    w_ = w;//ftoi_r(sz->x);
    h_ = h;//ftoi_r(sz->y);
    h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportXI, 0 );
    h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportYI, 0 );
    h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportWidthI, w_);
    h3dSetNodeParamI( horde3d_id_, H3DCamera::ViewportHeightI, h_ );
    H3DRes pipeline = h3dGetNodeParamI(horde3d_id_, H3DCamera::PipeResI ) ;
    h3dResizePipelineBuffers(pipeline, w_, h_);

    setFov(fov());
}

float Camera::fov() const
{
    static const float c = 114.59155902616465f; //360.0/PI
    if ( horde3d_id_ > 0 ) {
        float top = h3dGetNodeParamF(horde3d_id_, H3DCamera::TopPlaneF, 0);
        float nearp = h3dGetNodeParamF(horde3d_id_, H3DCamera::NearPlaneF, 0);
        return atanf(top/nearp)*c;
    }

    return 0.0;
}

void Camera::setFov(const float fov)
{
    static char params[48] = {0};
    if ( horde3d_id_ > 0 ) {
        float nearp = h3dGetNodeParamF(horde3d_id_, H3DCamera::NearPlaneF, 0);
        float farp = h3dGetNodeParamF(horde3d_id_, H3DCamera::FarPlaneF, 0);
        h3dSetupCameraView( horde3d_id_, fov, float(w_)/float(h_), nearp, farp );
        //        fov_ = fov;
        // notify the other components
        sprintf(params, "{\"kwargv\": {}, \"argv\": [%f]}", fov);
        if (owner()) owner()->sendMessage("setFov", params);
    }
}

const char* EGCALL Camera::queryPropertyNameOf(uint32 idx)
{
    switch (idx) {
    case 0: return "depth";
    case 1: return "fov";
    case 2: return "viewportX";
    case 3: return "viewportY";
    case 4: return "resizable";
    }
    return "";
}

Property Camera::getProperty(const char* name) const
{
    if ( 0 == strcmp(name, "depth") ) return getProperty(0u);
    else if ( 0 == strcmp(name, "fov") ) return getProperty(1u);
    else if ( 0 == strcmp(name, "viewportX") ) return getProperty(2u);
    else if ( 0 == strcmp(name, "viewportY") ) return getProperty(3u);
    else if ( 0 == strcmp(name, "resizable") ) return getProperty(4u);
    Property prop = {Property::PT_UNKNOWN, 0x0};
    return prop;
}

Property EGCALL Camera::getProperty(uint32 idx) const
{
    static float fov_tmp = 0.0f;
    switch (idx) {
    case 0: return make_property(Property::PT_INT32, &depth_);
    case 1:
        fov_tmp = fov();
        return make_property(Property::PT_FLOAT, &fov_tmp);
    case 2: return make_property(Property::PT_FLOAT, &w_);
    case 3: return make_property(Property::PT_FLOAT, &h_);
    case 4: return make_property(Property::PT_BOOL, &auto_resize_);
    }
    Property prop = {Property::PT_UNKNOWN, 0x0};
    return prop;
}

void EGCALL Camera::setProperty(const char* name, Property prop)
{
    if ( 0 == strcmp(name, "depth") ) setProperty(0u, prop);
    else if ( 0 == strcmp(name, "fov") ) setProperty(1u, prop);
    else if ( 0 == strcmp(name, "viewportX") ) setProperty(2u, prop);
    else if ( 0 == strcmp(name, "viewportY") ) setProperty(3u, prop);
    else if ( 0 == strcmp(name, "resizable") ) setProperty(4u, prop);
}

void EGCALL Camera::setProperty(uint32 idx, Property prop)
{
    // make a copy of value
    const void *val = prop.val;
    switch (idx) {
    case 0:
        {
            if (prop.p_type == Property::PT_INT32) setDepth(*static_cast<const int*>(val));
        }
        break;
    case 1:
        {
            if (prop.p_type == Property::PT_FLOAT) setFov(*static_cast<const float*>(val));
        }
        break;
    case 2:
        {
            if (prop.p_type == Property::PT_FLOAT) w_ = (*static_cast<const float*>(val));
        }
        break;
    case 3:
        {
            if (prop.p_type == Property::PT_FLOAT) h_ = (*static_cast<const float*>(val));
        }
        break;
    case 4:
        {
            if (prop.p_type == Property::PT_FLOAT) auto_resize_ = (*static_cast<const bool*>(val));
        }
        break;
    }

    if ((idx == 2 || idx ==3) && (w_>0 && h_>0)) {
        // resize and change auto_resize to false
        auto_resize_ = true;
        onResize(w_, h_);
        auto_resize_ = false;
    }
}

//const char* EGCALL Camera::saveToXml()
//{
//    static const char *tpl = "<Camera depth=%d fov=%.2f />";
//    sprintf(serial_data_, tpl, depth(), fov());
//    return serial_data_;
//}

void EGCALL Camera::loadFromXml(const char* contentbuf, uint32 size)
{
    XMLDoc doc;
    doc.parseBuffer( contentbuf, size );
    if( doc.hasError() ) {
        eg_modules_->engineLogger()->writeError("Camera::loadFromXml(): camera parsing failed, %d@%s", __LINE__, __FILE__);
        return;
    }

    XMLNode rootNode = doc.getRootNode();
    if ( !rootNode.isEmpty() && (strcmp(rootNode.getName(), "Camera") == 0) ) {
        depth_ = static_cast<int>(atoi(rootNode.getAttribute("depth", "0")));
        //fov_ = static_cast<float>(atof(rootNode.getAttribute("fov", "45.0")));
        w_ = static_cast<int>(atoi(rootNode.getAttribute("viewportX", "0")));
        h_ = static_cast<int>(atoi(rootNode.getAttribute("viewportY", "0")));
        auto_resize_ = !(w_ > 0 && h_ >0);
    } else {
        eg_modules_->engineLogger()->writeError("Camera::loadFromXml(): "
                                                "it's not a valid camera data, %d@%s", __LINE__, __FILE__);
    }
}

}
