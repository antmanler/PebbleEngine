import horde3d as h3d

class Scene(SceneBase):

    def __init__(self) :
        SceneBase.__init__(self, 'Default')
        self._particleSystem = None
        self.__mode = 0
        self.__render_mode = 0
        self.__rmode_dict = {}

    def load_res(self) :
        
        class H3DRes: pass
            
        h3dres = H3DRes()
        self._h3dres = h3dres
        # Pipelines
        h3dres.forwardPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0)
        h3dres.deferredPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/deferred_refined.pipeline.xml", 0)
        h3dres.hdrPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/hdr.pipeline.xml", 0)
        h3dres.sphPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/sph.pipeline.xml", 0)
        self.__rmode_dict = {
            0 : ('forward', h3dres.forwardPipe),
            1 : ('deferred', h3dres.deferredPipe),
            2 : ('hdr', h3dres.hdrPipe),
            3 : ('sph', h3dres.sphPipe),
        }
        # Overlays
        h3dres.fontMat = h3d.addResource(h3d.ResTypes.Material, "overlays/font.material.xml", 0)
        h3dres.panelMat = h3d.addResource(h3d.ResTypes.Material, "overlays/panel.material.xml", 0)
        h3dres.logoMat = h3d.addResource(h3d.ResTypes.Material, "overlays/logo.material.xml", 0)
        # Environment
        h3dres.env = h3d.addResource(h3d.ResTypes.SceneGraph, "models/sphere/sphere.scene.xml", 0)
        #h3dres.env = h3d.addResource(h3d.ResTypes.SceneGraph, "models/platform/platform.scene.xml", 0)
        # Skybox
        h3dres.skyBox = h3d.addResource(h3d.ResTypes.SceneGraph, "models/skybox/skybox.scene.xml", 0)
        # Shader for deferred shading
        h3dres.lightMat = h3d.addResource(h3d.ResTypes.Material, "materials/sphLight.material.xml", 0)
        # Knight
        h3dres.knight = h3d.addResource(h3d.ResTypes.SceneGraph, "models/knight/knight.scene.xml", 0)
        h3dres.knightAnim1 = h3d.addResource(h3d.ResTypes.Animation, "animations/knight_order.anim", 0)
        h3dres.knightAnim2 = h3d.addResource(h3d.ResTypes.Animation, "animations/knight_attack.anim", 0)
        # Particle system
        h3dres.particleSys  = h3d.addResource(h3d.ResTypes.SceneGraph, "particles/particleSys1/particleSys1.scene.xml", 0)
        h3dres.particleSys2 = h3d.addResource(h3d.ResTypes.SceneGraph, "particles/sphwater/particlesys.scene.xml", 0)
        if not h3d.utils.loadResourcesFromDisk(DataDir):
            print 'loading of some resources failed: See Horde3D_Log.html'

    def set_particlesys(self, psys) :
        # Attach particle system to hand joint
        h3d.findNodes(self._knight, 'Bip01_R_Hand', h3d.NodeTypes.Joint)
        hand = h3d.getNodeFindResult(0)
        if self._particleSystem is not None :
            h3d.removeNode(self._particleSystem)
        self._particleSystem = h3d.addNodes(hand, psys)
        h3d.setNodeTransform(self._particleSystem, 0, 40, 0, 90, 0, 0, 1, 1, 1)


    def setup_scene(self) :
        h3dres = self._h3dres

        self._env = h3d.addNodes(h3d.RootNode, h3dres.env)
        #h3d.setNodeTransform(self._env, 0, 0, 0, 0, 0, 0, 0.23, 0.23, 0.23)
        h3d.setNodeTransform(self._env, 0, -20, 0, 0, 0, 0, 20, 20, 20)
        # Add skybox
        self._sky = h3d.addNodes(h3d.RootNode, h3dres.skyBox)
        h3d.setNodeTransform(self._sky, 0, 0, 0, 90, 0, 0, 50, 50, 50)

        self._knight = h3d.addNodes(h3d.RootNode, h3dres.knight)
        h3d.setNodeTransform(self._knight, 0, 0, 0, 0, 180, 0, 0.1, 0.1, 0.1)
        h3d.setupModelAnimStage(self._knight, 0, h3dres.knightAnim1, 0, '', False)
        h3d.setupModelAnimStage(self._knight, 1, h3dres.knightAnim2, 0, '', False)
        
        self._light = h3d.addLightNode(h3d.RootNode, 'Light1', h3dres.lightMat, 'LIGHTING', 'SHADOWMAP')
        h3d.setNodeTransform(self._light, 0, 15, 10, -60, 0, 0, 1, 1, 1)
        h3d.setNodeParamF(self._light, h3d.Light.RadiusF, 0, 30)
        h3d.setNodeParamF(self._light, h3d.Light.FovF, 0, 90)
        h3d.setNodeParamI(self._light, h3d.Light.ShadowMapCountI, 1)
        h3d.setNodeParamF(self._light, h3d.Light.ShadowMapBiasF, 0, 0.01)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 0, 1.0)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 1, 0.8)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 2, 0.7)

        # Customize post processing effects
        mat = h3d.findResource(h3d.ResTypes.Material, 'pipelines/postHDR.material.xml')
        # hdrParams: exposure, brightpass threshold, brightpass offset (see shader for description)
        h3d.setMaterialUniform(mat, 'hdrExposure', 2.5, 0.0, 0.0, 0.0)
        h3d.setMaterialUniform(mat, 'hdrBrightThres', 0.5, 0.0, 0.0, 0.0)
        h3d.setMaterialUniform(mat, 'hdrBrightOffset', 0.08, 0.0, 0.0, 0.0)
        
        # Attach particle system to hand joint
        self.set_particlesys(h3dres.particleSys2)
        self.add_camera("MainCamera", self.__rmode_dict[3][1])
        h3d.setNodeTransform(self.camera, -25, 14, 9, -30, -60, 5, 1, 1, 1)
    
    
    def change_render_mode(self, mode) :
        if mode == self.__render_mode : return

        # for safe
        mode = mode % len(self.__rmode_dict)
        # get pipeline and name
        name, pipeline = self.__rmode_dict[mode%4]
        h3d.setNodeParamI( self.camera, h3d.Camera.PipeResI, pipeline )
        
        if mode < 3 and self.__render_mode >= 3 : 
            # reset default particle system
            self.set_particlesys(self._h3dres.particleSys)
        
        # sph pipeline
        if mode == 3 :
            self.set_particlesys(self._h3dres.particleSys2)

        # reset camera, force to resize buffers
        SetCamera(self.camera)
        self.__render_mode = mode
        return name


    def graphic_update(self, cur_fps, ani_time, weight):
        
        if (PIWIO.key_clicked() and PIWIO.clicked_key() == Qt.Key_F5) :self.__mode += 1

        if PIWIO.key_clicked() and PIWIO.clicked_key() == Qt.Key_F3 :
            self.change_render_mode(self.__render_mode + 1)

        if self.__mode > 0 :
            name, _ = self.__rmode_dict[self.__render_mode]
            h3d.utils.showText( "Pipeline: {}".format(name), 0.03, 0.24, 0.026, 1, 1, 1, self._h3dres.fontMat )


        if self.__mode == 3 : self.__mode = 0

        h3d.setModelAnimParams(self._knight, 0, ani_time, weight)
        h3d.setModelAnimParams(self._knight, 1, ani_time, 1.0-weight)
        h3d.updateModel( self._knight, h3d.ModelUpdateFlags.Animation | h3d.ModelUpdateFlags.Geometry )
        
        count = h3d.findNodes(self._particleSystem, '', h3d.NodeTypes.Emitter)
        for i in range(count):
            h3d.updateEmitter(h3d.getNodeFindResult(i), 1.0/cur_fps)

AddNewScene(Scene())

# def Default() : ChangeScene('Default')
