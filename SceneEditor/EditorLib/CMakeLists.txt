cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

SET(EditorLib_SOURCES
   HighLighter.cpp
   LuaFunctions.cpp
   TextEdit.cpp
)
SET(EditorLib_QOBJECT_HEADERS
   HighLighter.h
   TextEdit.h
)

SET(EditorLib_HEADERS
   LuaFunctions.h
)


QT4_WRAP_CPP(EditorLib_HEADERS_MOC ${EditorLib_QOBJECT_HEADERS})

# INCLUDE_DIRECTORIES("../../include"  "../../../Dependencies/Lua/src")
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY( EditorLib STATIC
    ${EditorLib_SOURCES} 
	${EditorLib_HEADERS}
	${EditorLib_QOBJECT_HEADERS}
    ${EditorLib_HEADERS_MOC}     
)

TARGET_LINK_LIBRARIES( EditorLib Lua )