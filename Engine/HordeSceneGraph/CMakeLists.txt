cmake_minimum_required(VERSION 2.8)

project(HordeSceneGraph)

include_directories(${PUBLICHEADER_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB RR_SRC "*.cpp")
file(GLOB INC_PRI "*.h*")

set(RR_SRC ${RR_SRC})
set(RR_INC ${INC_PUB} ${INC_YAPS} ${INC_PRI})

source_group(include FILES ${INC_PRI})

add_library(SceneGraph SHARED
	${RR_SRC}
	${RR_INC}
	)


target_link_libraries(SceneGraph Horde3D Horde3DUtils)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	IF(MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSECURE_SCL=0")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSECENGRAPH_DLL=1")		
	ENDIF(MSVC)
else()
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		target_link_libraries(SceneGraph ${OPENGL_LIBRARIES})
	endif()
endif()

