
from Engine import *
from PythonBehavior import *
import horde3d as h3d
from math import sin, cos

def get_smapler_by_name(res, name):
    cnt = h3d.getResElemCount(res, h3d.MatRes.SamplerElem)
    for i in range(cnt) :
        uni_name = h3d.getResParamStr(res, h3d.MatRes.SamplerElem, i, h3d.MatRes.SampNameStr)
        if uni_name == name : return i
    return -1
    
class SphSkybox(PythonBehavior) :

    def start(self):
        # save hordeid
        obj3d = self.gocBehavior('Object3D')
        self._sky = obj3d.horde_id
        self.light_mat_res = h3d.findResource(h3d.ResTypes.Material, "materials/sphLight.material.xml")
        
        # set up skybox
        mesh = h3d.getNodeChild(self._sky, 0)
        res = h3d.getNodeParamI(mesh, h3d.Mesh.MatResI)
        idx = get_smapler_by_name(res, "albedoMap")
        if idx != -1 :
            evn_map = h3d.getResParamI(res, h3d.MatRes.SamplerElem, idx, h3d.MatRes.SampTexResI)
            idx = get_smapler_by_name(self.light_mat_res, "envMap")
            if idx != -1:
                h3d.setResParamI(self.light_mat_res, h3d.MatRes.SamplerElem, idx, h3d.MatRes.SampTexResI, evn_map)
        
    def update(self) :
        pass
        
