from c_EngineBinding cimport *
#from libcpp import bool


cdef class PropertyBase:
    cdef bool __borrowed
    cdef char* name(self)
    cdef void from_cpropoerty(self, c_Property val)
    cdef c_Property get_property(self)
    cdef void set_property(self, c_Property val)

cdef class PropertyStatmentBase:
    cdef PropertyBase create_property(self)

cdef class FloatProperty(PropertyBase):
    cdef float* _value
cdef class PropertyFloat(PropertyStatmentBase) :
    cdef float _default

cdef class BoolProperty(PropertyBase):
    cdef bool* _value
cdef class PropertyBool(PropertyStatmentBase) :
    cdef bool _default

cdef class IntProperty(PropertyBase):
    cdef int* _value
cdef class PropertyInt(PropertyStatmentBase) :
    cdef int _default

cdef class Vec3Property(PropertyBase):
    cdef Vec3f* _value
cdef class PropertyVec3(PropertyStatmentBase) :
    cdef object _default

cdef class Vec4Property(PropertyBase):
    cdef Vec4f* _value
cdef class PropertyVec4(PropertyStatmentBase) :
    cdef object _default

cdef class StringProperty(PropertyBase):
    cdef char* _value
cdef class PropertyString(PropertyStatmentBase) :
    cdef object _default

