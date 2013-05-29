import horde3d as h3d
import horde3d.utils as utils
import os, sys
from numpy import matrix

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


class Scene(SceneBase):

    def __init__(self) :
        SceneBase.__init__(self, 'SphWin')
        self.__animation = False
        self.__demo = SPHDemo(self)
        self.reset()

    def reset(self) :
        self.__rotate = 0
        self.__mode = 0
        self.__data_list = []
        self.__file_ptr = 0
        self.__skip = 1
        self._sky = 0
        self.cmap_size = 1024
        self.light_fov = 90.0
        self.ligth_shader_res = 0
        self.sph1_shader_res = 0
        self.fov_slot = -1
        self.cmap_mat = {
            'cmat_r1':0,
            'cmat_r2':0,
            'cmat_r3':0,
            'cmat_r4':0,
        }
        self.create_cmap = True
    
    @property
    def animation(self):
        return self.__animation
    @animation.setter
    def animation(self, value):
        self.__animation = value is True

    @property
    def demo(self): return self.__demo

    def get_cmap_res(self) :
        res = self._h3dres.lightMat
        cnt = h3d.getResElemCount(res, h3d.MatRes.SamplerElem)
        for i in range(cnt) :
            uni_name = h3d.getResParamStr(res, h3d.MatRes.SamplerElem, i, h3d.MatRes.SampNameStr)
            if uni_name == 'causticMap' :
                h3d.setResParamI(res, h3d.MatRes.SamplerElem, i, h3d.MatRes.SampTexResI, self._h3dres.cmap)
                break

        res = h3d.findResource(h3d.ResTypes.Shader, 'shaders/sph_lighting.shader')
        if res != 0 :
            cnt = h3d.getResElemCount(res, h3d.ShaderRes.UniformElem)
            for i in range(cnt) :
                uni_name = h3d.getResParamStr(res, h3d.ShaderRes.UniformElem, i, h3d.ShaderRes.UnifNameStr)
                if uni_name in self.cmap_mat :
                    self.cmap_mat[uni_name] = i
            self.ligth_shader_res = res

        res = h3d.findResource(h3d.ResTypes.Shader, 'shaders/sph_fluid.shader')
        if res != 0 :
            cnt = h3d.getResElemCount(res, h3d.ShaderRes.UniformElem)
            for i in range(cnt) :
                uni_name = h3d.getResParamStr(res, h3d.ShaderRes.UniformElem, i, h3d.ShaderRes.UnifNameStr)
                if uni_name == 'cameraFov' :
                    self.fov_slot = i
                    break
            self.sph1_shader_res = res

    def get_smapler_by_name(self, res, name):
        cnt = h3d.getResElemCount(res, h3d.MatRes.SamplerElem)
        for i in range(cnt) :
            uni_name = h3d.getResParamStr(res, h3d.MatRes.SamplerElem, i, h3d.MatRes.SampNameStr)
            if uni_name == name : return i
        return -1

    def update_cmap_res(self) :
        #proj_mat = matrix(h3d.getCameraProjMat(self._light_camera)).reshape((4,4)).getT()
        proj_mat = h3d.getCameraProjMat(self._light_camera)
        _, view_mat = h3d.getNodeTransMats(self._light_camera)
        #view_mat = matrix(view_mat).reshape((4,4)).getT()
        mat = (proj_mat*(view_mat**-1)).getT()

        def set_row(elem_idx, row) :
            for i in range(4) :
                h3d.setResParamF(self.ligth_shader_res, 
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

    def start_animation(self, content_dir, skip=10, capture=True) :
        if os.path.exists(content_dir) and os.path.isdir(content_dir):
            #self.__data_list = [ os.path.join(content_dir, fn) for fn in os.listdir(content_dir) ]
            self.__data_list = []
            for fn in os.listdir(content_dir) :
                if os.path.splitext(fn)[1].lower() == '.fr' :
                    self.__data_list.append(os.path.join(content_dir, fn))
            sort_nicely(self.__data_list)
            self.__skip = skip
            self.__file_ptr = 0
            self.__shot_dir = ''
            if capture :
                self.__shot_dir = os.path.join(content_dir, "shot")
                if not os.path.exists(self.__shot_dir) :
                    os.mkdir(self.__shot_dir)
            #self.animation = True
        else :
            print "{} is not exisis or not a directory".format(content_dir)

    def change_skybox(self, skyres):
        if self._sky != 0:
            # remove the old sky box
            h3d.removeNode(self._sky)

        h3dres = self._h3dres
        # Add skybox
        self._sky = h3d.addNodes(h3d.RootNode, skyres)
        h3d.setNodeTransform(self._sky, 0, 0, 0, 0, 0, 0, 20, 20, 20)
        # Set ligth envmap
        mesh = h3d.getNodeChild(self._sky, 0)
        res = h3d.getNodeParamI(mesh, h3d.Mesh.MatResI)
        idx = self.get_smapler_by_name(res, "albedoMap")
        if idx != -1 :
            evn_map = h3d.getResParamI(res, h3d.MatRes.SamplerElem, idx, h3d.MatRes.SampTexResI)
            idx = self.get_smapler_by_name(h3dres.lightMat, "envMap")
            if idx != -1:
                h3d.setResParamI(h3dres.lightMat, h3d.MatRes.SamplerElem, idx, h3d.MatRes.SampTexResI, evn_map)

    def load_res(self) :
        self.reset()
        class H3DRes: pass
        h3dres = H3DRes()
        self._h3dres = h3dres
        # Pipelines
        h3dres.forwardPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0)
        h3dres.deferredPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/sph.pipeline.xml", 0)
        if self.create_cmap :
            h3dres.deferredPipe2 = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/caustic.pipeline.xml", 0)
        # Overlays
        h3dres.fontMat = h3d.addResource(h3d.ResTypes.Material, "overlays/font.material.xml", 0)
        h3dres.panelMat = h3d.addResource(h3d.ResTypes.Material, "overlays/panel.material.xml", 0)
        h3dres.logoMat = h3d.addResource(h3d.ResTypes.Material, "overlays/logo.material.xml", 0)
        # Shader for deferred shading
        h3dres.lightMat = h3d.addResource(h3d.ResTypes.Material, "materials/sph_light.material.xml", 0)
        # Environment
        #h3dres.env = h3d.addResource(h3d.ResTypes.SceneGraph, "models/platform/platform.scene.xml", 0)
        # Skybox
        h3dres.skyBox = h3d.addResource(h3d.ResTypes.SceneGraph, "models/skybox/skybox_stpeters.scene.xml", 0)
        h3dres.skyBox2 = h3d.addResource(h3d.ResTypes.SceneGraph, "models/skybox/skybox_desert.scene.xml", 0)
        # Volume box
        h3dres.volBox = h3d.addResource(h3d.ResTypes.SceneGraph, "sphscene/sph1.scene.xml", 0)
        # Room box
        h3dres.roomBox = h3d.addResource(h3d.ResTypes.SceneGraph, "models/roombox/box.scene.xml", 0)
        h3dres.plane = h3d.addResource(h3d.ResTypes.SceneGraph, "models/plane/plane.scene.xml", 0)
        # Load resources
        if not h3d.utils.loadResourcesFromDisk(DataDir + '|../../Data/SPH'):
            print 'loading of some resources failed: See Horde3D_Log.html'

        if self.create_cmap :
            # Create a caustic renderable texture
            h3dres.cmap = h3d.createTexture("caustic_map", self.cmap_size, self.cmap_size, 
                                            h3d.Formats.TEX_RGBA16F, h3d.ResFlags.TexRenderable)
            # Get shader uniform locations
            self.get_cmap_res()
               
    def setup_scene(self) :
        h3dres = self._h3dres

        # Add environment
        #self._env = h3d.addNodes(h3d.RootNode, h3dres.env)
        #h3d.setNodeTransform(self._env, 0, 0, 0, 0, 0, 0, 0.08, 0.08, 0.08)
        # Add skybox
        self.change_skybox(h3dres.skyBox)
        # Add light source
        self._light = h3d.addLightNode(h3d.RootNode, 'Light1', h3dres.lightMat, 'LIGHTING', 'SHADOWMAP')
        h3d.setNodeTransform(self._light, 15, 10, -5, -40, 120, 0, 1, 1, 1)
        h3d.setNodeParamF(self._light, h3d.Light.RadiusF, 0, 100)
        h3d.setNodeParamF(self._light, h3d.Light.FovF, 0, self.light_fov)
        h3d.setNodeParamI(self._light, h3d.Light.ShadowMapCountI, 1)
        h3d.setNodeParamF(self._light, h3d.Light.ShadowSplitLambdaF, 0, 0.9)
        h3d.setNodeParamF(self._light, h3d.Light.ShadowMapBiasF, 0, 0.001)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 0, 0.95)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 1, 0.95)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 2, 0.85)
        if self.create_cmap :
            # Light camera
            self._light_camera = h3d.addCameraNode(self._light, "light_camera", h3dres.deferredPipe2)
            h3d.setNodeParamI(self._light_camera, h3d.Camera.OutTexResI, h3dres.cmap)
            h3d.setNodeParamI(self._light_camera, h3d.Camera.ViewportXI, 0)
            h3d.setNodeParamI(self._light_camera, h3d.Camera.ViewportXI, 0)
            h3d.setNodeParamI(self._light_camera, h3d.Camera.ViewportWidthI, self.cmap_size)
            h3d.setNodeParamI(self._light_camera, h3d.Camera.ViewportHeightI, self.cmap_size)
            h3d.resizePipelineBuffers(h3dres.deferredPipe2, self.cmap_size, self.cmap_size )
            h3d.setupCameraView(self._light_camera, self.light_fov, 1.0, 0.1, 1000.0)
            # Update ligth shader uniforms
            self.update_cmap_res()

        # Create volume data
        self._vol = h3d.addNodes(h3d.RootNode, h3dres.volBox)
        self._sphdata = h3d.getNodeParamI(self._vol, 20000) # for VolumeResI = 20000

        self._room = h3d.addNodes(h3d.RootNode, h3dres.roomBox)
        h3d.setNodeTransform(self._room, 0, -0.01, 0, 0, 0, 0, 20, 20, 20)

        self._plane = h3d.addNodes(h3d.RootNode, h3dres.plane)
        h3d.setNodeTransform(self._plane, 0, 0, 0, 0, 0, 0, 0.2, 0.2, 0.2)

        # Add a camera
        self.add_camera("MainCamera", h3dres.deferredPipe)
        h3d.setNodeTransform(self.camera, -2.0, 1.2, -6.5, -13, -170, 0, 1, 1, 1)
        #h3d.setNodeTransform(self.camera, -0.8, 1.06, -1.08, -41, -154, -6.4, 1, 1, 1)
        #h3d.setNodeTransform(self.camera, -0.32, 0.23, -0.54, -19, -174, -0.01, 1, 1, 1)
        self.__mode = 0

    
    def graphic_update(self, cur_fps, ani_time, weight):
        forward = h3d.getNodeParamI( self.camera, h3d.Camera.PipeResI ) == self._h3dres.forwardPipe 
        
        if (PIWIO.key_clicked() and PIWIO.clicked_key() == Qt.Key_F5) :self.__mode += 1
        
        if self.__mode > 0 :
            if forward:
                h3d.utils.showText( "Pipeline: forward", 0.03, 0.24, 0.026, 1, 1, 1, self._h3dres.fontMat )
            else:
                h3d.utils.showText( "Pipeline: deferred", 0.03, 0.24, 0.026, 1, 1, 1, self._h3dres.fontMat )
        
        if self.__mode == 3 : self.__mode = 0

        if PIWIO.key_clicked():
            if PIWIO.clicked_key() == Qt.Key_F3 :
                if forward :
                    h3d.setNodeParamI( self.camera, h3d.Camera.PipeResI, self._h3dres.deferredPipe )
                else :
                    h3d.setNodeParamI( self.camera, h3d.Camera.PipeResI, self._h3dres.forwardPipe )
                SetCamera(self.camera)

            elif PIWIO.clicked_key() == Qt.Key_1 :
                self.change_skybox(self._h3dres.skyBox)
            elif PIWIO.clicked_key() == Qt.Key_2 :
                self.change_skybox(self._h3dres.skyBox2)

            elif PIWIO.clicked_key() == Qt.Key_3 :
                mtl = h3d.getNodeParamI(self._vol, 20001)
                h3d.setMaterialUniform(mtl, "FluidColor", 0.59, 0.69, 0.99, 1.0)
                h3d.setMaterialUniform(mtl, "absorptionIdx", 2.0, 1.1, 0.45, 0.0)
            elif PIWIO.clicked_key() == Qt.Key_4 :
                mtl = h3d.getNodeParamI(self._vol, 20001)
                h3d.setMaterialUniform(mtl, "FluidColor", 0.0, 0.0, 0.0, 0.0)
                h3d.setMaterialUniform(mtl, "absorptionIdx", 2.0, 1.1, 0.45, 0.0)

        if self.animation :
            self.__rotate += 30.0/cur_fps 
            anchor = h3d.getNodeParent(self.camera)
            t,r,s = h3d.getNodeTransform(anchor)
            h3d.setNodeTransform(anchor, t[0], t[1], t[2], r[0], self.__rotate, r[2], s[0], s[1], s[2])

        if len(self.__data_list) > 0:
            self.__file_ptr += self.__skip
            self.__file_ptr %= len(self.__data_list)
            fn = self.__data_list[self.__file_ptr]
            if not SPH.UpdateSphData(self._sphdata, fn) :
                print "can not load file {}".format(fn)
            else :
                SPH.AdvanceSph(self._vol)
                if self.__shot_dir != '' and self.__file_ptr > 0 :
                    fn = self.__data_list[self.__file_ptr-1]
                    fn = os.path.join(self.__shot_dir, '{}.tga'.format(os.path.split(fn)[-1]))
                    utils.screenshot(fn)



        if not forward and self.create_cmap :
            self.update_shader_fov(self.light_fov)
            h3d.render(self._light_camera)
            # restore
            self.update_shader_fov(45.0)

class SPHDemo(object) :

    def __init__(self, sph) :
        self.sph = sph
        if sys.platform == 'darwin' :
            self.__datadir = '../../../../../data'
        elif sys.platform[:3] == 'win' :
            self.__datadir = 'd:\data\sphdata'

    def __base(self, scene, scale, skip=5, capture=True) :
        self.sph.start_animation(os.path.join(self.__datadir, scene, '{}{}K'.format(scene, scale)), skip, capture)
        horde_window.activateWindow()

    def armadillo71K(self, skip=5, capture=True) :
        self.__base('armadillo', 71, skip, capture)

    def armadillo102K(self, skip=5, capture=True) :
        self.__base('armadillo', 102, skip, capture)

    def armadillo242K(self, skip=5, capture=True) :
        self.__base('armadillo', 242, skip, capture)

    def bunny42K(self, skip=5, capture=True) :
        self.__base('bunny', 42, skip, capture)

    def bunny112K(self, skip=5, capture=True) :
        self.__base('bunny', 122, skip, capture)

    def bunny337K(self, skip=5, capture=True) :
        self.__base('bunny', 337, skip, capture)


AddNewScene(Scene())

# def Sph1() : ChangeScene('Sph1')

