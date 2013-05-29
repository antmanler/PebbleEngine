from cpython.ref cimport PyObject
from libcpp cimport bool

ctypedef unsigned int uint32

cdef extern from "Engine/Engine.h" :
    ctypedef char const_char "const char"
    void piwBroadcastMessage(const_char *evt_name, const_char *param)

cdef extern from "Engine/utMath.h" namespace "Pebble" :
    #ctypedef any Any

    cdef cppclass Vec3f:
        float x, y, z
        Vec3f()
        Vec3f(float, float, float)

    cdef cppclass Vec4f:
        float x, y, z, w
        Vec4f()
        Vec4f(float, float, float, float)

    

cdef extern from "Engine/EngineSDK.h" namespace "Pebble::Property":

    cdef enum en_list :
        PT_UNKNOWN = 0,
        PT_BOOL,
        PT_INT32,
        PT_FLOAT,
        PT_VEC2F,
        PT_VEC3F,
        PT_VEC4F,
        PT_CSTRING,
        PT_COMPONENT,
        PT_CUSTORM_PRT

cdef extern from "Engine/EngineSDK.h" namespace "Pebble":
    cdef struct c_Property "Pebble::Property" :
        en_list p_type
        void* val

    ctypedef char* const_char_ptr " const char*"
    ctypedef c_Property ConstProp " const Property&"
    #ctypedef c_Property ConstProp



    IF UNAME_SYSNAME == "Windows":
        cdef cppclass IEntity :
            const_char_ptr __cdecl name()
            uint32 __cdecl id()
            void __cdecl sendEvent(const_char_ptr evt_name, void* param)

        cdef cppclass IBehavior :
            void __cdecl start()
            void __cdecl update()
            const_char_ptr __cdecl typeName()
            bool __cdecl processMessage(const_char_ptr evt, const_char_ptr params)
            const_char_ptr __cdecl saveToXml()
            void __cdecl loadFromXml(const_char_ptr contentbuf, uint32 size)
            uint32 __cdecl queryPropertyCount() 
            const_char_ptr __cdecl queryPropertyNameOf(uint32 idx)
            c_Property __cdecl getProperty(const_char_ptr name) 
            c_Property __cdecl getProperty(uint32 idx) 
            void __cdecl setProperty(const_char_ptr name, ConstProp prop)
            void __cdecl setProperty(uint32 idx, ConstProp prop)

    ELSE:
        cdef cppclass IEntity :
            const_char_ptr name()
            uint32 id()
            void sendEvent(const_char_ptr evt_name, void* param)

        cdef cppclass IBehavior :
            void start()
            void update()
            const_char_ptr typeName()
            bool processMessage(const_char_ptr evt, const_char_ptr params)
            const_char_ptr saveToXml()
            void loadFromXml(const_char_ptr contentbuf, uint32 size)
            uint32 queryPropertyCount() 
            const_char_ptr queryPropertyNameOf(uint32 idx)
            c_Property getProperty(const_char_ptr name) 
            c_Property getProperty(uint32 idx)
            void setProperty(const_char_ptr name, ConstProp prop)
            void setProperty(uint32 idx, ConstProp prop)
        

