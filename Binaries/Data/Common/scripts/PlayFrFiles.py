# encoding: utf-8
"""
PlayFrFiles.py

Created by Antmanler on 2012-04-04.
Copyright (c) 2012 Antmanler. All rights reserved.
"""

from Engine import *
from PythonBehavior import *
import horde3d as h3d
# import pebble as piw
import re

def tryint(s):
    try:
        return int(s)
    except:
        return s

def alphanum_key(s):
    """ Turn a string into a list of string and number chunks.
        "z23a" -> ["z", 23, "a"]
    """
    return [ tryint(c) for c in re.split('([0-9]+)', s) ]

def sort_nicely(l):
    """ Sort the given list in the way that humans expect.
    """
    l.sort(key=alphanum_key)

if sys.platform == 'darwin' :
    gDataBaseDir = "../../../../../data"
elif sys.platform[:3] == 'win' :
    gDataBaseDir = "../../../../data"

class PlayFrFiles(PythonBehavior) :
    
    fr_dir = PropertyString("armadillo242K")
    play  = PropertyBool(True)
    
    def start(self):
        # get horde id
        obj3d = self.gocBehavior('Object3D')
        self.h3did = obj3d.horde_id
        self._enable = h3d.getNodeType(self.h3did) == h3d.sph.H3DEXT_NodeType_Sph
        # print self.fr_dir, self.play, dir(self._piw_prop_fr_dir)
        # init
        self.__data_list = []
        self.__file_ptr = 0
        self.__skip = 1
        # print "changed!"
        if self._enable :
            if self.fr_dir != "" :
                self.change_fr_dir(self.fr_dir, self.__skip)
            self._sphdata = h3d.getNodeParamI(self.h3did, h3d.sph.H3DEXTSph.VolumeResI)
        else :
            print "PlayFrFiles attached to a wrong type of node, please specify a SPH node instead!"
    
    def update(self):
        if self._enable and self.play and len(self.__data_list) > 0:
            self.__file_ptr += self.__skip
            self.__file_ptr %= len(self.__data_list)
            fn = self.__data_list[self.__file_ptr]
            if not h3d.sph.updateSphDataFromFile(self._sphdata, fn) :
                print "can not load file {}".format(fn)
            else :
                h3d.sph.updateSphNode(self.h3did)
    
    # message handler for change frame dir
    def change_fr_dir(self, content_dir, skip=1) :
        datadir = os.path.join(gDataBaseDir, content_dir)
        if os.path.exists(datadir) and os.path.isdir(datadir):
            #self.__data_list = [ os.path.join(content_dir, fn) for fn in os.listdir(content_dir) ]
            self.__data_list = []
            for fn in os.listdir(datadir) :
                if os.path.splitext(fn)[1].lower() == '.fr' :
                    self.__data_list.append(os.path.join(datadir, fn))
            sort_nicely(self.__data_list)
            self.__skip = skip
            self.__file_ptr = 0
            broadcastMessage("start_animation")
        else :
            print "{} is not exisis or not a directory".format(datadir)
            self.__data_list = []
            

# temporary controller
class SPHDemo(object) :

    # def __init__(self) :
    #     self.__datadir = gDataBaseDir
    #     if sys.platform == 'darwin' :
    #         self.__datadir = '../../../../../data'
    #     elif sys.platform[:3] == 'win' :
    #         self.__datadir = 'd:\data\sphdata'

    def __base(self, scene, scale, skip=2) :
        broadcastMessage("change_fr_dir", os.path.join(scene, '{}{}K'.format(scene, scale)), skip)
        # horde_window.activateWindow()

    def armadillo71K(self, skip=2) :
        self.__base('armadillo', 71, skip)

    def armadillo102K(self, skip=2) :
        self.__base('armadillo', 102, skip)

    def armadillo242K(self, skip=2) :
        self.__base('armadillo', 242, skip)

    def bunny42K(self, skip=2) :
        self.__base('bunny', 42, skip)

    def bunny112K(self, skip=2) :
        self.__base('bunny', 122, skip)

    def bunny337K(self, skip=2) :
        self.__base('bunny', 337, skip)
	
    def box211K(self, skip=1) :
        self.__base('box', 211, skip)	
    
    def stop(self):
        # send a invalid directory
        broadcastMessage("change_fr_dir", "")

