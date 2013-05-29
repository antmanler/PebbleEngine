# encoding: utf-8
"""
ShowStats.py

Created by Antmanler on 2012-04-06.
Copyright (c) 2012 Antmanler. All rights reserved.
"""

from Engine import *
from PythonBehavior import *
import horde3d as h3d
from math import sin, cos

class ShowStats(PythonBehavior) :
    
    show = PropertyBool(True)
    stats_level = PropertyInt(1)
    
    def start(self):
        # load over lay resources
        self._font_mat_res = h3d.addResource( h3d.ResTypes.Material, "overlays/font.material.xml", 0 );
        self._panel_mat_res = h3d.addResource( h3d.ResTypes.Material, "overlays/panel.material.xml", 0 );
        # TODO: get resource data from Engine interface
        self._enable = h3d.utils.loadResourcesFromDisk('../../Data/Common')
    
    # handle message
    def preCamRender(self) :
        if self.show and self._enable:
            h3d.utils.showFrameStats( self._font_mat_res, self._panel_mat_res, self.stats_level )
    
    def show_level(self, level):
        try:
            nlevel = int(nlevel)
            if nlevel > h3d.utils.H3DUTMaxStatMode : nlevel = 0
        except:
            nlevel = self.stats_level
        self.stats_level = nlevel
    
    def show_stats(self, show = True):
        self.show = show