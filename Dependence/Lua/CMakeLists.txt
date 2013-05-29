cmake_minimum_required(VERSION 2.8) 

IF(MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

SET(Lua_SOURCES
	src/lapi.c
	src/lauxlib.c
	src/lbaselib.c
	src/lcode.c
	src/ldblib.c
	src/ldebug.c
	src/ldo.c
	src/ldump.c
	src/lfunc.c
	src/lgc.c
	src/linit.c
	src/liolib.c
	src/llex.c
	src/lmathlib.c
	src/lmem.c
	src/loadlib.c
	src/lobject.c
	src/lopcodes.c
	src/loslib.c
	src/lparser.c
	src/lstate.c
	src/lstring.c
	src/lstrlib.c
	src/ltable.c
	src/ltablib.c
	src/ltm.c
	src/lua.c
	src/lundump.c
	src/lvm.c
	src/lzio.c
	src/print.c
)
SET(Lua_HEADERS
	src/lapi.h
	src/lauxlib.h
	src/lcode.h
	src/ldebug.h
	src/ldo.h
	src/lfunc.h
	src/lgc.h
	src/llex.h
	src/llimits.h
	src/lmem.h
	src/lobject.h
	src/lopcodes.h
	src/lparser.h
	src/lstate.h
	src/lstring.h
	src/ltable.h
	src/ltm.h
	src/lua.h
	src/luaconf.h
	src/lualib.h
	src/lundump.h
	src/lvm.h
	src/lzio.h
)

ADD_LIBRARY( Lua STATIC
    ${Lua_SOURCES} 
    ${Lua_HEADERS}     
)

CopyFilesTo(Lua "${Lua_HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/" ${PUBLICHEADER_DIR}/Lua/)

TARGET_LINK_LIBRARIES( Lua )