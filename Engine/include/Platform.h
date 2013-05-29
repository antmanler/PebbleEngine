
#pragma once

#if defined( _DEBUG ) || defined( DEBUG )
	#include <assert.h>
#define PEBBLE_DEBUG
#endif

// Detect platform
#if defined( WINCE )
#	if !defined( PLATFORM_WIN_CE )
#		define PLATFORM_WIN_CE
#	endif
#elif defined( WIN32 ) || defined( _WINDOWS )
// #	if !defined( PLATFORM_WIN_ES2 )
// #		define PLATFORM_WIN_ES2
// #	endif
#	if !defined( PLATFORM_WIN )
#		define PLATFORM_WIN
#	endif
//#	define PLATFORM_ES2
#elif defined( __APPLE__ ) || defined( __APPLE_CC__ )
#   include <TargetConditionals.h>
#   if (TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR)
#       if !defined(PLATFORM_IOS)
#			define PLATFORM_IOS
#		endif
#   if !defined( PLATFORM_ES2 )
#      define PLATFORM_ES2
#   endif
#   else
#		if !defined( PLATFORM_MAC )
#			define PLATFORM_MAC
#		endif
#   endif
#elif defined( ANDROID )
#   if !defined( PLATFORM_ANDROID )
#      define PLATFORM_ANDROID
#   endif
#   if !defined( PLATFORM_ES2 )
#      define PLATFORM_ES2
#   endif
#elif defined( NACL )
#   if !defined( PLATFORM_NACL )
#      define PLATFORM_NACL
#   endif
#   if !defined( PLATFORM_ES2 )
#      define PLATFORM_ES2
#   endif
#	include <stdlib.h>
#	include <string.h>
#elif defined( __QNX__ )
#   define PLATFORM_QNX
#   if !defined( PLATFORM_ES2 )
#      define PLATFORM_ES2
#   endif
#	include <stdlib.h>
#	include <string.h>
#else
#	if !defined( PLATFORM_LINUX )
#		define PLATFORM_LINUX
#	endif
#endif


#ifndef DLLEXP
#	ifdef PLATFORM_WIN
#		define DLLEXP extern "C" __declspec( dllexport )
#       ifdef ENGINE_DLL
#           define ENGINE_EXPORT __declspec( dllexport )
#	else
#           define ENGINE_EXPORT
#       endif
#	else
#		if defined( __GNUC__ ) && __GNUC__ >= 4
#		  define DLLEXP extern "C" __attribute__ ((visibility("default")))
#   	else
#		  define DLLEXP extern "C"
#   	endif
#       define ENGINE_EXPORT
#	endif
#endif


// Shortcuts for common types
typedef signed char int8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;


#if !defined( PLATFORM_WIN ) && !defined( PLATFORM_WIN_CE )
#	define _stricmp strcasecmp
#	define _mkdir( name ) mkdir( name, 0755 )
#endif

#ifdef PLATFORM_WIN_CE
#define GetProcessAffinityMask
#define SetThreadAffinityMask
#ifndef vsnprintf
#	define vsnprintf _vsnprintf
#endif
#undef ASSERT
#undef min
#undef max
#endif

#if !defined( _MSC_VER ) || (defined( _MSC_VER ) && (_MSC_VER < 1400))
#	define strncpy_s( dst, dstSize, src, count ) strncpy( dst, src, count < dstSize ? count : dstSize )
#endif
#if defined( _MSC_VER ) && (_MSC_VER < 1400)
#   define vsnprintf _vsnprintf
#endif


// Runtime assertion
#if defined( PEBBLE_DEBUG )
#	define ASSERT( exp ) assert( exp );
#else
#	define ASSERT( exp )
#endif

// Support for MSVC static code analysis
#if defined( _MSC_VER ) && defined( _PREFAST_ )
#	define ASSERT( exp ) __analysis_assume( exp );
#endif

// Static compile-time assertion
namespace StaticAssert
{
	template< bool > struct FAILED;
	template<> struct FAILED< true > { };
}
#define ASSERT_STATIC( exp ) (StaticAssert::FAILED< (exp) != 0 >())

#include "utMath.h"
#include "YapsInterfaces.h"

#define EGCALL YAPSCALL

#include "sigslot.h"

namespace Pebble {
// typedef std::string String;
//using namespace Gallant;
using namespace sigslot;
// using namespace cdiggins;
//typedef cdiggins::any Any;

}


