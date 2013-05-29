cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

SET(qtga_SOURCES
	main.cpp
	qtgahandler.cpp
)
SET(qtga_HEADERS
	qtgahandler.h	
)


QT4_WRAP_CPP(qtga_HEADERS_MOC ${qtga_HEADERS})

INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_BINARY_DIR}")

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY( qtga SHARED 
    ${qtga_SOURCES} 
    ${qtga_HEADERS_MOC}     
)

TARGET_LINK_LIBRARIES( qtga ${QT_LIBRARIES} )