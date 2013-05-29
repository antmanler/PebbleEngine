cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

SET(TerrainToolPlugIn_SOURCES
	ConverterWizard.cpp
	TerrainToolPlugIn.cpp
)
SET(TerrainToolPlugIn_HEADERS
	ConverterWizard.h
	TerrainToolPlugIn.h
)
SET(TerrainToolPlugIn_FORMS
	ConverterWizard.ui
)

QT4_WRAP_CPP(TerrainToolPlugIn_HEADERS_MOC ${TerrainToolPlugIn_HEADERS})
QT4_WRAP_UI(TerrainToolPlugIn_FORMS_HEADERS ${TerrainToolPlugIn_FORMS})

SET(QT_USE_QTXML TRUE)

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

ADD_LIBRARY( TerrainToolPlugIn SHARED 
    ${TerrainToolPlugIn_SOURCES} 
	${TerrainToolPlugIn_FORMS_HEADERS}
    ${TerrainToolPlugIn_HEADERS_MOC}     
)

TARGET_LINK_LIBRARIES( TerrainToolPlugIn ${QT_LIBRARIES} PebbleEditorCore )