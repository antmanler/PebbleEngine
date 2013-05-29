// ****************************************************************************************
//
// Pebble Scene Editor
// --------------------------------------
// Copyright (C) 2011 Antmanler
//
// This file is part of the Pebble Scene Editor.
//
// The Pebble Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License
//
// The Pebble Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Credits : Volker Wiendl who created the major work of the original editor and Horde3D
// is a flexible graphics engine. see <http://www.horde3d.org> and <http://http://goo.gl/Me2HT>
//
// ****************************************************************************************


//#include <optixu/optixu_matrix_namespace.h>
#include <iostream>

# ifdef _WIN32
#   include <cfloat>
#   define ISFINITE _finite
# else
#   include <cmath>
#   define ISFINITE std::isfinite
# endif

#include <Horde3D/Horde3D.h>

//#include "Core.h"
#include "Trackball.h"

//-----------------------------------------------------------------------------
// 
// Helpers
//
//-----------------------------------------------------------------------------

namespace {
    
    inline float Clamp(float val, float min, float max)
    {
        return val > min ? (val < max ? val : max) : min;
    }
}


TrackballManipulator::TrackballManipulator()
: camera_(0)
, viewport_(0.0)
, start_mat_()
, pivot_()
, start_cam_pos_()
, start_pivot_()
, rotate_speed_(1.5)
, zoom_speed_(1.0)
, translate_speed_(1.0)
{
    
}

void TrackballManipulator::setCamera(int camera)
{
    camera_ = camera;
    if (camera != 0) {
        viewport_.x = h3dGetNodeParamI( camera_, H3DCamera::ViewportXI);
        viewport_.y = h3dGetNodeParamI( camera_, H3DCamera::ViewportYI);
        viewport_.z = h3dGetNodeParamI( camera_, H3DCamera::ViewportWidthI);
        viewport_.w = h3dGetNodeParamI( camera_, H3DCamera::ViewportHeightI);
    }
}

void TrackballManipulator::handleMouseFunc(int button, int state, int x, int y, int modifier)
{
    if (0 == camera_)  return ;

    switch(state) {
        case Mouse::DOWN:
        {
            x -= viewport_.x;
            y -= viewport_.w - 1 - (viewport_.y + viewport_.w -1);
            
            // check that the click is in the viewport
            int w = viewport_.z;
            int h = viewport_.w;
            
            if (x<0 || y<0 || x>=w || y>=h) break;
            inter_state_ = InteractionState(modifier, button, state);
            inter_state_.last_x = x;
            inter_state_.last_y = y;
            start_mat_ = get_camera_local_mat();
//            pivot_ = start_mat_.getTranslationPart() ;
            start_pivot_ = pivot_;
            start_cam_pos_ = start_mat_.getTranslationPart();
            call_func(x,y);
            break;
        }
        case Mouse::UP: break;
    }
}

void TrackballManipulator::handleMoveFunc(int x, int y)
{
    inter_state_.state = Mouse::MOTION;
    call_func(x,y);
}

void TrackballManipulator::handleResize(int new_xres, int new_yres)
{
    if (camera_ != 0) {
        viewport_.x = h3dGetNodeParamI( camera_, H3DCamera::ViewportXI);
        viewport_.y = h3dGetNodeParamI( camera_, H3DCamera::ViewportYI);
        viewport_.z = new_xres;
        viewport_.w = new_yres;
    }
}

void TrackballManipulator::setPivot(float x, float y, float z)
{
    pivot_ = vec3(x, y, z);
}

void TrackballManipulator::call_func(int x, int y)
{
    int modifier = inter_state_.modifier;
    int button   = inter_state_.button;
    
    if (modifier == 0 && button == Mouse::LEFT){
        rotate(x, y);
    }
    
    if ((modifier == 0 && button == Mouse::MIDDLE) || 
        (modifier == Mouse::ALT && button == Mouse::LEFT) ) {
        translate(x, y);
    }
    
    if (modifier == 0 && button == Mouse::RIGHT) {
        zoom(x, y);
    }

}

void TrackballManipulator::translate(int x, int y)
{
    float tx = (inter_state_.last_x - x) / 450.0f;
    float ty = -(inter_state_.last_y - y) / 450.0f;
    
    tx *= translate_speed_;
    ty *= translate_speed_;
    float distance = (start_cam_pos_ - pivot_).getLength();
    mat4 cam_mat = get_camera_local_mat() ;
    vec3 up    = cam_mat.getColumn(1).xyz();
    vec3 right = cam_mat.getColumn(0).xyz();
    cam_mat.setTranslationPart(start_cam_pos_ + up*distance*ty + right*distance*tx);
    
    set_camera_local_mat(cam_mat) ;
    pivot_ = start_pivot_ + up*distance*ty + right*distance*tx;
}

void TrackballManipulator::rotate(int x, int y)
{
    if( x==inter_state_.last_x  && y==inter_state_.last_y ) return;
    
    vec3 a = compute_vector(inter_state_.last_x, inter_state_.last_y);
    vec3 b = compute_vector(x, y);
    vec3 n = a.crossProduct(b);
    
    n.normalize();
    a.normalize();
    b.normalize();
    float dot_a_b = a.dotProduct(b);
    
    dot_a_b = Clamp(dot_a_b,-1.0,+1.0);
    vec3 nc =  get_camera_local_mat().getRotatedVector3(n);
    nc.normalize();
    
    float alpha = -1 * acos(dot_a_b);
    alpha = alpha * rotate_speed_;

    mat4 local_mat;
	local_mat.translate(pivot_) ;
    local_mat.rotate(nc, alpha*RAD_TO_DEG);
	local_mat.translate(-pivot_);
    local_mat = local_mat*start_mat_;
    
    set_camera_local_mat(local_mat);
    
    // get local matrix
    start_mat_ = local_mat;
    inter_state_.last_x = x;
    inter_state_.last_y = y;
}

void TrackballManipulator::zoom(int x, int y)
{
    float t = (y-inter_state_.last_y) / 250.0f;
    t *= zoom_speed_ ;
    mat4 cam_mat = get_camera_local_mat() ;
    float distance = (start_cam_pos_ - pivot_).getLength();
    
    vec3 camera_pos = start_cam_pos_ - cam_mat.getColumn(2).xyz()*t*distance;

    cam_mat.setTranslationPart(camera_pos);
    set_camera_local_mat(cam_mat);

}

vec3 TrackballManipulator::compute_vector(int x, int y)
{
    vec3 c(viewport_.z * 0.5f, viewport_.w * 0.5f, 0);
    
    float sphere_x = c.x;
    float sphere_y = c.y;
    
    vec3 v((float)x,(float)y,0);
    
    v -= c;
    v.x /= sphere_x;
    v.y /= sphere_y;
    v.y = -v.y;
    
    float z2 = 1.0f - v.x*v.x - v.y*v.y;
    if (z2 < 0)   z2 = 0;
    
    v.z = sqrt( z2 );
    v.normalize();
    
    return v;
}

mat4 TrackballManipulator::get_camera_local_mat()
{
    if (0 == camera_) return mat4();
    const float *loc_mat;
    h3dGetNodeTransMats(camera_, &loc_mat, NULL);
    return mat4(loc_mat);
}

void TrackballManipulator::set_camera_local_mat(const mat4 &mat)
{
    if ( 0 == camera_) return;
    h3dSetNodeTransMat(camera_, mat);
}
