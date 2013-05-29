# encoding: utf-8
"""
PlayFrFiles.py

Created by Antmanler on 2012-04-04.
Copyright (c) 2012 Antmanler. All rights reserved.
"""

from Engine import *
from PythonBehavior import *
from ctypes import *

import numpy as np
import horde3d as h3d

import os
import re
import struct

def tryint(s):
    try: return int(s)
    except: return s

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

class PlayFrFilesEx(PythonBehavior) :
    
    fr_dir   = PropertyString("armadillo242K")
    play     = PropertyBool(True)
    template = PropertyString()
    
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
        self.__solids = {}
        # print self.cm
        if self._enable :
            if self.fr_dir != "" :
                self.change_fr_dir(self.fr_dir, self.__skip)
            self._sphdata = h3d.getNodeParamI(self.h3did, h3d.sph.H3DEXTSph.VolumeResI)
            # parse template
            if self.template :
                solids = [ x.split(':') for x in self.template.split() ]
                for kind, params in solids :
                    scngraph, cx, cy, cz, sx, sy, sz, bx, by, bz = params.split(';')
                    scnres = h3d.findResource(h3d.ResTypes.SceneGraph, scngraph)
                    if not scnres :
                        print "Can not find template's scene graph"
                        return
                    # print scngraph, scnres, cx, cy, cz, sx, sy, sz
                    self.__solids[int(kind)] = dict(scnres=scnres,
                                                    com = np.array([float(cx), float(cy), float(cz)]),
                                                    scale = np.array([float(sx), float(sy), float(sz)]),
                                                    bscale = np.array([float(bx), float(by), float(bz)]),
                                                    nodes = [])

        else :
            print "PlayFrFiles attached to a wrong type of node, please specify a SPH node instead!"
    
    def update(self):
        if self._enable and self.play and len(self.__data_list) > 0:
            self.__file_ptr += self.__skip
            self.__file_ptr %= len(self.__data_list)
            fn = self.__data_list[self.__file_ptr]

            with open(fn, 'rb') as fin :
                # read 4btyes as int
                num_par = struct.unpack("i", fin.read(4))[0]
                # load particles
                buf_size = 4 + 3*4*num_par
                buf = (c_char * buf_size)()
                fin.seek(0, 0)
                fin.readinto(buf)

                if not h3d.sph.updateSphData(self._sphdata, buf, buf_size) :
                    print "can not load file {}".format(fn)
                else :
                    h3d.sph.updateSphNode(self.h3did)
                    
                    curr_state = {}
                    fsz = os.path.getsize(fn)
                    # parse for solid node
                    if (buf_size + 2*(4+4)) < fsz :
                        # this file contain solid particles
                        # skip the sample dist and density
                        fin.seek(4+4, os.SEEK_CUR)
                        solid_num = struct.unpack("i", fin.read(4))[0]
                        # print solid_num
                        for solid in range(solid_num) :
                            solid_id = struct.unpack("i", fin.read(4))[0]
                            # CoM
                            buf = fin.read(4*3)
                            com = np.frombuffer(buf, dtype=np.float32)
                            # rotation matrix
                            buf = fin.read(4*9)
                            rot = np.frombuffer(buf, dtype=np.float32).reshape((3, 3))
                            
                            state =  curr_state.get(solid_id, [])
                            if not state : curr_state[solid_id] = state
                            state.append( (com, rot) )
                    
                    # update solids
                    self.update_solids(curr_state)

    def update_solids(self, curr_state) :

        for solid_id, params in self.__solids.items() :

            solid_state = curr_state.get(solid_id, [])
            nodes  = params['nodes']
            scale  = params['scale']
            bscale = params['bscale']
            CoM    = params['com']

            # translate to CoM
            tmat1 = np.mat(np.eye(4))
            tmat1[0, 3], tmat1[1, 3], tmat1[2, 3] = -CoM
            # scale mat
            smat = np.mat(np.eye(4))
            smat[0, 0], smat[1, 1], smat[2, 2] = scale

            if len(nodes) > len(solid_state) :
                # remove extra solids
                for _ in range(len(nodes) - len(solid_state)) :
                    # print "remove node"
                    h3d.removeNode(nodes.pop())
            elif len(nodes) < len(solid_state):
                # add nodes
                for _ in range(len(solid_state) - len(nodes)) :
                    # print "add node"
                    node = h3d.addNodes(h3d.RootNode, params['scnres'])
                    if not node :
                        print "failed to add solid node"
                        return
                    h3d.setNodeTransform(node, 0, 0, 0, 0, 0, 0, scale[0], scale[1], scale[2])
                    nodes.append(node)

            for i in range(len(solid_state)) :
                node = nodes[i]
                cm, rot = solid_state[i]

                # rotation matrix
                rmat = np.mat(np.zeros((4,4)))
                rmat[3, 3] = 1.0
                rmat[0:3, 0:3] = np.mat(rot).T + rmat[0:3, 0:3]
                # print rmat

                # translate to pos
                pos = cm*bscale
                tmat2 = np.mat(np.eye(4))
                tmat2[0, 3], tmat2[1, 3], tmat2[2, 3] = pos

                transform = (tmat2*smat*rmat*tmat1)
                h3d.setNodeTransMat(node, transform)

    
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

