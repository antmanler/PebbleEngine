cmake_minimum_required(VERSION 2.8)

include_directories(${PUBLICHEADER_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB RR_SRC "*.cpp")
file(GLOB INC_PRI "*.h*")

set(RR_SRC ${RR_SRC})
set(RR_INC ${INC_PUB})

source_group(include FILES ${INC_PRI})

add_library(dflib STATIC
	${RR_SRC}
	${RR_INC}
)

