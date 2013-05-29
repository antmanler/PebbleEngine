
project(scalefieldGen)

# Add some useful default arguments to the nvcc flags.  This is an example of how we use
# PASSED_FIRST_CONFIGURE.  Once you have configured, this variable is TRUE and following
# block of code will not be executed leaving you free to edit the values as much as you
# wish from the GUI or from ccmake.
if(NOT PASSED_FIRST_CONFIGURE)
  SPHSFGEN_AddFlagToNvcc("-use_fast_math")
  SPHSFGEN_AddFlagToNvcc("-gencode=arch=compute_13,code=\\\"sm_13,compute_13\\\"")
  
  if( APPLE )
    # Undef'ing __BLOCKS__ for OSX builds.  This is due to a name clash between OSX 10.6
    # C headers and CUDA headers
    set(flag "-U__BLOCKS__")
    list(FIND CUDA_NVCC_FLAGS ${flag} index)
    if(index EQUAL -1)
      list(APPEND CUDA_NVCC_FLAGS ${flag})
      set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} CACHE LIST "Semi-colon delimit multiple arguments." FORCE)
    endif()
  endif()
endif(NOT PASSED_FIRST_CONFIGURE)

# This passes a preprocessor definition to cl.exe when processing CUDA code.
if(USING_WINDOWS_CL)
  list(APPEND CUDA_NVCC_FLAGS --compiler-options /D_USE_MATH_DEFINES)
endif()

include_directories( ${CUDA_INCLUDE_DIRS} )
include_directories(${CUDA_SDK_ROOT_DIR}/common/inc)

# add_subdirectory(tb)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tb)

# Collect the sources files
file(GLOB RR_SRC "*.cpp")
file(GLOB RR_INC "*.h*")
file(GLOB RR_CUSRC "*.cu")
#file(GLOB RR_CUINC "*.cuh")

# add executable
SPHSFGEN_AddCudaLibrary(scalefieldGen 
${RR_CUSRC} 
${RR_SRC}
${RR_INC}
)

# append extra linkage
#target_link_libraries(scalefieldGen )