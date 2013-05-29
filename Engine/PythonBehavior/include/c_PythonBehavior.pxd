from cpython.ref cimport PyObject
from Engine cimport c_Property, IBehavior, bool, uint32, const_char_ptr, ConstProp

cdef extern from "PythonBehavior.h" namespace "Pebble" :
    char gContentDir[128]
    ctypedef void proxy_init(void *self)
    ctypedef void proxy_release(void *self)
    ctypedef void proxy_start(void *self)
    ctypedef void proxy_update(void *self)
    ctypedef char* proxy_typeName(void *self)
    ctypedef bool proxy_processMessage(void *self, char *evt, char *params)
    #ctypedef char* proxy_saveToXml(void *self)
    ctypedef void proxy_loadFromXml(void *self, char *buf, uint32 size)
    ctypedef uint32 proxy_queryPropertyCount(void *self)
    ctypedef char* proxy_queryPropertyNameOf(void *self, uint32 idx)
    ctypedef c_Property proxy_getPropertyByIdx(void *self, uint32 idx)
    ctypedef c_Property proxy_getPropertyByName(void *self, char *name)
    ctypedef void proxy_setPropertyByIdx(void *self, uint32 idx, c_Property prop)
    ctypedef void proxy_setPropertyByName(void *self, char *name, c_Property prop)

    cdef cppclass PyBehavior:
        PyObject *pyobj
        proxy_init *init
        proxy_release *release
        proxy_start *start
        proxy_update *update
        proxy_typeName *typeName
        proxy_processMessage *processMessage
        #proxy_saveToXml *saveToXml
        proxy_loadFromXml *loadFromXml
        proxy_queryPropertyCount *queryPropertyCount
        proxy_queryPropertyNameOf *queryPropertyNameOf
        proxy_getPropertyByIdx *getPropertyByIdx
        proxy_getPropertyByName *getPropertyByName
        proxy_setPropertyByIdx *setPropertyByIdx
        proxy_setPropertyByName *setPropertyByName

    IF UNAME_SYSNAME == "Windows":
        cdef cppclass PyBehaviorWrapper :
            PyBehaviorWrapper(PyBehavior* proxy)
            PyBehaviorWrapper(IBehavior* behavior)
            void __cdecl start()
            void __cdecl update()
            const_char_ptr __cdecl typeName()
            bool __cdecl processMessage(const_char_ptr evt, const_char_ptr params)
            #const_char_ptr __cdecl saveToXml()
            void __cdecl loadFromXml(const_char_ptr contentbuf, uint32 size)
            uint32 __cdecl queryPropertyCount()
            const_char_ptr __cdecl queryPropertyNameOf(uint32 idx)
            c_Property __cdecl getProperty(const_char_ptr name)
            c_Property __cdecl getProperty(uint32 idx)
            void __cdecl setProperty(const_char_ptr name, ConstProp prop)
            void __cdecl setProperty(uint32 idx, ConstProp prop)
            # python side
            IBehavior* gocBehavior(char *type_name)
            bool removeBehavior(PyBehaviorWrapper *behavior)
            void sendMessage(char* evt_name, char* param)

    ELSE:
        cdef cppclass PyBehaviorWrapper :
            PyBehaviorWrapper(PyBehavior* proxy)
            PyBehaviorWrapper(IBehavior* behavior)
            void start()
            void update()
            const_char_ptr typeName()
            bool processMessage(const_char_ptr evt, const_char_ptr params)
            #const_char_ptr saveToXml()
            void loadFromXml(const_char_ptr contentbuf, uint32 size)
            uint32 queryPropertyCount()
            const_char_ptr queryPropertyNameOf(uint32 idx)
            c_Property getProperty(const_char_ptr name)
            c_Property getProperty(uint32 idx)
            void setProperty(const_char_ptr name, ConstProp prop)
            void setProperty(uint32 idx, ConstProp prop)
            # python side
            IBehavior* gocBehavior(char *type_name)
            bool removeBehavior(PyBehaviorWrapper *behavior)
            void sendMessage(char* evt_name, char* param)

    bool isPyBehaviorWrapper(IBehavior *bheavior)
    PyObject* castToPythonObject(PyBehaviorWrapper* wrapper)

