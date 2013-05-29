# encoding: utf-8
"""
CirOrbitCamera.py

Created by Antmanler on 2012-04-05.
Copyright (c) 2012 Antmanler. All rights reserved.
"""

from Engine import *
from PythonBehavior import *
import horde3d as h3d

class CirOrbitCamera(PythonBehavior) :
    
    def start(self):
        # get horde id
        h3did = self.gocBehavior('Object3D').horde_id
        self.parent_id = h3d.getNodeParent(h3did)
        self._enable = self.parent_id != 0
        # print self._enable, h3did, self.parent_id
        self._rotate = 0.0
        self._animation = False
    
    def update(self):
        if self._enable and self._animation :
            frame_time = h3d.getStat(h3d.Stats.FrameTime, False)
            self._rotate += (30.0 * frame_time) * 0.001 
            t,r,s = h3d.getNodeTransform(self.parent_id)
            h3d.setNodeTransform(self.parent_id, t[0], t[1], t[2], r[0], self._rotate, r[2], s[0], s[1], s[2])
    
    # handle messages
    def start_animation(self):
        # print "OK Recved"
        self._animation = True
    
    def stop_animation(self):
        self._animation = False
