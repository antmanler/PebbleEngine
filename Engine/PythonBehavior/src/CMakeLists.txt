# If the pyx file is a C++ file, we should specify that here.

file(GLOB RR_CPYSRC "*.pyx")
file(GLOB RR_CPPSRC "*.cpp")
file(GLOB INC_PRI "../include/*.*")

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set_source_files_properties(
  ${RR_CPYSRC}
  PROPERTIES CYTHON_IS_CXX TRUE )


# Multi-file cython modules do not appear to be working at the moment.
cython_add_standalone_library( PythonBehavior cython_generated ${RR_CPYSRC} ${RR_CPPSRC} ${INC_PRI} )
target_link_libraries(PythonBehavior Engine)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(MSVC)
		add_custom_target( hack4win
				COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../hack4win/fart.exe
				#ARGS
				#-w
				#-V
				-q
				*.c,*.h,*.cpp
				"\"#include \\\"Python.h\\\"\""
				"\"#include \\\"dPython.h\\\"\""
				DEPENDS ${cython_generated}
				WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
				COMMENT "Changing the included python files."
		)
		add_dependencies(PythonBehavior hack4win)
	endif()
endif()
