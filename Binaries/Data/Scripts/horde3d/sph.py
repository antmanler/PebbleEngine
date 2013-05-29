# encoding: utf-8
"""
sph.py

Created by Antmanler on 2012-04-04.
Copyright (c) 2012 Antmanler. All rights reserved.
"""

from ctypes import *
import sys
import os

if not 'c_bool' in globals():
    c_bool = c_int

__all__ = []
__platform = sys.platform
if __platform == 'darwin' :
    h3d = cdll.LoadLibrary('libHorde3D.dylib')
elif __platform[:3] == 'win' :
    h3d = cdll.LoadLibrary('Horde3D.dll')
else:
    h3d = cdll.LoadLibrary('libHorde3D.so')

if hasattr(h3d, 'h3dextAddSphNode'):
    H3DEXT_NodeType_Sph = 300
    __all__.append('H3DEXT_NodeType_Sph')
    H3DEXT_ResType_Sph = 100
    __all__.append('H3DEXT_ResType_Sph')
    
    class H3DEXTSph(object):
        VolumeResI = 20000
        MatResI    = 20001
    __all__.append('Sph')
    
    addSphNode = h3d.h3dextAddSphNode
    addSphNode.restype = c_int
    addSphNode.argtypes = [c_int, c_char_p, c_int, c_int]
    __all__.append('addSphNode')
    
    updateSphData = h3d.h3dextUpdateSphData
    updateSphData.restype = c_bool
    updateSphData.argtypes = [c_int, c_char_p, c_int]
    __all__.append('updateSphData')
    
    updateSphDataFromFile = h3d.h3dextUpdateSphDataFromFile
    updateSphDataFromFile.restype = c_bool
    updateSphDataFromFile.argtypes = [c_int, c_char_p]
    __all__.append('updateSphDataFromFile')
    
    updateSphNode = h3d.h3dextUpdateSphNode
    updateSphNode.restype = None
    updateSphNode.argtypes = [c_int]
    __all__.append('updateSphNode')
    
    '''void scaleFiledGenerator (const float* points, const int points_num,
    const float xmin, const float ymin, const float zmin,
    const float xmax, const float ymax, const float zmax,
    float* saclefiled, const int width, const int height, const int depth)'''
    __setSphScaleFieldGenerateCallback = h3d.h3dextSetSphScaleFieldGenerateCallback
    __setSphScaleFieldGenerateCallback.restype = c_bool
    __setSphScaleFieldGenerateCallback.argtypes = [c_int, c_void_p]
    def setSphScaleFieldGenerateCallback(sph_node, dll_name, func_name) :
        dir, filename = os.path.split(dll_name)
        dyobj = os.path.join(dir, os.path.splitext(filename)[0])
        
        if __platform == 'darwin' :
            dyobj_path = "lib{}.dylib".format(dyobj)
        elif __platform[:3] == 'win' :
            dyobj_path = "{}.dll".format(dyobj)
        else:
            dyobj_path = "lib{}.so".format(dyobj)
        
        try:
            lib = cdll.LoadLibrary(dyobj_path)
        except :
            print "Load {} error".format(dyobj_path)
            return None
        
        if hasattr(lib, 'scaleFieldInit') and hasattr(lib, 'scaleFieldRelease') :
            if hasattr(lib, func_name) :
                func = getattr(lib, func_name)
                lib.scaleFieldInit()
                if __setSphScaleFieldGenerateCallback(sph_node, cast(func, c_void_p)) : 
                    return lib
                else : print 'False'
                return  None
            else :
                print "Function of {} does not exist in {}".format(func_name, dyobj)
                return None
        else :
            print "It's not a valid lib, for it's lack of 'scaleFieldInit' or 'scaleFieldRelease' or both."
            return None
            
    __all__.append('setSphScaleFieldGenerateCallback')
    
    def releaseSphScaleFieldGenerateCallback(handle) :
        # release it first
        handle.scaleFieldRelease()
        if __platform[:3] == 'win' :
            windll.kernel32.FreeLibrary(handle._handle)
        else:
            if __platform == 'darwin' :
                libdl = CDLL("libdl.dylib")
            else :
                libdl = CDLL("libdl.so")
            libdl.dlclose(handle._handle)
    __all__.append('releaseSphScaleFieldGenerateCallback')
        
    