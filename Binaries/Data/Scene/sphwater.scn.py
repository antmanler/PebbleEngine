import horde3d as h3d

class Scene(SceneBase):

    def __init__(self) :
        SceneBase.__init__(self, 'Sph')
        self.__mode = 0

    def load_res(self) :
        
        class H3DRes: pass
            
        h3dres = H3DRes()
        self._h3dres = h3dres
        # Pipelines
        h3dres.forwardPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0)
        h3dres.deferredPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/sphvol.pipeline.xml", 0)
        # Overlays
        h3dres.fontMat = h3d.addResource(h3d.ResTypes.Material, "overlays/font.material.xml", 0)
        h3dres.panelMat = h3d.addResource(h3d.ResTypes.Material, "overlays/panel.material.xml", 0)
        h3dres.logoMat = h3d.addResource(h3d.ResTypes.Material, "overlays/logo.material.xml", 0)
        # Shader for deferred shading
        h3dres.lightMat = h3d.addResource(h3d.ResTypes.Material, "materials/sphLight.material.xml", 0)
        # Environment
        h3dres.env = h3d.addResource(h3d.ResTypes.SceneGraph, "models/platform/platform.scene.xml", 0)
        # Skybox
        h3dres.skyBox = h3d.addResource(h3d.ResTypes.SceneGraph, "models/skybox/skybox_desert.scene.xml", 0)
        # Volume box
        h3dres.volBox = h3d.addResource(h3d.ResTypes.SceneGraph, "models/sphvolbox/box.scene.xml", 0)
        if not h3d.utils.loadResourcesFromDisk(DataDir):
            print 'loading of some resources failed: See Horde3D_Log.html'
        # Load sph volume
        h3dres.sphVol = SPH.LoadVolumeData('./frame_273.fr')
        if h3dres.sphVol != 0:
            material =  h3d.findResource( h3d.ResTypes.Material, "models/sphvolbox/box.material.xml" )
            h3d.setResParamI(material, h3d.MatRes.SamplerElem, 0, h3d.MatRes.SampTexResI, h3dres.sphVol)
        else :
            print 'can not laod sph raw volume data'

    def setup_scene(self) :
        h3dres = self._h3dres

        # Add environment
        self._env = h3d.addNodes(h3d.RootNode, h3dres.env)
        h3d.setNodeTransform(self._env, 0, 0, 0, 0, 0, 0, 0.23, 0.23, 0.23)
        # Add skybox
        self._sky = h3d.addNodes(h3d.RootNode, h3dres.skyBox)
        h3d.setNodeTransform(self._sky, 0, 0, 0, 0, 0, 0, 100, 50, 100)
        # Add light source
        self._light = h3d.addLightNode(h3d.RootNode, 'Light1', h3dres.lightMat, 'LIGHTING', 'SHADOWMAP')
        h3d.setNodeTransform(self._light, 50, 40, -50, -60, 130, 0, 1, 1, 1)
        h3d.setNodeParamF(self._light, h3d.Light.RadiusF, 0, 250)
        h3d.setNodeParamF(self._light, h3d.Light.FovF, 0, 90)
        h3d.setNodeParamI(self._light, h3d.Light.ShadowMapCountI, 3)
        h3d.setNodeParamF(self._light, h3d.Light.ShadowSplitLambdaF, 0, 0.9)
        h3d.setNodeParamF(self._light, h3d.Light.ShadowMapBiasF, 0, 0.001)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 0, 0.9)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 1, 0.9)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 2, 0.9)
        # Create volume data
        self._vol = h3d.addNodes(h3d.RootNode, h3dres.volBox)
        h3d.setNodeTransform(self._vol, 0, 3, 0, 0, 0, 0, 3, 3, 3)

        # Add a camera
        self.add_camera("MainCamera", h3dres.deferredPipe)
        h3d.setNodeTransform(self.camera, 9, 6.0, -14, -15, 145, 0, 1, 1, 1)

            
    def graphic_update(self, cur_fps, ani_time, weight):
        forward = h3d.getNodeParamI( self.camera, h3d.Camera.PipeResI ) == self._h3dres.forwardPipe 
        
        if (PIWIO.key_clicked() and PIWIO.clicked_key() == Qt.Key_F5) :self.__mode += 1
        
        if self.__mode > 0 :
            if forward:
                h3d.utils.showText( "Pipeline: forward", 0.03, 0.24, 0.026, 1, 1, 1, self._h3dres.fontMat )
            else:
                h3d.utils.showText( "Pipeline: deferred", 0.03, 0.24, 0.026, 1, 1, 1, self._h3dres.fontMat )
        
        if self.__mode == 3 : self.__mode = 0

        if PIWIO.key_clicked() and PIWIO.clicked_key() == Qt.Key_F3 :
            if forward :
                h3d.setNodeParamI( self.camera, h3d.Camera.PipeResI, self._h3dres.deferredPipe )
            else :
                h3d.setNodeParamI( self.camera, h3d.Camera.PipeResI, self._h3dres.forwardPipe )
            SetCamera(self.camera)

        #self._crowdSim.update(1.0/cur_fps)


AddNewScene(Scene())

# def Sph() : ChangeScene('Sph')
