
#include <limits>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "utPlatform.h"
#include "utMath.h"

namespace SPH 
{

/**
 * This function will be invoked just once before scaleFiledGenerator used
 * usually, any initialization should be done here 
 */
DLLEXP void scaleFieldInit()
{
	// Leave blank
}

/**
 * Release resources
 */
DLLEXP void scaleFieldRelease()
{
	// Leave blank
}


DLLEXP void scaleFieldGenerator (const float* points, const int points_num,
	const float xmin, const float ymin, const float zmin,
	const float xmax, const float ymax, const float zmax,
	float* saclefiled, const int width, const int height, const int depth)
{
	using namespace Horde3D;

	const float iso_radius = 0.03811f;
#ifndef M_PI
	const float M_PI=3.14159265358979323846f;
#endif
	const float mass = 0.00005f ;
	const float kernel = mass*315.0f/(64.0f*M_PI*pow(iso_radius, 9));
	const int   mc_range = 1;

	// clear data field
	memset(saclefiled, 0, width*height*depth*sizeof(float));

	// compute the stride
	Vec3f extends(xmax - xmin, ymax - ymin, zmax - zmin);
	Vec3f stride3f(extends.x/(width -2.0f*mc_range - 6.0f),
		extends.y/(height - 2.0f*mc_range - 6.0f),
		extends.z/(depth - 2.0f*mc_range - 6.0f));

	float stride = stride3f.x;
	stride = maxf(stride, stride3f.y);
	stride = maxf(stride, stride3f.z);
	float inv_stride = 1.0f/stride;

	int min_x = 0;
	int min_y = 0;
	int min_z = 0;

	Vec3f volpos(xmin - stride, ymin - stride, zmin - stride);
	// position in volume
	Vec3f piv;
	const Vec3f* pos = reinterpret_cast<const Vec3f *>(points);
	for (int i = 0; i < points_num; ++i) {

		piv = (pos[i] - volpos) * inv_stride;

		for (int z = -mc_range; z <= mc_range; z++) {
			for (int y = -mc_range; y <= mc_range; y++) {
				for (int x = -mc_range; x <= mc_range; x++) {

					float dx = ((int)piv.x)*stride + volpos.x - pos[i].x + stride*x;
					float dy = ((int)piv.y)*stride + volpos.y - pos[i].y + stride*y;
					float dz = ((int)piv.z)*stride + volpos.z - pos[i].z + stride*z;

					float h2_r2 = maxf(iso_radius*iso_radius - (dx*dx + dy*dy + dz*dz), 0.0f);

					float v = kernel*h2_r2*h2_r2*h2_r2;


					int vindex = (int)(piv.x + x - min_x + 2)
						+ ((int)(piv.y + y - min_y + 2))*width
						+ ((int)(piv.z + z - min_z + 2))*width*height;

					if ((vindex < 0) || (vindex > width*height*depth)) continue;

					saclefiled[vindex] += v;
				}
			}
		}
	}

}

}