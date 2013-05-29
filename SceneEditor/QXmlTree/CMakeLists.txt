cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

SET(QXmlTree_SOURCES
   QXmlTreeModel.cpp
	QXmlTreeNode.cpp
	QXmlTreeUndoCommands.cpp
	QXmlTreeView.cpp
)

SET(QXmlTree_QOBJECT_HEADERS
   QXmlTreeModel.h
	QXmlTreeNode.h
	QXmlTreeView.h
)

SET(QXmlTree_HEADERS
	QXmlTreeUndoCommands.h
)

QT4_WRAP_CPP(QXmlTree_HEADERS_MOC ${QXmlTree_QOBJECT_HEADERS})

# INCLUDE_DIRECTORIES(../../include)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY( QXmlTree STATIC
    ${QXmlTree_SOURCES} 
	${QXmlTree_HEADERS}
	${QXmlTree_QOBJECT_HEADERS}
    ${QXmlTree_HEADERS_MOC}     
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

TARGET_LINK_LIBRARIES( QXmlTree )