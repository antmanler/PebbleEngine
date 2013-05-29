cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

SET(qpsd_SOURCES
	main.cpp
	qpsdhandler.cpp
)
SET(qpsd_HEADERS
	qpsdhandler.h	
)


QT4_WRAP_CPP(qpsd_HEADERS_MOC ${qpsd_HEADERS})

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY( qpsd SHARED 
    ${qpsd_SOURCES} 
    ${qpsd_HEADERS_MOC}     
)

TARGET_LINK_LIBRARIES( qpsd ${QT_LIBRARIES} )