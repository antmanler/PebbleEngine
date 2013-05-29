cmake_minimum_required(VERSION 2.8)

project(Engine)

include_directories(${PUBLICHEADER_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB RR_SRC "src/*.cpp")
file(GLOB INC_PUB "include/*.h*")
#file(GLOB INC_YAPS "include/Yaps/*.h*")

add_definitions(-DCMAKE )

add_custom_target(EngineSDK)
CopyFilesTo(EngineSDK "${INC_PUB}" "" ${PUBLICHEADER_DIR}/Engine/)
#CopyFilesTo(EngineSDK "${INC_YAPS}" "" "${PUBLICHEADER_DIR}/Engine/Yaps")

add_subdirectory(src)
# build plugins
add_subdirectory(HordeSceneGraph)
add_subdirectory(BulletPhysics)
add_subdirectory(PythonBehavior)
