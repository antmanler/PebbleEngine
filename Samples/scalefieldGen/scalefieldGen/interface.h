
#pragma  once

#ifndef DLL
#	if defined( WIN32 ) || defined( _WINDOWS )
#		define DLL extern "C" __declspec( dllimport )
#	else
#		if defined( __GNUC__ ) && __GNUC__ >= 4
#		  define DLLEXP extern "C" __attribute__ ((visibility("default")))
#   	else
#		  define DLLEXP extern "C"
#   	endif
#	endif
#endif

DLL void scaleFieldInit();
DLL void scaleFieldRelease();

DLL void scaleFieldGenerator (const float* points, const int points_num,
	const float xmin, const float ymin, const float zmin,
	const float xmax, const float ymax, const float zmax,
	float* saclefiled, const int width, const int height, const int depth);