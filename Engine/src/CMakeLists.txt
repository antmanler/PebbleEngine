cmake_minimum_required(VERSION 2.8)

project(EngineCore)

include_directories(${PUBLICHEADER_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# private include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB RR_SRC "*.cpp")
file(GLOB INC_PRI "include/*.h*")

set(RR_SRC ${RR_SRC})
set(RR_INC ${INC_PUB} ${INC_PRI})

source_group(include FILES ${INC_PRI})

add_library(Engine SHARED
	${RR_SRC}
	${RR_INC}
	)

#target_link_libraries(Engine BulletDynamics BulletCollision LinearMath Horde3D Horde3DUtils)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	IF(MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSECURE_SCL=0")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DENGINE_DLL=1")		
	ENDIF(MSVC)
endif()

add_dependencies(Engine EngineSDK)
