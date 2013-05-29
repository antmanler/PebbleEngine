
# CMAKE_BINARY_DIR is used for inclusion of automatically generated files
include_directories(. ../Shared ../../Bindings/C++ ../../../ ${CMAKE_BINARY_DIR} ${HORDE3D_EXTENSION_INCLUDE_DIRS})

file(GLOB RR_SRC "*.cpp")
file(GLOB INC_PUB "*.h*")

set(HORDE3D_SOURCES 
    # egAnimatables.cpp
    # egAnimation.cpp
    # egCamera.cpp
    # egCom.cpp
    # egExtensions.cpp
    # egGeometry.cpp
    # egLight.cpp
    # egMain.cpp
    # egMaterial.cpp
    # egModel.cpp
    # egModules.cpp
    # egParticle.cpp
    # egPipeline.cpp
    # egPrimitives.cpp
    # egRendererBase.cpp
    # egRenderer.cpp
    # egResource.cpp
    # egScene.cpp
    # egSceneGraphRes.cpp
    # egShader.cpp
    # egTexture.cpp
    # utImage.cpp
    # utOpenGL.cpp
    # config.h
    # egAnimatables.h
    # egAnimation.h
    # egCamera.h
    # egCom.h
    # egExtensions.h
    # egGeometry.h
    # egLight.h
    # egMaterial.h
    # egModel.h
    # egModules.h
    # egParticle.h
    # egPipeline.h
    # egPrerequisites.h
    # egPrimitives.h
    # egRenderer.h
    # egRendererBase.h
    # egResource.h
    # egScene.h
    # egSceneGraphRes.h
    # egShader.h
    # egTexture.h
    # utImage.h
    # utTimer.h
    # utOpenGL.h
    ${RR_SRC}
    ${INC_PUB}
	../../Bindings/C++/Horde3D.h
	${HORDE3D_EXTENSION_HEADERS}
	${HORDE3D_EXTENSION_SOURCES}
	)

add_definitions(-DCMAKE )

add_library(Horde3D SHARED
	${HORDE3D_SOURCES}
	)


if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	IF(MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DSECURE_SCL=0")		
	ENDIF(MSVC)
	target_link_libraries(Horde3D opengl32)
	
else()
	
	install(TARGETS Horde3D
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)
		
	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
		FIND_LIBRARY(CORE_FOUNDATION_LIBRARY CoreFoundation)
		target_link_libraries(Horde3D ${OPENGL_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
	endif()
	
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		target_link_libraries(Horde3D GL ${HORDE3D_EXTENSION_LIBS})
	endif()
	
endif()

set(CP_HEADERS
	${HORDE3D_EXTENSION_HEADERS}
	${CMAKE_CURRENT_SOURCE_DIR}/../../Bindings/C++/Horde3D.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Shared/utMath.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Shared/utXML.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Shared/rapidxml.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Shared/rapidxml_print.h
	)

CopyFilesTo(Horde3D "${CP_HEADERS}" "" ${PUBLICHEADER_DIR}/Horde3D/)
