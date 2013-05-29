# encoding: utf-8
"""
ScaleFieldGen.py

Created by Antmanler on 2012-05-21.
Copyright (c) 2012 Antmanler. All rights reserved.
"""

from Engine import *
from PythonBehavior import *
import horde3d as h3d

class ScaleFieldGen(PythonBehavior) :
    
    dll_name = PropertyString("scalefieldGen")
    func_name = PropertyString("scaleFiledGenerator")
    replace = PropertyBool(True)
    
    def start(self):
        # get horde id
        h3did = self.gocBehavior('Object3D').horde_id
        self.handle = h3d.sph.setSphScaleFieldGenerateCallback(h3did, self.dll_name, self.func_name)
    
    def release(self) :
        if self.handle is not None :
            h3d.sph.releaseSphScaleFieldGenerateCallback(self.handle)