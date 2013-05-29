/**
 * @file  YapsHost.h
 * @author  antmanler (blog.antmanler.com)
 * @version 0.7
 *
 * @section LICENSE
 *
 * GNU LESSER GENERAL PUBLIC LICENSE
 * Version 3, 29 June 2007
 *
 * <http://www.gnu.org/copyleft/lesser.html>
 */
#ifndef FILE_PluginHostSide_h__
#define FILE_PluginHostSide_h__

#include <map>
#include <string>
#include <stack>
#include <queue>
#include <assert.h>

#include "YapsInterfaces.h"

#define YAPS_VERSION 0.41
#define YAPS_REIGSTER_FUNCTION_NAME "register_plugin"


#if defined PLATFORM_WIN
#   include <Windows.h>
#   ifdef _MSC_VER
#       define WIN32_LEAN_AND_MEAN
#   endif
#   define DYNLIB_HANDLE hInstance
#   define DYNLIB_LOAD( a ) LoadLibrary( a )
#   define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#   define DYNLIB_UNLOAD( a ) !FreeLibrary( a )
struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;
#elif defined PLATFORM_LINUX
#   include <dlfcn.h>
#   define DYNLIB_HANDLE void*
#   define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#   define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#   define DYNLIB_UNLOAD( a ) dlclose( a )
#elif defined PLATFORM_MAC
#   include <CoreFoundation/CoreFoundation.h>
#   include <dlfcn.h>
#    define DYNLIB_HANDLE void*
//#    define DYNLIB_LOAD( a ) mac_loadDylib( a )
#   define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#   define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#   define DYNLIB_UNLOAD( a ) dlclose( a )
#endif


namespace {
    
    const char gUnknown[] = "Unknown" ;
} // namespace

namespace Yaps {
    
    template<typename Interface>
    class Factory {
    public:
        typedef std::stack<Interface *> InstanceStack ;
        
        Factory(IFactory *factory) {
            this->factoy_ = factory ;
        }
        
        ~Factory() {
            while (!this->instances_.empty()) {
                (this->instances_.top())->Destroy() ;
                this->instances_.pop() ;
            }
            
            if (factoy_) {
                factoy_->Destroy(); ;
            }
        }
        
        Interface *Get() {
            Interface *inst = this->Create() ;
            if ( inst )  {
                this->instances_.push(inst) ;
                return inst ;
            }
            return NULL ;
        }
        
        Interface *Create() {
            if ( this->factoy_ )
                return static_cast<Interface *>(this->factoy_->Create()) ;
            return NULL ;
        }
        
        InstanceStack& instances() {return instances_;}
        
        virtual const char *GetFactoryName() {
            return this->factoy_ ? this->factoy_->GetFactoryName() : gUnknown ;
        }
        
        IFactory* ptr() {return factoy_;}
        
    private:
        IFactory *factoy_ ;
        InstanceStack instances_ ;
    };
    
    template <typename Interface>
    class Host : public IHost {
    public:
        
        typedef Factory<Interface> FactoryType ;
        
        /**
         Constructor, assigns name and version to the host
         */
        Host(const std::string& name,const int& version) :
        host_name_(name),plugin_version_(version) {}
        
        /**
         Destructor, auto deletes all loaded factories
         */
        virtual ~Host(void) {
            Destroy();
        }
        
        virtual void YAPSCALL Destroy() {
            typename FactoryMap::iterator it ;
            for (it = loaded_factories_.begin() ;
                 it != loaded_factories_.end(); ++it) {
                delete it->second ;
            }
            loaded_factories_.clear();
        }
        
        virtual int YAPSCALL GetPluginVersion() { return plugin_version_;}
        
        virtual const char* YAPSCALL GetHostName() {return host_name_.c_str();}
        
        virtual bool YAPSCALL HasFactoryOf(const char* name) {
            typename FactoryMap::iterator it =
            this->loaded_factories_.find(std::string(name));
            return it != loaded_factories_.end();
        }
        
        IFactory * YAPSCALL  AddFactory(IFactory* factory, const int& plugin_version ) {
            
            if ((factory != NULL) && (plugin_version == this->GetPluginVersion())) {
                this->loaded_factories_[factory->GetFactoryName()] = new FactoryType(factory);
                return factory;
            }
            return NULL;
        }
        
        IFactory* YAPSCALL RemoveFactory(IFactory* factory) {
            typename FactoryMap::iterator it =
            this->loaded_factories_.find(std::string(factory->GetFactoryName()));
            if (it != loaded_factories_.end()) {
                this->loaded_factories_.erase(it);
                return factory;
            }
            return NULL;
        }
        
        FactoryType *GetFactory(const char *factory_name) {
            typename FactoryMap::iterator it =
            this->loaded_factories_.find(std::string(factory_name));
            if (it != loaded_factories_.end()) return it->second;
            return NULL;
        }
        
        Interface *CreateInterfaceObject(const char *factory_name) {
            FactoryType *f = this->GetFactory(factory_name) ;
            if (f) return f->Create() ;
            return NULL ;
        }
        
        Interface *GetInterfaceObject(const char *factory_name) {
            FactoryType *f = this->GetFactory(factory_name) ;
            if (f) return f->Get() ;
            return NULL ;
        }
        
        /**
         Returns the number of factories loaded in host
         */
        size_t GetFactoryCount() {return loaded_factories_.size();}
        
        /**
         Files the factory_vector with all the factories of the host
         */
        void GetAllFactories(std::vector<FactoryType*>& factory_vector  )
        {
            typename FactoryMap::iterator it ;
            for (it = loaded_factories_.begin();
                 it != loaded_factories_.end(); ++it) {
                FactoryType* dr = it->second;
                factory_vector.push_back(dr);
            }
        }
        
    protected:
        std::string host_name_ ;
        int plugin_version_ ;
        
    private:
        typedef std::map<std::string, FactoryType*> FactoryMap;
        FactoryMap loaded_factories_ ;
        
    };
    
#if !defined(PLATFORM_IOS)
    template <typename ManagerType>
    class Plugin {
    public:
        Plugin(std::string filename, bool& is_load)
        :dll_handle_(0),
        dll_refrence_count_(0),
        register_plugin_functions_(0) {
            
            is_load = true;
            
            this->dll_handle_ = DYNLIB_LOAD(filename.c_str());
            if (this->dll_handle_) {
                try  {
                    register_plugin_functions_ = reinterpret_cast<RegisterPluginFunction *>(
                                                                                            DYNLIB_GETSYM(dll_handle_, YAPS_REIGSTER_FUNCTION_NAME));
                    
                    if(! register_plugin_functions_) {
                        is_load = false;
                        DYNLIB_UNLOAD(dll_handle_);
                    } else{
                        dll_refrence_count_ = new size_t(1);
                    }
                } catch(...)  {
                    DYNLIB_UNLOAD(dll_handle_);
                    is_load = false;
                }
            } else {
                is_load = false;
            }
        }
        
        
        /** Destroys the plugin, unloading its library when no more references
         to it exist.
         */
        ~Plugin()  {
            // Only unload the DLL if there are no more references to it
            if( dll_refrence_count_ && !--*dll_refrence_count_)  {
                delete dll_refrence_count_;
                DYNLIB_UNLOAD(dll_handle_);
            }
        }
        
        
        /** Creates a copy of a plugin that has already been loaded.
         Required to provide correct semantics for storing plugins in
         an STL map container.
         
         @param  Other  Other plugin instance to copy
         */
        Plugin(const Plugin &Other)
        :dll_handle_(Other.dll_handle_),
        dll_refrence_count_(Other.dll_refrence_count_),
        register_plugin_functions_(Other.register_plugin_functions_){
            ++*dll_refrence_count_;
        }
        
        /// Register the plugin to a kernel
        void RegisterPlugin(ManagerType* K) {
            if (register_plugin_functions_)
                register_plugin_functions_(K);
        }
        
    private:
        /// Signature for the plugin's registration function
        typedef void RegisterPluginFunction(ManagerType*);
        
        ///< Win32 DLL handle
        DYNLIB_HANDLE dll_handle_;
        ///< Number of references to the DLL
        size_t *dll_refrence_count_;
        ///< Plugin registration function
        RegisterPluginFunction   *register_plugin_functions_;
    };
#else
    struct PluginEntry
    {
        const char plugin_name[128];
        void* register_func;
    };
    struct PluginList
    {
        PluginEntry plugins[16];
        int plugin_count;
    };
    extern PluginList StaticImportedPlugins;
    
    /**
     * Because iOS does not allow dynamic load for apps, 
     * so here is a static importer for plugins
     */
    template <typename ManagerType>
    class Plugin {
    public:
        Plugin(std::string filename, bool& is_load)
        :register_plugin_functions_(0x0)
        {
            is_load = false;
            for (int i = 0; i < StaticImportedPlugins.plugin_count;  ++i) {
                if (filename == std::string(StaticImportedPlugins.plugins[i].plugin_name)) {
                    register_plugin_functions_ = reinterpret_cast<RegisterPluginFunction*>(StaticImportedPlugins.plugins[i].register_func);
                    is_load = true;
                    break;
                }
            }
        }
        
        /** Destroys the plugin, unloading its library when no more references
         to it exist.
         */
        ~Plugin()  {
        }
        
        /** Creates a copy of a plugin that has already been loaded.
         Required to provide correct semantics for storing plugins in
         an STL map container.
         
         @param  Other  Other plugin instance to copy
         */
        Plugin(const Plugin &Other)
        :register_plugin_functions_(Other.register_plugin_functions_)
        {
        }
        
        /// Register the plugin to a kernel
        void RegisterPlugin(ManagerType* K) {
            if (register_plugin_functions_) {
                register_plugin_functions_(K);
            }
        }
    private:
        /// Signature for the plugin's registration function
        typedef void RegisterPluginFunction(ManagerType*);
        ///< Plugin registration function
        RegisterPluginFunction   *register_plugin_functions_;
    };
#endif
    
    template <typename ManagerType>
    class Manager : public ManagerType
    {
    public:
        // inline do nothing
        Manager() {}
        
        virtual ~Manager(void) {
            Destroy();
        }
        
        /**
         * Destructor.
         Deletes all stored plugins.
         */
        virtual void Destroy()
        {
            for (HostMapIterator itr = loadedServers.begin(); itr != loadedServers.end(); ++itr) {
                itr->second->Destroy();
            }
            loadedServers.clear();
            
            for (PluginMapIterator it = loadedPlugins.begin(); it != loadedPlugins.end(); ++it) {
                delete it->second;
            }
            loadedPlugins.clear();
        }
        
        /**
         * Returns version of the pugg library
         */
        double YAPSCALL GetYapsVersion() {return YAPS_VERSION;}
        
        IHost* YAPSCALL AddHost( IHost* host )
        {
            std::string serverName = host->GetHostName() ;
            if(loadedServers.find(serverName) == loadedServers.end())
            {
                loadedServers[serverName] = host;
                return host;
            }
            else return NULL;
        }
        
        IHost* YAPSCALL GetHost( const char * serverName )
        {
            HostMapIterator it;
            it = loadedServers.find(std::string(serverName));
            if (it != loadedServers.end()) return it->second;
            else return NULL;
        }
        
        /**
         Tries to load a plugin from the given file name.
         @param file_name a string holding the full path and name to the plugin file
         @return true if the plugin is successfully loaded.
         Checks the following:
         -If the plugin from the file is already loaded
         -If the file has a void register_plugin(Yaps::IManager*) function
         If above coditions are plug-in and all of its drivers that passes the version
         control are auto loaded.
         
         */
        bool LoadPlugin(const char *fileName )
        {
            bool isOK = false;
            if (loadedPlugins.find(fileName) == loadedPlugins.end())
            {
                Plugin<ManagerType>* plugin = new Plugin<ManagerType>(fileName,isOK);
                if (isOK)
                {
                    loadedPlugins[fileName] = plugin;
                    plugin->RegisterPlugin(this);
                } else delete plugin;
            }
            return isOK;
        }
        
    protected:
        typedef std::map< std::string, Plugin<ManagerType>* > PluginMap;
        typedef typename PluginMap::iterator PluginMapIterator;
        
        typedef std::map<std::string,IHost*> HostMap;
        typedef HostMap::iterator HostMapIterator;
        
        PluginMap loadedPlugins;
        HostMap loadedServers;
    };
    
}

#endif
