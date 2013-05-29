cmake_minimum_required(VERSION 2.8)

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#

SET(project_policies
    CMP0001 # NEW: CMAKE_BACKWARDS_COMPATIBILITY should no longer be used.
    CMP0002 # NEW: Logical target names must be globally unique.
    CMP0003 # NEW: Libraries linked via full path no longer produce linker search paths.
    CMP0004 # NEW: Libraries linked may NOT have leading or trailing whitespace.
    CMP0005 # NEW: Preprocessor definition values are now escaped automatically.
    CMP0006 # NEW: Installing MACOSX_BUNDLE targets requires a BUNDLE DESTINATION.
    CMP0007 # NEW: List command no longer ignores empty elements.
    CMP0008 # NEW: Libraries linked by full-path must have a valid library file name.
    CMP0009 # NEW: FILE GLOB_RECURSE calls should not follow symlinks by default.
    CMP0010 # NEW: Bad variable reference syntax is an error.
    CMP0011 # NEW: Included scripts do automatic cmake_policy PUSH and POP.
    CMP0012 # NEW: if() recognizes numbers and boolean constants.
    CMP0013 # NEW: Duplicate binary directories are not allowed.
    CMP0014 # NEW: Input directories must have CMakeLists.txt
    )
FOREACH(policy ${project_policies})
    IF(POLICY ${policy})
        CMAKE_POLICY(SET ${policy} NEW)
    ENDIF()
ENDFOREACH()

project(PebbleInWater)

#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  "${CMAKE_SOURCE_DIR}/CMake")

#include(AddConfigFiles)
#include(InstallSDK)

SET(PUBLICHEADER_DIR ${CMAKE_BINARY_DIR}/inc)

function(CopyFilesTo TAG HEADERS FDIR DIR)
    foreach(header ${HEADERS})
        get_filename_component(target ${header} NAME)
        add_custom_command(
            TARGET "${TAG}" POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS
            -E
            copy_if_different
            ${FDIR}${header}
            ${DIR}${target}
            COMMENT "Copying Plugin custom sdk headers to the sdk folder."
        )
    endforeach()
endfunction()

# accumulate list of sources of extension modules
# this can not be done inside the Extensions folder since cmake can only pass variables to subdirectories, never upwards...
set(HORDE3D_EXTENSION_SOURCES)
set(HORDE3D_EXTENSION_INCLUDES)
set(HORDE3D_EXTENSION_INSTALLS)
set(HORDE3D_EXTENSION_INCLUDE_DIRS)
set(HORDE3D_EXTENSION_LIBS)
set(HORDE3D_EXTENSION_HEADERS)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/WinMS)
    else()
        set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/Win/Bin)
    endif()
else()
    set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME})
endif()

if(DEFINED CMAKE_CONFIGURATION_TYPES)
else()
    set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME}/Bin)
endif()

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR} CACHE STRING "Where binaries and .dll files go" FORCE)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR} CACHE STRING "Where binaries and .dll files go" FORCE)

# if(CMAKE_CONFIGURATION_TYPES)
#     # set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
#     foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
#         string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
#         set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR} )
#         set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR} )
# #     set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
#     endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
# endif()

# Python libraries
find_package(PythonLibs REQUIRED)
include_directories("${PYTHON_INCLUDE_DIR}")

# OpenGL
find_package(OpenGL REQUIRED)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories("${OPENGL_INCLUDE_DIR}")
endif()

#
# Organize projects into folders
#
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Dependence)
SET_PROPERTY(TARGET PythonQt               PROPERTY FOLDER "Dependence")
SET_PROPERTY(TARGET Lua                    PROPERTY FOLDER "Dependence")
SET_PROPERTY(TARGET Horde3D                PROPERTY FOLDER "Dependence/Horde3D")
SET_PROPERTY(TARGET ColladaConv	           PROPERTY FOLDER "Dependence/Horde3D")
SET_PROPERTY(TARGET Horde3DUtils	       PROPERTY FOLDER "Dependence/Horde3D")
SET_PROPERTY(TARGET BulletDynamics         PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET BulletCollision	       PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET BulletMultiThreaded	   PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET BulletSoftBody	       PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET LinearMath	           PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET MiniCL	               PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET ConvexDecomposition	   PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET GIMPACTUtils PROPERTY FOLDER "Dependence/Bullet")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
SET_PROPERTY(TARGET BulletSoftBodySolvers_DX11        PROPERTY FOLDER "Dependence/Bullet")
SET_PROPERTY(TARGET BulletSoftBodySolvers_OpenCL_Mini PROPERTY FOLDER "Dependence/Bullet")
endif()

add_subdirectory(Engine)
SET_PROPERTY(TARGET Engine	           PROPERTY FOLDER "PiwEngine")
SET_PROPERTY(TARGET EngineSDK	       PROPERTY FOLDER "PiwEngine")
SET_PROPERTY(TARGET SceneGraph	       PROPERTY FOLDER "PiwEngine/Components")
SET_PROPERTY(TARGET PythonBehavior	   PROPERTY FOLDER "PiwEngine/Components")
SET_PROPERTY(TARGET BulletPhysics	   PROPERTY FOLDER "PiwEngine/Components")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
SET_PROPERTY(TARGET hack4win           PROPERTY FOLDER "PiwEngine")
endif()

add_subdirectory(PebblePlayer)
# SET_PROPERTY(TARGET EditorCore              PROPERTY FOLDER "oldEditor")
# SET_PROPERTY(TARGET QPropertyEditorPIW      PROPERTY FOLDER "oldEditor")
# SET_PROPERTY(TARGET PebblePlayer            PROPERTY FOLDER "PebblePlayer")

add_subdirectory(SceneEditor)
SET_PROPERTY(TARGET SceneEditor            PROPERTY FOLDER "SceneEditor")
SET_PROPERTY(TARGET EditorLib		       PROPERTY FOLDER "SceneEditor/Libs")
SET_PROPERTY(TARGET PebbleEditorCore       PROPERTY FOLDER "SceneEditor/Libs")
SET_PROPERTY(TARGET QXmlTree	           PROPERTY FOLDER "SceneEditor/Libs")
#SET_PROPERTY(TARGET QPropertyEditor        PROPERTY FOLDER "SceneEditor/Libs")
SET_PROPERTY(TARGET qtga    	       	   PROPERTY FOLDER "SceneEditor/PlugIns")
SET_PROPERTY(TARGET qpsd	           	   PROPERTY FOLDER "SceneEditor/PlugIns")
SET_PROPERTY(TARGET TerrainToolPlugIn  	   PROPERTY FOLDER "SceneEditor/PlugIns")
SET_PROPERTY(TARGET QTerrainNodeExtension  PROPERTY FOLDER "SceneEditor/PlugIns")



