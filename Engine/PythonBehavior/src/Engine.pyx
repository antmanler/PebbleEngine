#from c_EngineBinding cimport *
from Engine cimport *

from libc.stdlib cimport malloc, free
from libc.stdio cimport sprintf
from libc.string cimport strlen
#from libcpp import bool

from cpython.unicode cimport *
import numpy as np

import json

'''
//TODO: fix from_cpropoerty, get_property, set_property
'''

# utils
cdef unicode tounicode(char* s):
    return s[0:len(s)].decode('UTF-8', 'strict')

cdef class PropertyBase:
    def __cinit__(self) :
        self.__borrowed = False
    cdef char* name(self) :
        # return the name of this property
        ret = getattr(self,'_name', 'Error').encode('UTF-8')
        cdef char* c_string = ret
        return c_string

    cdef void from_cpropoerty(self, c_Property val)  : raise NotImplementedError
    cdef c_Property get_property(self) : raise NotImplementedError
    cdef void set_property(self, c_Property val) : raise NotImplementedError
    def set_value(self, value) : raise NotImplementedError
    def get_value(self) : raise NotImplementedError

cdef class PropertyStatmentBase:
    cdef PropertyBase create_property(self) : raise NotImplementedError


# float property
cdef class FloatProperty(PropertyBase):
    def __cinit__(self):
        PropertyBase.__init__(self)
        self._value = <float*>malloc(sizeof(float))
        self._value[0] = 0.0
    def __dealloc__(self):
        if not self.__borrowed :
            free(self._value)

    cdef void from_cpropoerty(self, c_Property val) :
        if val.p_type == PT_FLOAT :
            free(self._value)
            self._value = NULL
            self._value = <float*>val.val
            self.__borrowed = True

    cdef c_Property get_property(self) :
        cdef c_Property ret
        ret.p_type = PT_FLOAT
        ret.val = <void*>self._value
        return ret

    cdef void set_property(self, c_Property val) :
        if val.p_type == PT_FLOAT :
            self._value[0] = (<float*>val.val)[0]

    def set_value(self, value) : 
        self._value[0] = <float>value
    def get_value(self) :
        return <float>self._value[0]

cdef class PropertyFloat(PropertyStatmentBase):
    def __cinit__(self, float default = 0.0) :
        PropertyStatmentBase.__init__(self)
        self._default = default

    cdef PropertyBase create_property(self):
        ret = FloatProperty()
        ret.set_value(self._default)
        return ret

# boolean property
cdef class BoolProperty(PropertyBase):
    def __cinit__(self):
        PropertyBase.__init__(self)
        self._value = <bool*>malloc(sizeof(bool))
        self._value[0] = 0
    def __dealloc__(self):
        if not self.__borrowed :
            free(self._value)    

    cdef void from_cpropoerty(self, c_Property val) :
        if val.p_type == PT_BOOL :
            free(self._value)
            self._value = NULL
            self._value = <bool*>val.val
            self.__borrowed = True

    cdef c_Property get_property(self) :
        cdef c_Property ret
        ret.p_type = PT_BOOL
        ret.val = <void*>self._value
        return ret

    cdef void set_property(self, c_Property val) :
        if val.p_type == PT_BOOL :
            self._value[0] = (<bool*>val.val)[0]

    def set_value(self, value) : 
        self._value[0] = <bool>value
    def get_value(self) :
        return <bool>self._value[0]

cdef class PropertyBool(PropertyStatmentBase):
    def __cinit__(self, bool default = False) :
        PropertyStatmentBase.__init__(self)
        self._default = default

    cdef PropertyBase create_property(self):
        ret = BoolProperty()
        ret.set_value(self._default)
        return ret

# integer property
cdef class IntProperty(PropertyBase):
    def __cinit__(self):
        PropertyBase.__init__(self)
        self._value = <int*>malloc(sizeof(int))
        self._value[0] = 0
    def __dealloc__(self):
        if not self.__borrowed :
            free(self._value)  
    
    cdef void from_cpropoerty(self, c_Property val) :
        if val.p_type == PT_INT32 :
            free(self._value)
            self._value = NULL
            self._value = <int*>val.val
            self.__borrowed = True
    
    cdef c_Property get_property(self) :
        cdef c_Property ret
        ret.p_type = PT_INT32
        ret.val = <void*>self._value
        return ret

    cdef void set_property(self, c_Property val) :
        if val.p_type == PT_INT32 :
            self._value[0] = (<int*>val.val)[0]

    def set_value(self, value) : 
        self._value[0] = <int>value
    def get_value(self) :
        return <int>self._value[0]

cdef class PropertyInt(PropertyStatmentBase):
    def __cinit__(self, int default = 0) :
        PropertyStatmentBase.__init__(self)
        self._default = default

    cdef PropertyBase create_property(self):
        ret = IntProperty()
        ret.set_value(self._default)
        return ret

# vector 3d
cdef class Vec3Property(PropertyBase):
    def __cinit__(self):
        PropertyBase.__init__(self)
        self._value = new Vec3f()
    def __dealloc__(self):
        if not self.__borrowed :
            del self._value
    
    cdef void from_cpropoerty(self, c_Property val) :
       if val.p_type == PT_VEC3F :
            del self._value
            self._value = NULL
            self._value = <Vec3f*>val.val
            self.__borrowed = True
    
    cdef c_Property get_property(self) :
        cdef c_Property ret
        ret.p_type = PT_VEC3F
        ret.val = <void*>self._value
        return ret

    cdef void set_property(self, c_Property val) :
        cdef Vec3f* v3
        if val.p_type == PT_VEC3F :
            v3 = <Vec3f*>val.val
            self._value.x = v3.x
            self._value.y = v3.y
            self._value.z = v3.z

    def set_value(self, value) : 
        self._value.x = value[0]
        self._value.y = value[1]
        self._value.z = value[2]
    def get_value(self) :
        vec3 = np.array([self._value.x, self._value.y, self._value.z])
        return vec3

cdef class PropertyVec3(PropertyStatmentBase):
    def __cinit__(self, default = None ) :
        PropertyStatmentBase.__init__(self)
        if default is None :
            default = [0.0, 0.0, 0.0]
        self._default = default

    cdef PropertyBase create_property(self):
        ret = Vec3Property()
        ret.set_value(self._default)
        return ret


# vector 4d
cdef class Vec4Property(PropertyBase):
    def __cinit__(self):
        PropertyBase.__init__(self)
        self._value = new Vec4f()
    def __dealloc__(self):
        if not self.__borrowed :
            del self._value

    cdef void from_cpropoerty(self, c_Property val) :
        if val.p_type == PT_VEC4F :
            del self._value
            self._value = NULL
            self._value = <Vec4f*>val.val
            self.__borrowed = True
    
    cdef c_Property get_property(self) :
        cdef c_Property ret
        ret.p_type = PT_VEC4F
        ret.val = <void*>self._value
        return ret

    cdef void set_property(self, c_Property val) :
        cdef Vec4f* v4
        if val.p_type == PT_VEC4F :
            v4 = <Vec4f*>val.val
            self._value.x = v4.x
            self._value.y = v4.y
            self._value.z = v4.z
            self._value.w = v4.w

    def set_value(self, value) : 
        self._value.x = value[0]
        self._value.y = value[1]
        self._value.z = value[2]
        self._value.w = value[3]
    def get_value(self) :
        vec4 = np.array([self._value.x, self._value.y, self._value.z, self._value.w])
        return vec4

cdef class PropertyVec4(PropertyStatmentBase):
    def __cinit__(self, default = None) :
        PropertyStatmentBase.__init__(self)
        if default is None :
            default = [0.0, 0.0, 0.0, 0.0]
        self._default = default

    cdef PropertyBase create_property(self):
        ret = Vec4Property()
        ret.set_value(self._default)
        return ret

# c string wrapper
cdef class StringProperty(PropertyBase):
    def __cinit__(self):
        PropertyBase.__init__(self)
        self._value = NULL
    def __dealloc__(self):
        if self._value != NULL : free(self._value)

    cdef void from_cpropoerty(self, c_Property val) :
        if val.p_type == PT_CSTRING :
            if self._value != NULL : free(self._value)
            self._value = <char*>malloc(strlen(<char*>(val.val))+1)
            sprintf(<char*>(self._value), "%s", <char*>(val.val))
    
    cdef c_Property get_property(self) :
        cdef c_Property ret
        ret.p_type = PT_CSTRING
        ret.val = <void*>self._value
        return ret

    cdef void set_property(self, c_Property val) :
        if val.p_type == PT_CSTRING :
            if self._value != NULL : free(self._value)
            self._value = <char*>malloc(strlen(<char*>(val.val))+1)
            sprintf(<char*>(self._value), "%s", <char*>(val.val))

    def set_value(self, value) : 
        cdef bytes ret = value.encode('UTF-8')
        cdef char* c_str = ret
        # allac new space
        if self._value == NULL :
            self._value = <char*>malloc(len(value)+1)
        elif len(self._value) < len(value) :
            if self._value != NULL : free(self._value)
            self._value = <char*>malloc(len(value)+1)
        sprintf(self._value, "%s", c_str)
    
    def get_value(self) :
        return tounicode(self._value)

cdef class PropertyString(PropertyStatmentBase):
    def __cinit__(self, default = None) :
        PropertyStatmentBase.__init__(self)
        if default is None : default = ""
        self._default = default

    cdef PropertyBase create_property(self):
        ret = StringProperty()
        ret.set_value(self._default)
        return ret

def broadcastMessage(name, *args, **kwargs) :
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
        #print cname, msg
        piwBroadcastMessage(cname, msg)
    except TypeError :
        # can not serialize to json, failed quietly
        return False


