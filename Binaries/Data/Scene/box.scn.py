import horde3d as h3d
from math import sin, cos, radians, degrees, sqrt, atan2
from random import random, seed

class Scene(SceneBase):

    def __init__(self) :
        SceneBase.__init__(self, 'Box')
        self.__mode = 0

    def load_res(self) :
        
        class H3DRes: pass
            
        h3dres = H3DRes()
        self._h3dres = h3dres
        # Pipelines
        #h3dres.forwardPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0)
        h3dres.hdrPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/hdr.pipeline.xml", 0)
        h3dres.deferredPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/deferred_refined.pipeline.xml", 0)
        # Overlays
        h3dres.fontMat = h3d.addResource(h3d.ResTypes.Material, "overlays/font.material.xml", 0)
        h3dres.panelMat = h3d.addResource(h3d.ResTypes.Material, "overlays/panel.material.xml", 0)
        h3dres.logoMat = h3d.addResource(h3d.ResTypes.Material, "overlays/logo.material.xml", 0)
        # Shader for deferred shading
        h3dres.lightMat = h3d.addResource(h3d.ResTypes.Material, "materials/light.material.xml", 0)
        # Skybox
        h3dres.skyBox = h3d.addResource(h3d.ResTypes.SceneGraph, "models/skybox/skybox.scene.xml", 0)
        # Particle system
        h3dres.particleSys  = h3d.addResource(h3d.ResTypes.SceneGraph, "particles/particleSys1/particleSys1.scene.xml", 0)
        # Load physics scene
        h3dres.dominoRes = h3d.addResource(h3d.ResTypes.SceneGraph, "models/domino/domino.scene.xml", 0)
        if not h3d.utils.loadResourcesFromDisk(DataDir):
            print 'loading of some resources failed: See Horde3D_Log.html'

    def setup_scene(self) :
        h3dres = self._h3dres

        # Add skybox
        self._sky = h3d.addNodes(h3d.RootNode, h3dres.skyBox)
        h3d.setNodeTransform(self._sky, 0, 0, 0, 0, 0, 0, 500, 500, 500)
        # Add light source
        self._light = h3d.addLightNode(h3d.RootNode, 'Light1', h3dres.lightMat, 'LIGHTING', 'SHADOWMAP')
        h3d.setNodeTransform(self._light, 0, 20, 50, -30, 0, 0, 1, 1, 1)
        h3d.setNodeParamF(self._light, h3d.Light.RadiusF, 0, 200)
        h3d.setNodeParamF(self._light, h3d.Light.FovF, 0, 90)
        h3d.setNodeParamI(self._light, h3d.Light.ShadowMapCountI, 3)
        h3d.setNodeParamF(self._light, h3d.Light.ShadowSplitLambdaF, 0, 0.9)
        h3d.setNodeParamF(self._light, h3d.Light.ShadowMapBiasF, 0, 0.001)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 0, 0.9)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 1, 0.7)
        h3d.setNodeParamF(self._light, h3d.Light.ColorF3, 2, 0.75)
        # Add domino scene
        self._domino = h3d.addNodes(h3d.RootNode, h3dres.dominoRes )
        # Add particle effects
        h3d.findNodes(self._domino, 'sphere', h3d.NodeTypes.Mesh)
        sphere = h3d.getNodeFindResult(0)
        self._particleSystem = h3d.addNodes(sphere, h3dres.particleSys)
        # Set particle parameter
        count = h3d.findNodes(self._particleSystem, '', h3d.NodeTypes.Emitter)
        for i in range(count):
            node = h3d.getNodeFindResult(i)
            h3d.setNodeParamF(node, h3d.Emitter.ForceF3, 1, 1.5)
            h3d.setNodeParamF(node, h3d.Emitter.EmissionRateF, 0, 200)
            h3d.setNodeParamF(node, h3d.Emitter.SpreadAngleF, 0, 45)

        # Customize post processing effects
        mat = h3d.findResource(h3d.ResTypes.Material, 'pipelines/postHDR.material.xml')
        # hdrParams: exposure, brightpass threshold, brightpass offset (see shader for description)
        h3d.setMaterialUniform(mat, 'hdrExposure', 2.5, 0.0, 0.0, 0.0)
        h3d.setMaterialUniform(mat, 'hdrBrightThres', 0.5, 0.0, 0.0, 0.0)
        h3d.setMaterialUniform(mat, 'hdrBrightOffset', 0.08, 0.0, 0.0, 0.0)

        self.add_camera("MainCamera", h3dres.hdrPipe)
        h3d.setNodeTransform(self.camera, -14.0, 40.0, 24.0, -50.0, -20, 1.3, 1, 1, 1)
            
    def graphic_update(self, cur_fps, ani_time, weight):
        hdr = h3d.getNodeParamI( self.camera, h3d.Camera.PipeResI ) == self._h3dres.hdrPipe 
        
        if (PIWIO.key_clicked() and PIWIO.clicked_key() == Qt.Key_F5) :self.__mode += 1
        
        if self.__mode > 0 :
            if hdr:
                h3d.utils.showText( "Pipeline: hdr", 0.03, 0.24, 0.026, 1, 1, 1, self._h3dres.fontMat )
            else:
                h3d.utils.showText( "Pipeline: deferred", 0.03, 0.24, 0.026, 1, 1, 1, self._h3dres.fontMat )
        
        if self.__mode == 3 : self.__mode = 0

        if PIWIO.key_clicked() and PIWIO.clicked_key() == Qt.Key_F3 :
            if hdr :
                h3d.setNodeParamI( self.camera, h3d.Camera.PipeResI, self._h3dres.deferredPipe )
            else :
                h3d.setNodeParamI( self.camera, h3d.Camera.PipeResI, self._h3dres.hdrPipe )
            SetCamera(self.camera)

        count = h3d.findNodes(self._particleSystem, '', h3d.NodeTypes.Emitter)
        for i in range(count):
            h3d.updateEmitter(h3d.getNodeFindResult(i), 1.0/cur_fps)


AddNewScene(Scene())

# def Box() : ChangeScene('Box')


