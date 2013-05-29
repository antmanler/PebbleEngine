from Engine import *
from PythonBehavior import *

import horde3d as h3d

class HideMe(PythonBehavior) :
    
    hide = PropertyBool(True)

    def start(self) :

        obj3d = self.gocBehavior('Object3D')
        self.h3did = obj3d.horde_id
        if self.hide :
            h3d.setNodeFlags(self.h3did, h3d.NodeFlags.Inactive, True)
