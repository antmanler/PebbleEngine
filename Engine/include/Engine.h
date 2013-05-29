
#pragma once

#ifndef DLL
#	if defined( WIN32 ) || defined( _WINDOWS )
#		define DLL extern "C" __declspec( dllimport )
#	else
#  if defined( __GNUC__ ) && __GNUC__ >= 4
#   define DLL extern "C" __attribute__ ((visibility("default")))
#  else
#		define DLL extern "C"
#  endif
#	endif
#endif

DLL bool piwInit(const char* buf, int sz);

DLL void piwParseSysArgv(int argc, char** argv);

DLL bool piwLoadPlugin(const char* filename);

DLL void piwRelease();

DLL void piwUpdate();

DLL void piwRender();

DLL void piwResize(unsigned int w, unsigned int h);

DLL bool piwLoadScene(const char* buf, unsigned int size);

DLL bool piwutLoadSceneFromFile(const char* filename);

DLL bool piwutDumpMessages();

/**
  * \brief Broadcast message to the entities in 'world'
  *
  * This is a expensive method, that will send message for each entities' components
  *
  * @param evt_name name of the event
  * @param param parameters encoded in JSON
  */
DLL void piwBroadcastMessage(const char* evt_name, const char* param);





