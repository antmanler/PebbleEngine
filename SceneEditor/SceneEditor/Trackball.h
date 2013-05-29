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

#pragma once

#include "MMaths.h"

class Mouse 
{
public:
    enum MODIFIER {
        SHIFT = 0x100, CTRL, ALT
    } ;
    
    enum BUTTON {
        LEFT = 0x200, MIDDLE, RIGHT
    };
    
    enum STATE {
        DOWN = 0x300, UP, MOTION
    };
};

class TrackballManipulator {
public:
    TrackballManipulator();
    void setCamera(int camera);
    void handleMouseFunc(int button, int state, int x, int y, int modifier);
    void handleMoveFunc(int x, int y);
    void handleResize(int new_xres, int new_yres);    
    void setPivot(float x, float y, float z);
    void setTranslateSpeed(float speed) { translate_speed_ =  speed; }
    
private:
    struct InteractionState {
        InteractionState():modifier(0), button(0), state(0) {}
        InteractionState(int modifier, int button, int state)
        : modifier(modifier), button(button), state(state)
        {}
        int modifier; // Shift,Ctrl,Alt
        int button;   // Left,Middle,Right
        int state;    // Down,Up
        int last_x, last_y;
    };
    
    void call_func(int x, int y);
    
    void translate(int x, int y);
    void rotate(int x, int y);
    void zoom(int x, int y);
    
    // utils
    vec3 compute_vector(int x, int y);
    inline mat4 get_camera_local_mat();
    void set_camera_local_mat(const mat4 &mat);
    
    // Data
    int camera_;
    InteractionState inter_state_;
    vec4 viewport_;
    mat4 start_mat_;
    vec3 pivot_;
    vec3 start_cam_pos_;
    vec3 start_pivot_;
    float rotate_speed_;
    float zoom_speed_;
    float translate_speed_;
};
