/**
* @file  YapsInterfaces.h
* @author  antmanler (blog.antmanler.com)
* @version 0.6
*
* @section LICENSE
*
* GNU LESSER GENERAL PUBLIC LICENSE
* Version 3, 29 June 2007
*
* <http://www.gnu.org/copyleft/lesser.html>
*/

/*! \mainpage Yaps version Documentation
*
* \section intro_sec Introduction
*
* Yaps(Yet Another Plugin System) 
* is a light weight c++ framework for plugin management.
* 
* - Features
*   - Only two-header file library, no installation
*   - Corss compiler in windows
*   - Auto loading of plugins from files
*   - Auto Version control
*   - Object oriented design
*
* - Guide lines
*   - All interface classes should be completely abstract. Every method should be pure virtual. 
*     (Or inline... you could safely write inline convenience methods that call other methods.) 
*   - All global functions should be extern "C" to prevent incompatible name mangling. 
*   - Don't use the standard C++ library. 
*   - Don't use exception handling. 
*   - Don't use virtual destructors. Instead, create a Destroy() method and 
*     an overloaded operator delete that calls Destroy(). 
*   - Don't allocate memory on one side of the DLL boundary and free it on the other. 
*     Different DLLs and executables can be built with different heaps, 
*     and using different heaps to allocate and free chunks of memory is a sure recipe for a crash. 
*     For example, don't inline your memory allocation functions so that they could be built 
*     differently in the executable and DLL. 
*   - Don't use overloaded methods in your interface. Different compilers order them within the 
*     vtable differently. 
*
* - Credits
*   - Chad Austin, Binary-compatible C++ Interfaces <http://chadaustin.me/cppinterface.html>
*/

#ifndef FILE_YAPSINTERFACE_H
#define FILE_YAPSINTERFACE_H

/**
    Important: In msvc6 please enable the RTTI support when compiling both
    host and plugin.
    From  menu->project->setting->c/c++->c++ program
    make sure the "Enable Run-Time Type Information (RTTI)" is checked.
*/

#ifndef NULL
#define NULL 0
#endif

#if defined ( WIN32 ) || defined( _WINDOWS )
#   define PLATFORM_WIN
#   ifdef _MSC_VER
#       define YAPSAPI __declspec(dllexport)
#       define YAPSCALL __cdecl
#       if _MSC_VER == 1200
#           pragma warning(disable:4541)
#       endif
#   else
#       define YAPSAPI
#       define YAPSCALL __cdecl
#   endif
#elif defined( __APPLE__ ) || defined( __APPLE_CC__ )
#   define PLATFORM_MAC
#   define YAPSAPI
#   define YAPSCALL
#else
#   define PLATFORM_LINUX
#   define YAPSAPI
#   define YAPSCALL
#endif

#ifdef __cplusplus
#   define EXTERN_C  extern "C"
#else
#   define EXTERN_C
#endif // __cplusplus

namespace Yaps
{

/**
  Base class for all exported interfaces.
  Host program must inherit this class.
  */
class IExportInterface ;

/**
        IFactory create a instance of interface out side of plugin.
    */
class IFactory ;

/**
        Base class for host class
        Required for storage of different types of hosts
  */
class IHost ;

/**
        Manager the pulgins
    */
class IManager ;

class IExportInterface {
public:
    /**
   Important method that release the instance created by this kind of factory
            Invoke the destructor or simply "delete this".
            For example:
            class SomConcreteClass: public ImplementInterface<SomeInterface> {
                void SomeConcreteClass::Destroy() {
                    delete(this) ;
                }
            }
            or a custom destroy :
            void SomeConcreteClass::Destroy() {
                {...} // do something else
                ::operator delete(this) ; //!important: invoke the global delete operator
            }
            
  */
    virtual void YAPSCALL Destroy() = 0 ;

    /**
            Make delete operator invokable outside of plugin
        */
    /*void operator delete(void* p) {
        if (p) {
            IExportInterface* inst = static_cast<IExportInterface*>(p);
            inst->Destroy();
        }
    }*/
};

class IFactory : public IExportInterface {
public:
    virtual IExportInterface * YAPSCALL Create() = 0 ;
    virtual const char * YAPSCALL GetFactoryName() = 0 ;
};

class IHost {
public:
    virtual void YAPSCALL Destroy() = 0;
    /**
   Returns the plugin version
  */
    virtual int YAPSCALL GetPluginVersion() = 0 ;
    /**
   Returns the host name
  */
    virtual const char * YAPSCALL GetHostName() = 0 ;

    virtual bool YAPSCALL HasFactoryOf(const char* name) = 0;

    /**
   *	Attamps to add a factory to the host.
   Host versions must match.
   Return NULL other wise
   */
    virtual IFactory* YAPSCALL AddFactory(IFactory* factory, const int& plugin_version) = 0 ;

    /**
      * \brief remove factory from host
      * \note this dose not release the factory
      * @return NULL if the factory not found, otherwise the factory being removed
      */
    virtual IFactory* YAPSCALL RemoveFactory(IFactory* factory) = 0;

};

class IManager {
public:
    /**
            Return the current version of Yaps
            make sure your plugin is supported by this version of Yaps' interface.
        */
    virtual double YAPSCALL  GetYapsVersion() = 0 ;

    /**
   Adds the given host to the system.
   Returns the given IHost pointer, you can use CastToIHost to cast it into
   the specific type of your own host
   */
    virtual IHost* YAPSCALL  AddHost( IHost* host ) = 0 ;

    /**
   Gets the pointer for the IHost matching the given name
   returns null, if no match is found.
   */
    virtual IHost* YAPSCALL GetHost( const char * host_name) = 0 ;

};

/**
        Utility template for convenience
    */
template<typename Interface>
class Implement: public Interface {
public:
    void operator delete(void* p) {
        ::operator delete(p);
    }
};

/**
        Utility template for making a default factory
    */
template<typename ConcreteClass>
class FactoryExport : public Implement<IFactory> {
public:
    FactoryExport(const char *factory_name):factory_name_(factory_name){
        //            int namelen = 0 ;
        //            while (factory_name[namelen] != '\0') {
        //                namelen ++ ;
        //            }
        //            this->factory_name_ = new char[++namelen] ;
        //            while (namelen > 0) {
        //                factory_name_[namelen] = factory_name[--namelen] ;
        //            }
    }

    virtual void YAPSCALL Destroy() {
        //            delete []factory_name_ ;
        delete this ;
    }

    IExportInterface * YAPSCALL Create() {
        return new ConcreteClass() ;
    }

    virtual const char * YAPSCALL  GetFactoryName() {
        return this->factory_name_ ;
    }
private:
    const char *factory_name_ ;
};

} // namespace Yaps

#endif //FILE_YAPSINTERFACE_H
