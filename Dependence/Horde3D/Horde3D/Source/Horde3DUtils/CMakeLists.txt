
include_directories(../Shared)
include_directories(../../Bindings/C++)


add_library(Horde3DUtils SHARED
	main.cpp
	../../Bindings/C++/Horde3DUtils.h
	)
	
target_link_libraries(Horde3DUtils Horde3D)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")	
else()
	
	install(TARGETS Horde3DUtils
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		)
endif()

CopyFilesTo(Horde3DUtils "../../Bindings/C++/Horde3DUtils.h" "${CMAKE_CURRENT_SOURCE_DIR}/" ${PUBLICHEADER_DIR}/Horde3D/)
