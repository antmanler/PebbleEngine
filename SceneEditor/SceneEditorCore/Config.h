#ifndef PebbleEditorCore_EXPORT_H
#define PebbleEditorCore_EXPORT_H

#if defined( WINCE )
#	if !defined( PLATFORM_WIN_CE )
#		define PLATFORM_WIN_CE
#	endif
#elif defined( WIN32 ) || defined( _WINDOWS )
#	if !defined( PLATFORM_WIN )
#		define PLATFORM_WIN
#	endif
#elif defined( __APPLE__ ) || defined( __APPLE_CC__ )
#   if !defined( PLATFORM_MAC )
#      define PLATFORM_MAC
#   endif
#else
#	if !defined( PLATFORM_LINUX )
#		define PLATFORM_LINUX
#	endif
#endif


#ifdef PebbleEditorCore_BUILT_AS_STATIC
#  define DLLEXP
#  define PEBBLEEDITORCORE_NO_EXPORT
#else
#  ifndef DLLEXP
#    ifdef PLATFORM_WIN
#      ifdef PebbleEditorCore_EXPORTS
          /* We are building this library */
#        define DLLEXP __declspec(dllexport)
#      else
          /* We are using this library */
#        define DLLEXP __declspec(dllimport)
#    endif
#    else
#        define DLLEXP
#    endif
#  endif

#  ifndef PEBBLEEDITORCORE_NO_EXPORT
#    define PEBBLEEDITORCORE_NO_EXPORT 
#  endif
#endif

#endif