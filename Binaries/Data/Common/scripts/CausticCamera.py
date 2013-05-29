
from Engine import *
from PythonBehavior import *
import horde3d as h3d
from math import sin, cos

def get_smapler_by_name(res, name):
    if 0 == res :
        print 'Error resource'
        return
    cnt = h3d.getResElemCount(res, h3d.MatRes.SamplerElem)
    for i in range(cnt) :
        uni_name = h3d.getResParamStr(res, h3d.MatRes.SamplerElem, i, h3d.MatRes.SampNameStr)
        if uni_name == name : return i
    return -1

class CausticCamera(PythonBehavior) :
    
    light_shader_name = PropertyString("shaders/sph_lighting.shader")
    light_material_name = PropertyString("materials/sph_light.material.xml")
    main_camera_fov = PropertyFloat(45.0)
    
    def get_cmap_res(self) :
        # print self.light_material_name
        res = h3d.findResource(h3d.ResTypes.Material, self.light_material_name)
        idx = get_smapler_by_name(res, 'causticMap')
        if idx >= 0 : h3d.setResParamI(res, h3d.MatRes.SamplerElem, idx, h3d.MatRes.SampTexResI, self.cmap)
        else : print "Get error caustic map id"
        self.light_mat_res = res
        
        # get light matrix uniform id
        res = h3d.findResource(h3d.ResTypes.Shader, self.light_shader_name)
        if res != 0 :
            cnt = h3d.getResElemCount(res, h3d.ShaderRes.UniformElem)
            for i in range(cnt) :
                uni_name = h3d.getResParamStr(res, h3d.ShaderRes.UniformElem, i, h3d.ShaderRes.UnifNameStr)
                if uni_name in self.cmap_mat : self.cmap_mat[uni_name] = i
            self.light_shader_res = res
        else: print "Get error light shader id"

        res = h3d.findResource(h3d.ResTypes.Shader, 'shaders/sph_fluid.shader')
        if res != 0 :
            cnt = h3d.getResElemCount(res, h3d.ShaderRes.UniformElem)
            for i in range(cnt) :
                uni_name = h3d.getResParamStr(res, h3d.ShaderRes.UniformElem, i, h3d.ShaderRes.UnifNameStr)
                if uni_name == 'cameraFov' :
                    self.fov_slot = i
                    break
            self.sph1_shader_res = res
        else : print "Get error fluid shader id"

    def update_cmap_res(self) :
        proj_mat = h3d.getCameraProjMat(self._light_camera)
        _, view_mat = h3d.getNodeTransMats(self._light_camera)
        mat = (proj_mat*(view_mat**-1)).getT()
        #print mat
		
        def set_row(elem_idx, row) :
            for i in range(4) :
                h3d.setResParamF(self.light_shader_res, 
                                 h3d.ShaderRes.UniformElem,
                                 elem_idx, h3d.ShaderRes.UnifDefValueF4,
                                 i, mat[row,i])
                
        set_row(self.cmap_mat['cmat_r1'], 0)
        set_row(self.cmap_mat['cmat_r2'], 1)
        set_row(self.cmap_mat['cmat_r3'], 2)
        set_row(self.cmap_mat['cmat_r4'], 3)
        
    def update_shader_fov(self, fov) :
        h3d.setResParamF(self.sph1_shader_res, 
                         h3d.ShaderRes.UniformElem,
                         self.fov_slot, h3d.ShaderRes.UnifDefValueF4,
                         0, fov)

    def start(self):
        self.cmap_size = 1024
        self.light_fov = 0.0
        self.light_mat_res = 0
        self.light_shader_res = 0
        self.sph1_shader_res = 0
        self.fov_slot = -1
        self.cmap = None
        self.cmap_mat = {
            'cmat_r1':0,
            'cmat_r2':0,
            'cmat_r3':0,
            'cmat_r4':0,
        }
        self.create_cmap = True
        
        obj3d = self.gocBehavior('Object3D')
        self._light_camera = obj3d.horde_id

        # Create a caustic renderable texture
        if self.create_cmap :
            self.cmap = h3d.createTexture("caustic_map", self.cmap_size, self.cmap_size, 
                                        h3d.Formats.TEX_RGBA16F, h3d.ResFlags.TexRenderable)
            # Set camera output texture
            h3d.setNodeParamI(self._light_camera, h3d.Camera.OutTexResI, self.cmap)
            # Get shader uniform locations
            self.get_cmap_res()
                
    #### handle messages #####
    def setFov(self, fov):
        if fov != self.light_fov :
            self.light_fov = fov
            self.update_cmap_res()

    def preCamRender(self) :
        self.update_shader_fov(self.light_fov) 
    
    def postCamRender(self) :
        self.update_shader_fov(self.main_camera_fov)     
