
#pragma once

#ifndef DLL
#	if defined( WIN32 ) || defined( _WINDOWS )
#		define DLL extern "C" __declspec( dllimport )
#	else
#  if defined( __GNUC__ ) && __GNUC__ >= 4
#   define DLL extern "C" __attribute__ ((visibility("default")))
#  else
#		define DLL extern "C"
#  endif
#	endif
#endif

/**
 * initializes the physics world
 */
DLL void h3dextInitPhysics();
/**
 * releases the physics instance (delete from memory)
 */
DLL void h3dextReleasePhysics();
/**
 * "render" function of physics world (updates physics transformations)
 */
DLL void h3dextUpdatePhysics();
/**
 * resets the world to it's initial state
 */
DLL void h3dextResetPhysics();
/**
 * Clear the world
 */
DLL void h3dextClearPhysics();
/** 
 * Creates a new PhysicsNode based on the data provided to this function
 */
DLL void h3dextCreatePhysicsNode(const char* xmlData, int hordeID);
/**
 * Removes a physics node
 */
DLL void h3dextRemovePhysicsNode( int hordeID );
