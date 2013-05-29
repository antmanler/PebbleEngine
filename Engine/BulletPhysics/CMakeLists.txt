cmake_minimum_required(VERSION 2.8)
project(BulletPhysics)

include_directories(${PUBLICHEADER_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# bullet
include_directories(${PebbleInWater_SOURCE_DIR}/Dependence/Bullet/src)

file(GLOB RR_SRC "*.cpp")
file(GLOB INC_PRI "*.h*")

set(RR_SRC ${RR_SRC})
set(RR_INC ${INC_PUB} ${INC_YAPS} ${INC_PRI})

add_library(BulletPhysics SHARED
	${RR_SRC}
	${RR_INC}
	)

target_link_libraries(BulletPhysics BulletDynamics BulletCollision LinearMath Horde3D Horde3DUtils)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	IF(MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSECURE_SCL=0")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DBULLETPHYSICS_DLL=1")		
	ENDIF(MSVC)
endif()