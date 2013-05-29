cmake_minimum_required(VERSION 2.8)

project(Horde3D)

# terrain extension (check egExtensions.cpp if it's activated)
option(HORDE3D_BUILD_TERRAIN "Build the terrain extension into Horde3D" ON)
if(HORDE3D_BUILD_TERRAIN)
	set(HORDE3D_EXTENSION_SOURCES
		${HORDE3D_EXTENSION_SOURCES}
		${Horde3D_SOURCE_DIR}/Extensions/Terrain/Source/terrain.h
		${Horde3D_SOURCE_DIR}/Extensions/Terrain/Source/terrain.cpp
		${Horde3D_SOURCE_DIR}/Extensions/Terrain/Source/extension.h
		${Horde3D_SOURCE_DIR}/Extensions/Terrain/Source/extension.cpp
		)
	set(HORDE3D_EXTENSION_INSTALLS
		${HORDE3D_EXTENSION_INSTALLS}
		"extMan().installExtension( new Horde3DTerrain::ExtTerrain() );\n"		
		)
	set(HORDE3D_EXTENSION_INCLUDES
		${HORDE3D_EXTENSION_INCLUDES}
		"\n#include \"Extensions/Terrain/Source/extension.h\"\n"
		)
	set(HORDE3D_EXTENSION_HEADERS
		${HORDE3D_EXTENSION_HEADERS}
		${Horde3D_SOURCE_DIR}/Extensions/Terrain/Bindings/C++/Horde3DTerrain.h
		)
endif(HORDE3D_BUILD_TERRAIN)

option(HORDE3D_BUILD_SPH "Build the SPH particle render extension into Horde3D" ON)
if(HORDE3D_BUILD_SPH)
	set(HORDE3D_EXTENSION_SOURCES
		${HORDE3D_EXTENSION_SOURCES}
		${Horde3D_SOURCE_DIR}/Extensions/SPH/Source/egSph.h
		${Horde3D_SOURCE_DIR}/Extensions/SPH/Source/egSph.cpp
		${Horde3D_SOURCE_DIR}/Extensions/SPH/Source/sph_extension.h
		${Horde3D_SOURCE_DIR}/Extensions/SPH/Source/sph_extension.cpp
		)
	set(HORDE3D_EXTENSION_INSTALLS
		${HORDE3D_EXTENSION_INSTALLS}
		"extMan().installExtension( new Horde3DSph::ExtSph() );\n"		
		)
	set(HORDE3D_EXTENSION_INCLUDES
		${HORDE3D_EXTENSION_INCLUDES}
		"\n#include \"Extensions/SPH/Source/sph_extension.h\"\n"
		)
	# set(HORDE3D_EXTENSION_INCLUDE_DIRS
	# 	${HORDE3D_EXTENSION_INCLUDE_DIRS}
	# 	${Horde3D_SOURCE_DIR}/Extensions/Terrain/Bindings/C++
	# 	${PebbleInWater_SOURCE_DIR}/Dependence/Bullet/src
	# 	)
	# set(HORDE3D_EXTENSION_LIBS
	# 	${HORDE3D_EXTENSION_LIBS}
	# 	BulletDynamics
	# 	)
	set(HORDE3D_EXTENSION_HEADERS
		${HORDE3D_EXTENSION_HEADERS}
		${Horde3D_SOURCE_DIR}/Extensions/SPH/Bindings/C++/Horde3DSph.h
		)
endif()

# option(HORDE3D_BUILD_PHYSICS "Build the BulletPhysics extension into Horde3D" ON)
# if(HORDE3D_BUILD_TERRAIN)
# 	set(HORDE3D_EXTENSION_SOURCES
# 		${HORDE3D_EXTENSION_SOURCES}
# 		${Horde3D_SOURCE_DIR}/Extensions/BulletPhysics/Source/egPhysics.h
# 		${Horde3D_SOURCE_DIR}/Extensions/BulletPhysics/Source/egPhysics.cpp
# 		${Horde3D_SOURCE_DIR}/Extensions/BulletPhysics/Source/physics_extension.h
# 		${Horde3D_SOURCE_DIR}/Extensions/BulletPhysics/Source/physics_extension.cpp
# 		)
# 	set(HORDE3D_EXTENSION_INSTALLS
# 		${HORDE3D_EXTENSION_INSTALLS}
# 		"extMan().installExtension( new Horde3DPhysics::ExtPhysics() );\n"		
# 		)
# 	set(HORDE3D_EXTENSION_INCLUDES
# 		${HORDE3D_EXTENSION_INCLUDES}
# 		"\n#include \"Extensions/BulletPhysics/Source/physics_extension.h\"\n"
# 		)
# 	set(HORDE3D_EXTENSION_INCLUDE_DIRS
# 		${HORDE3D_EXTENSION_INCLUDE_DIRS}
# 		${Horde3D_SOURCE_DIR}/Extensions/Terrain/Bindings/C++
# 		${PebbleInWater_SOURCE_DIR}/Dependence/Bullet/src
# 		)
# 	# FIXME must be cross platform
# 	set(HORDE3D_EXTENSION_LIBS
# 		${HORDE3D_EXTENSION_LIBS}
# 		BulletDynamics
# 		)
# 	set(HORDE3D_EXTENSION_HEADERS
# 		${HORDE3D_EXTENSION_HEADERS}
# 		${Horde3D_SOURCE_DIR}/Extensions/BulletPhysics/Bindings/C++/Horde3DPhysics.h
# 		)
# endif(HORDE3D_BUILD_TERRAIN)


# # sound extension (check egExtensions.cpp if it's activated)
# option(HORDE3D_BUILD_SOUND "Build the sound extension into Horde3D" OFF)
# if(HORDE3D_BUILD_SOUND)
# 	set(HORDE3D_EXTENSION_SOURCES
# 		${HORDE3D_EXTENSION_SOURCES}
# 		${Horde3D_SOURCE_DIR}/Extensions/Sound/Source/decode_vorbis.cpp
# 		${Horde3D_SOURCE_DIR}/Extensions/Sound/Source/decode_wave.cpp
# 		${Horde3D_SOURCE_DIR}/Extensions/Sound/Source/sound_extension.cpp
# 		${Horde3D_SOURCE_DIR}/Extensions/Sound/Source/listener.cpp
# 		${Horde3D_SOURCE_DIR}/Extensions/Sound/Source/manager.cpp
# 		${Horde3D_SOURCE_DIR}/Extensions/Sound/Source/soundNode.cpp
# 		${Horde3D_SOURCE_DIR}/Extensions/Sound/Source/soundResource.cpp
# 		)
# 	set(HORDE3D_EXTENSION_INSTALLS
# 		${HORDE3D_EXTENSION_INSTALLS}
# 		"extMan().installExtension( new Horde3DTerrain::ExtTerrain() );\n"
# 		)
# 	set(HORDE3D_EXTENSION_INCLUDES
# 		${HORDE3D_EXTENSION_INCLUDES}
# 		"\n#include \"Extensions/Sound/Source/sound_extension.h\"\n"
# 		)
# 	# FIXME must be cross platform
# 	set(HORDE3D_EXTENSION_INCLUDE_DIRS
# 		${HORDE3D_EXTENSION_INCLUDE_DIRS}
# 		"/usr/include/AL"
# 		)
# 	# FIXME must be cross platform
# 	set(HORDE3D_EXTENSION_LIBS
# 		${HORDE3D_EXTENSION_LIBS}
# 		"openal"
# 		"vorbisfile"
# 		)
# endif(HORDE3D_BUILD_SOUND)

# create egExtensions.cpp
CONFIGURE_FILE(Horde3D/Source/Horde3DEngine/egExtensions_auto_include.h.in ${CMAKE_BINARY_DIR}/egExtensions_auto_include.h)
CONFIGURE_FILE(Horde3D/Source/Horde3DEngine/egExtensions_auto_install.h.in ${CMAKE_BINARY_DIR}/egExtensions_auto_install.h)

add_subdirectory(Horde3D)
IF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Extensions)
  add_subdirectory(Extensions)
ENDIF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Extensions)

# IF(HORDE3D_BUILD_TERRAIN)
# SET_PROPERTY(TARGET Horde3DTerrain         PROPERTY FOLDER "Horde3D/Extensions")
# ENDIF(HORDE3D_BUILD_TERRAIN)
# 
# IF(HORDE3D_BUILD_SPH)
# SET_PROPERTY(TARGET Horde3DTerrain         PROPERTY FOLDER "Horde3D/Extensions")
# ENDIF(HORDE3D_BUILD_SPH)
