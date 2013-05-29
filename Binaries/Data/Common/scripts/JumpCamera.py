
from Engine import *
from PythonBehavior import *
import horde3d as h3d
from math import sin, cos

class JumpCamera(PythonBehavior) :
    
    jump = PropertyBool(True)

    def start(self):
        self.time = 0
        self.x0y0 = None
        print self.jump
        self.jump = False
        print self.jump
        self.recv_msg = self.gocBehavior('RecvMsg')
        print self.recv_msg.typeName()
        # get horde id
        obj3d = self.gocBehavior('Object3D')
        self.h3did = obj3d.horde_id
    
    def update(self) :
        self.time += 0.01
        t, r, s = h3d.getNodeTransform(self.h3did)
        if self.x0y0 is None : self.x0y0 = t[0:2]
        h3d.setNodeTransform(self.h3did, self.x0y0[0] * (1.0 + cos(self.time)), self.x0y0[1] * (0.5 + sin(self.time)), t[2], r[0], r[1], r[2], s[0], s[1], s[2])
        # send message
        i_time = int(self.time)
        if i_time % 1000 == 0 :
            self.sendMessage('printH3dId', self.h3did)
        if i_time == 5000:
            self.removeBehavior(self.recv_msg)
        
