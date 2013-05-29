
from cpython.ref cimport PyObject

from c_PythonBehavior cimport PyBehavior, PyBehaviorWrapper, bool, uint32, c_Property, isPyBehaviorWrapper, castToPythonObject

from Engine cimport IBehavior

cdef class PythonBehaviorBase:
    cdef PyBehaviorWrapper* _pybehavior

cdef class CythonBehavior(PythonBehaviorBase):
    cpdef PythonBehaviorBase gocBehavior(self, name)
    cpdef bool removeBehavior(self, behavior)
    cdef char* _typeName(self)
    #cdef char* _saveToXml(self)
    cdef void _loadFromXml(self, char *buf, uint32 size)
    cdef bool _processMessage(self, char *evt, char *params)
    cdef uint32 _queryPropertyCount(self)
    cdef char* _queryPropertyNameOf(self, uint32 idx)
    cdef c_Property _getPropertyByIdx(self, uint32 idx)
    cdef c_Property _getPropertyByName(self, char *name)
    cdef void _setPropertyByIdx(self, uint32 idx, c_Property prop)
    cdef void _setPropertyByName(self, char *name, c_Property prop)

cdef PyBehavior* make_pywrapper(CythonBehavior inst)

cdef class CppBehavior(PythonBehaviorBase): 
    cdef void _set_cppbehavior(self, IBehavior* behavior)

cdef CppBehavior create_cpp_wrapper(IBehavior* behavior)

cdef public api IBehavior* create_behavior_from_code(char* name, object code)
cdef public api void release_pythonbehavior_module(char* name)
