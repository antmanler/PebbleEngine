
# This enforces a particular version of CMake that we require to process the script files
# properly.
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#
SET(project_policies
    CMP0001 # NEW: CMAKE_BACKWARDS_COMPATIBILITY should no longer be used.
    CMP0002 # NEW: Logical target names must be globally unique.
    CMP0003 # NEW: Libraries linked via full path no longer produce linker search paths.
    CMP0004 # NEW: Libraries linked may NOT have leading or trailing whitespace.
    CMP0005 # NEW: Preprocessor definition values are now escaped automatically.
    CMP0006 # NEW: Installing MACOSX_BUNDLE targets requires a BUNDLE DESTINATION.
    CMP0007 # NEW: List command no longer ignores empty elements.
    CMP0008 # NEW: Libraries linked by full-path must have a valid library file name.
    CMP0009 # NEW: FILE GLOB_RECURSE calls should not follow symlinks by default.
    CMP0010 # NEW: Bad variable reference syntax is an error.
    CMP0011 # NEW: Included scripts do automatic cmake_policy PUSH and POP.
    CMP0012 # NEW: if() recognizes numbers and boolean constants.
    CMP0013 # NEW: Duplicate binary directories are not allowed.
    CMP0014 # NEW: Input directories must have CMakeLists.txt
    )
FOREACH(policy ${project_policies})
    IF(POLICY ${policy})
        CMAKE_POLICY(SET ${policy} NEW)
    ENDIF()
ENDFOREACH()

project(SphScalefieldGen)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/WinMS)
    else()
        set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/Win)
    endif()
else()
    set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME})
endif()

if(DEFINED CMAKE_CONFIGURATION_TYPES)
else()
    set(BINARY_OUT_DIR ${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME}/Bin)
endif()
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR} CACHE STRING "Where binaries and .dll files go" FORCE)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR} CACHE STRING "Where binaries and .dll files go" FORCE)

# Add paths to our CMake code to the module path, so they can be found automatically by
# CMake.
set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/CMakeFiles"
  ${CMAKE_MODULE_PATH}
  )

# Tells CMake to build all the libraries as shared libraries by default.  This can be
# overrided by individual libraries later.
set(BUILD_SHARED_LIBS ON)

# Turn off the warning that NVCC issues when generating PTX from our CUDA samples.  This
# is a custom extension to the FindCUDA code distributed by CMake.
OPTION(CUDA_REMOVE_GLOBAL_MEMORY_SPACE_WARNING "Suppress the 'Advisory: Cannot tell what pointer points to, assuming global memory space' warning nvcc makes." ON)
# Default to building 32 bit PTX.  This can still be changed from the GUI later on.
option(CUDA_64_BIT_DEVICE_CODE "Compile device code in 64 bit mode" OFF)

# Find at least a 4.1 version of CUDA.
find_package(CUDA 4.1 REQUIRED)

# Present the CUDA_64_BIT_DEVICE_CODE on the default set of options.
mark_as_advanced(CLEAR CUDA_64_BIT_DEVICE_CODE)

function(SPHSFGEN_AddCudaLibrary target_name)

  # These calls will group CUDA files into their own directories in the Visual
  # Studio projects.
  source_group("CUDA Files" REGULAR_EXPRESSION ".+\\.cu$")

  # Separate the sources from the CMake and CUDA options fed to the macro.  This code
  # comes from the CUDA_COMPILE_PTX macro found in FindCUDA.cmake.  We are copying the
  # code here, so that we can use our own name for the target.  target_name is used in the
  # creation of the output file names, and we want this to be unique for each target in
  # the SDK.
  CUDA_GET_SOURCES_AND_OPTIONS(source_files cmake_options options ${ARGN})

  # Create the rules to build the PTX from the CUDA files.
  #CUDA_WRAP_SRCS( ${target_name} PTX generated_files ${source_files} ${cmake_options}
  #  OPTIONS ${options} )

  # Here is where we create the rule to make the executable.  We define a target name and
  # list all the source files used to create the target.  In addition we also pass along
  # the cmake_options parsed out of the arguments.
  CUDA_ADD_LIBRARY(${target_name}
    ${source_files}
    #${generated_files}
    ${cmake_options}
    )

  # Most of the samples link against the sutil library and the optix library.  Here is the
  # rule that specifies this linkage.
  # target_link_libraries( ${target_name})
endfunction()

function(SPHSFGEN_AddFlagToNvcc flag)
  list(FIND CUDA_NVCC_FLAGS ${flag} index)
  if(index EQUAL -1)
    list(APPEND CUDA_NVCC_FLAGS ${flag})
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} CACHE LIST "Semi-colon delimit multiple arguments." FORCE)
  endif()
endfunction()

# add core
add_subdirectory(scalefieldGen)

# Now that everything is done, indicate that we have finished configuring at least once.
# We use this variable to set certain defaults only on the first pass, so that we don't
# continually set them over and over again.
set(PASSED_FIRST_CONFIGURE ON CACHE INTERNAL "Already Configured once?")
