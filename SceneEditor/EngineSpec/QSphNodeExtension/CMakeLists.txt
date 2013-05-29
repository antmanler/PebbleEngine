cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

file(GLOB RR_SRC "*.cpp")
file(GLOB RR_INC "*.h*")
file(GLOB RR_FROM "*.ui")

ADD_DEFINITIONS(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

SET(QT_USE_QTXML TRUE)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
    "${CMAKE_CURRENT_BINARY_DIR}" 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    )

QT4_WRAP_CPP(GEN_SRC ${RR_INC})
QT4_WRAP_UI(GEN_FORM ${RR_FROM})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	
ADD_LIBRARY( QSphNodeExtension SHARED 
    ${RR_SRC} 
	${RR_INC}
	${RR_FROM}
	${GEN_SRC}
    ${GEN_FORM}     
)

TARGET_LINK_LIBRARIES( QSphNodeExtension
    dflib
	#QPropertyEditor
    PebbleEditorCore
    ${QT_LIBRARIES} 
)
