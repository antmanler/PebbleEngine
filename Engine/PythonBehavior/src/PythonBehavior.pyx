cimport cpython.ref as cpy_ref
#from cpython.ref cimport Py_XDECREF
cimport cpython.module as mod
from Engine cimport *
from c_PythonBehavior cimport *
from PythonBehavior cimport *
from cpython.unicode cimport *
import inspect, json

# utils
cdef unicode tounicode(char* s):
    return s[0:len(s)].decode('UTF-8', 'strict')

# Base class for c side and py side
cdef class PythonBehaviorBase:
    def init(self) : pass
    def release(self) : pass
    def start(self) : pass
    def update(self) : pass
    #def saveToXml(self) : pass
    def loadFromXml(self, buf) : pass
    def typeName(self) :
        # default return the class name
        return unicode(self._piw_typeName)


# Python side scirpt inherit from this class
cdef c_Property __default_property
__default_property.p_type = PT_UNKNOWN
__default_property.val = NULL
cdef class CythonBehavior(PythonBehaviorBase):
    def __cinit__(self):
        PythonBehaviorBase.__init__(self)
        cdef PyBehavior* proxy = make_pywrapper(self)
        self._pybehavior = new PyBehaviorWrapper(proxy)

    cpdef PythonBehaviorBase gocBehavior(self, name) :
        '''
        Get or create a behavior specified by type name
        '''
        pyname = name.encode('UTF-8')
        cdef char* cname = pyname
        cdef IBehavior* behavior
        behavior = self._pybehavior.gocBehavior(cname)
        if behavior == NULL : return None
        if isPyBehaviorWrapper(behavior) :
            return <CythonBehavior>castToPythonObject(<PyBehaviorWrapper*>behavior)
        return create_cpp_wrapper(behavior)

    cpdef bool removeBehavior(self, behavior) :
        cdef PythonBehaviorBase pybhv
        if not isinstance(behavior, PythonBehaviorBase) : return False
        pybhv = <PythonBehaviorBase>behavior
        return self._pybehavior.removeBehavior(pybhv._pybehavior)

    def sendMessage(self, name, *args, **kwargs) :
        '''
        TODO:
        '''
        cdef char* cname
        cdef char* msg
        params = dict(argv=args, kwargv=kwargs)
        try:
            data = json.dumps(params)
            pyname = name.encode('UTF-8')
            cname = pyname
            pymsg = data.encode('UTF-8')
            msg = pymsg
            self._pybehavior.sendMessage(cname, msg)
        except TypeError :
            # can not serialize to json, failed quietly
            return False

    # c functions make a bridage
    cdef char* _typeName(self) :
        ret = self.typeName().encode('UTF-8')
        cdef char* c_string = ret
        return c_string
    
    #cdef char* _saveToXml(self):
        #ret = self.saveToXml().encode('UTF-8')
        #cdef char* c_string = ret
        #return c_string

    cdef void _loadFromXml(self, char *buf, uint32 size):
        pystr = buf[0:size].decode('UTF-8', 'strict')
        self.loadFromXml(pystr)

    cdef bool _processMessage(self, char *evt, char *params):
        # dispatch event to event handlers
        handler = getattr(self, tounicode(evt), None)
        #print evt, params
        # its not a valid message handler
        if handler is None or not callable(handler) : return True
        try :
            data = json.loads(tounicode(params))
            argv = data.get('argv',[])
            kwargv = data.get('kwargv',{})
            acount = len(argv)+len(kwargv)
            # insepct the handler and make a safe call
            args, varargs, keywords, defaults = inspect.getargspec(handler)
            defaults = 0 if defaults is None else len(defaults)
            if len(args)-1 == acount or len(args)-1-defaults <= acount or (varargs and keywords):
                ret = handler(*argv, **kwargv)
                if ret == False : return False
        except ValueError:
            # its not a json value
            return True
        return True


    ##### Handle for property system:
    cdef uint32 _queryPropertyCount(self) :
        # get propertes' count
        return len(self.__class__.__dict__['_piw_props'])

    cdef char* _queryPropertyNameOf(self, uint32 idx):
        # return property name indicated by index(idx)
        name = ""
        if idx >= 0 and idx < len(self.__class__.__dict__['_piw_props']) :
            name = self.__class__.__dict__['_piw_props'].keys()[idx]
        cdef char* c_string = name
        return c_string

    cdef c_Property _getPropertyByIdx(self, uint32 idx):
        cdef PropertyBase cprop
        if idx >= 0 and idx < len(self.__class__.__dict__['_piw_props']) :
            # get property
            name = self.__class__.__dict__['_piw_props'].keys()[idx]
            cprop = <PropertyBase>getattr(self, "_piw_prop_{}".format(name))
            return cprop.get_property()
        else :
            return __default_property

    cdef c_Property _getPropertyByName(self, char *name):
        cdef PropertyBase cprop
        pystr = tounicode(name)
        pyprop = getattr(self, "_piw_prop_{}".format(pystr), None)
        if pyprop is not None :
            cprop = <PropertyBase>pyprop
            return cprop.get_property()
        else :
            return __default_property

    cdef void _setPropertyByIdx(self, uint32 idx, c_Property prop):
        cdef PropertyBase cprop
        if idx >= 0 and idx < len(self.__class__.__dict__['_piw_props']) :
            # get property
            name = self.__class__.__dict__['_piw_props'].keys()[idx]
            cprop = <PropertyBase>getattr(self, "_piw_prop_{}".format(name))
            cprop.set_property(prop)

    cdef void _setPropertyByName(self, char *name, c_Property prop):
        cdef PropertyBase cprop
        pystr = tounicode(name)
        pyprop = getattr(self, "_piw_prop_{}".format(pystr), None)
        if pyprop is not None :
             cprop = <PropertyBase>pyprop
             cprop.set_property(prop)


class PyBehaviorMeta(type):
    '''
    Meta class for building python behavior
    '''
    def __new__(cls, name, bases, dct):
        # closure for making properties
        def make_prop(pname) :
            def getter(this):
                return getattr(this, pname).get_value()
            def setter(this, value):
                return getattr(this, pname).set_value(value)
            return property(getter, setter)

        props = dct.get('_piw_props', {})
        # collect properties
        for name, prop in dct.items() :
            if isinstance(prop, PropertyStatmentBase) and not name.startswith('_') :
                props[name] = prop
                # pop from class attribute
                # dct.pop(name)
                # replace it with a property
                dct[name] = make_prop("_piw_prop_{}".format(name))
        dct['_piw_props'] = props
        #dct['_piw_typeName'] = name
        # remove the props
        return super(PyBehaviorMeta, cls).__new__(cls, name, bases, dct)

class PythonBehavior(CythonBehavior):
    '''
    Base class for every python behavior
    '''
    __metaclass__ = PyBehaviorMeta
    def __init__(self) :
        # add properties
        cdef PropertyStatmentBase p
        CythonBehavior.__init__(self)
        for name, prop in self.__class__.__dict__['_piw_props'].items() :
            p = prop
            setattr(self, "_piw_prop_{}".format(name), p.create_property())


## create a Behavior object from python side
cdef public api IBehavior* create_behavior_from_code(char* name, object code) :
    # behavior instance
    cdef CythonBehavior behavior
    # convert to pyuincode
    mod_name = tounicode(name)
    # get or import module
    module = sys.modules.get(mod_name, None)
    if module is None:
        #print "import ", mod_name
        module = mod.PyImport_ExecCodeModule(name, code)
        #cpy_ref.Py_XDECREF(<cpy_ref.PyObject*>module)
        #module = sys.modules.get(mod_name, None)
    try :
        Behavior = getattr(module, name)
        behavior = Behavior()
        setattr(behavior, '_piw_typeName', tounicode(name))
        return <IBehavior*>behavior._pybehavior
    except AttributeError:
        return <IBehavior*>0x0


cdef public api void release_pythonbehavior_module(char* name) :
    # convert to pyuincode
    mod_name = tounicode(name)
    if sys.modules.has_key(mod_name) :
        #print "pop ", mod_name
        sys.modules.pop(mod_name)


## wrap cpp side behavior
cdef class CppBehavior(PythonBehaviorBase):
    def __cint__(self):
        PythonBehaviorBase.__init__(self)
        self._pybehavior = NULL

    def __init__(self) :
        # add properties
        cdef PropertyStatmentBase p
        CythonBehavior.__init__(self)
        for name, prop in self.__class__.__dict__['_piw_props'].items() :
            p = prop
            setattr(self, "_piw_prop_{}".format(name), p.create_property())

    cdef void _set_cppbehavior(self, IBehavior* behavior) :
        if behavior != NULL :
            self._pybehavior = new PyBehaviorWrapper(behavior)

def __create_dummy_class() :
    # a dummy class for modification
    class Dummy(CppBehavior) : pass
    return Dummy

# closure for making properties
def __make_prop(name) :
    def getter(this):
        # get cpp stuff
        cdef PythonBehaviorBase pyb
        cdef PropertyBase prop
        cdef char *cname
        cdef c_Property cprop = __default_property
        # convert to c string
        temp = name.encode('UTF-8')
        cname = temp
        # get c_property
        pyb = <PythonBehaviorBase>this
        cprop = pyb._pybehavior.getProperty(cname)
        # update py_property
        pyprop = getattr(this, "_piw_prop_{}".format(name))
        prop = <PropertyBase>pyprop
        prop.set_property(cprop)
        # convert to python value
        return pyprop.get_value()
    def setter(this, value):
        # get cpp stuff
        cdef PythonBehaviorBase pyb
        cdef PropertyBase prop
        cdef char *cname
        cdef c_Property cprop = __default_property
        # update py_property
        pyprop = getattr(this, "_piw_prop_{}".format(name))
        pyprop.set_value(value)
        # get c_property
        prop = <PropertyBase>pyprop
        cprop = prop.get_property()
        # set property
        pyb = <PythonBehaviorBase>this
        # convert to c string
        temp = name.encode('UTF-8')
        cname = temp
        pyb._pybehavior.setProperty(cname, cprop)

    return property(getter, setter)

cdef CppBehavior create_cpp_wrapper(IBehavior* behavior) :

    cdef CppBehavior cppbehavior
    cdef c_Property prop
    cdef PropertyStatmentBase prop_stat
    cdef int p_count
    cdef int i
    cdef const_char_ptr p_name
    cdef char* cname
    
    # create a dummy class
    cls = __create_dummy_class()
    props = {}
    p_count = behavior.queryPropertyCount()
    for i in range(p_count) :
        p_name = behavior.queryPropertyNameOf(i)
        c_prop = behavior.getProperty(p_name)
        # create a right kind of property statement
        if c_prop.p_type == PT_FLOAT :
            prop_stat = PropertyFloat()
        elif c_prop.p_type == PT_BOOL :
            prop_stat = PropertyBool()
        elif c_prop.p_type == PT_INT32 :
            prop_stat = PropertyInt()
        elif c_prop.p_type == PT_VEC3F :
            prop_stat = PropertyVec3()
        elif c_prop.p_type == PT_VEC4F :
            prop_stat = PropertyVec4()
        elif c_prop.p_type == PT_CSTRING :
            prop_stat = PropertyString()
        else : continue
        # update properties dictionary
        pyname = tounicode(<char*>p_name)
        props.setdefault(pyname, prop_stat)
        # add a new property
        setattr(cls, pyname, __make_prop(pyname))
    setattr(cls, '_piw_props', props)
    setattr(cls, '_piw_typeName', tounicode(<char*>behavior.typeName()))

    # create a instance
    inst = cls()
    # setup cpp behavior
    cppbehavior = <CppBehavior>inst
    cppbehavior._set_cppbehavior(behavior)
    return cppbehavior


###### proxy functions
cdef void __init(void *self) :
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    pyobj.init()

cdef void __release(void *self) :
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    pyobj.release()

cdef void __start(void *self):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    pyobj.start()

cdef void __update(void *self):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    pyobj.update()

cdef char* __typeName(void *self):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    return pyobj._typeName()

cdef bool __processMessage(void *self, char *evt, char *params):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    return pyobj._processMessage(evt, params)

#cdef char* __saveToXml(void *self):
    #cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    #return pyobj._saveToXml()

cdef void __loadFromXml(void *self, char *buf, uint32 size):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    pyobj._loadFromXml(buf, size)

cdef uint32 __queryPropertyCount(void *self):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    return pyobj._queryPropertyCount()

cdef char* __queryPropertyNameOf(void *self, uint32 idx):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    return pyobj._queryPropertyNameOf(idx)

cdef c_Property __getPropertyByIdx(void *self, uint32 idx):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    return pyobj._getPropertyByIdx(idx)

cdef c_Property __getPropertyByName(void *self, char *name):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    return pyobj._getPropertyByName(name)

cdef void __setPropertyByIdx(void *self, uint32 idx, c_Property prop):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    pyobj._setPropertyByIdx(idx, prop)

cdef void __setPropertyByName(void *self, char *name, c_Property prop):
    cdef CythonBehavior pyobj = <PythonBehaviorBase>(self)
    pyobj._setPropertyByName(name, prop)

###### wrapper function
cdef PyBehavior* make_pywrapper(CythonBehavior inst) :
    cdef PyBehavior* ret = new PyBehavior()
    ret.pyobj = <cpy_ref.PyObject*>inst
    ret.init = <proxy_init*>__init
    ret.release = <proxy_release*>__release
    ret.start = <proxy_start*>__start
    ret.update = <proxy_update*>__update
    ret.typeName = <proxy_typeName*>__typeName
    ret.processMessage = <proxy_processMessage*>__processMessage
    #ret.saveToXml = <proxy_saveToXml*>__saveToXml
    ret.loadFromXml = <proxy_loadFromXml*>__loadFromXml
    ret.queryPropertyCount = <proxy_queryPropertyCount*>__queryPropertyCount
    ret.queryPropertyNameOf = <proxy_queryPropertyNameOf*>__queryPropertyNameOf
    ret.getPropertyByIdx = <proxy_getPropertyByIdx*>__getPropertyByIdx
    ret.getPropertyByName = <proxy_getPropertyByName*>__getPropertyByName
    ret.setPropertyByIdx = <proxy_setPropertyByIdx*>__setPropertyByIdx
    ret.setPropertyByName = <proxy_setPropertyByName*>__setPropertyByName
    return ret

###### init module
ScriptDir = "{}/../Scripts".format(tounicode(gContentDir))
import sys
import os

base = os.path.abspath(ScriptDir)
if sys.platform == 'win32':
    site_packages = os.path.join(base, 'Lib', 'site-packages')
else:
    site_packages = os.path.join(base, 'lib', 'python%s' % sys.version[:3], 'site-packages')
prev_sys_path = list(sys.path)
import site
site.addsitedir(site_packages)
sys.real_prefix = sys.prefix
sys.prefix = base
# Move the added items to the front of the path:
new_sys_path = []
for item in list(sys.path):
    if item not in prev_sys_path:
        new_sys_path.append(item)
        sys.path.remove(item)
new_sys_path.append(ScriptDir)
sys.path[:0] = new_sys_path

