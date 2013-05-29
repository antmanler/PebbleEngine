#ifndef _SPH_RENDER_H_
#define _SPH_RENDER_H_

#include "vector3f.h"

typedef struct
{
    vector3 p;
    vector3 n;
    float val;
} cellvertex;

typedef struct
{
    cellvertex *v[8];
    cellvertex vmem[8];
} gridcell;

typedef struct
{
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    float voxelsize;
    //gridcell g;
    float* density;
} implicit_surface;

typedef struct  
{
    // For marching cubes
    float mc_stride;
    int mc_range;
    float iso_radius;
    //coordinate of volume corner
    vector3 volpos;

    int vol_width;
    int vol_height;
    int vol_depth;
    float mass;
    float *volume;
    implicit_surface im_vol;

}renderofs;

typedef void AddTriangleCallback(vector3 *v1, vector3 *v2, vector3 *v3,
                                 vector3 *n1, vector3 *n2, vector3 *n3);

renderofs* render_create(float stride, int range, float iradius);
void render_delete(renderofs *r);
void render_create_implicit(renderofs* r, float k, const vector3* pos, int n_particles);
void renderCreatImplicit(renderofs *r, int xR, int yR, int zR, const float *dis);
void render_marching_cube(renderofs *render, float threshold, AddTriangleCallback *callback);

#endif

