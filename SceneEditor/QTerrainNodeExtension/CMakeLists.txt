cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

SET(QTerrainNodeExtension_SOURCES
	ExtTerrainPlugIn.cpp
	QTerrainNode.cpp
	QTerrainNodePage.cpp
)
SET(QTerrainNodeExtension_HEADERS
	ExtTerrainPlugIn.h
	QTerrainNode.h
	QTerrainNodePage.h
)
SET(QTerrainNodeExtension_FORMS
	QTerrainNodePage.ui
)


ADD_DEFINITIONS(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)
SET(QT_USE_QTXML TRUE)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

QT4_WRAP_CPP(QTerrainNodeExtension_HEADERS_MOC ${QTerrainNodeExtension_HEADERS})
QT4_WRAP_UI(QTerrainNodeExtension_FORMS_HEADERS ${QTerrainNodeExtension_FORMS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	
ADD_LIBRARY( QTerrainNodeExtension SHARED 
    ${QTerrainNodeExtension_SOURCES} 
	${QTerrainNodeExtension_HEADERS}
	${QTerrainNodeExtension_FORMS_HEADERS}
    ${QTerrainNodeExtension_HEADERS_MOC}     
)

TARGET_LINK_LIBRARIES( QTerrainNodeExtension ${QT_LIBRARIES} PebbleEditorCore QXmlTree Lua Horde3D Horde3DUtils )