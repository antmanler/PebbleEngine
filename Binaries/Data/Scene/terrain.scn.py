import horde3d as h3d

class Scene(SceneBase):

    def __init__(self) :
        SceneBase.__init__(self, 'Terrain')

    def load_res(self) :
        
        class H3DRes: pass
            
        h3dres = H3DRes()
        self._h3dres = h3dres
        # Pipelines
       # Pipelines
        h3dres.forwardPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0)
        # Overlays
        h3dres.fontMat = h3d.addResource(h3d.ResTypes.Material, "overlays/font.material.xml", 0)
        h3dres.panelMat = h3d.addResource(h3d.ResTypes.Material, "overlays/panel.material.xml", 0)
        h3dres.logoMat = h3d.addResource(h3d.ResTypes.Material, "overlays/logo.material.xml", 0)
        # Terrain
        h3dres.terrain = h3d.addResource(h3d.ResTypes.SceneGraph, "terrains/terrain1/terrain1.scene.xml", 0)
        h3dres.matRes = h3d.findResource(h3d.ResTypes.Material, "terrains/terrain1/terrain1.material.xml" );
        if not h3d.utils.loadResourcesFromDisk(DataDir + "|../../Data/Terrain"):
            print 'loading of some resources failed: See Horde3D_Log.html'

    def setup_scene(self) :
        h3dres = self._h3dres
        # Add terrain
        self.terrain = h3d.addNodes(h3d.RootNode, h3dres.terrain)
        # Set sun direction for ambient pass
        h3d.setMaterialUniform(h3dres.matRes, "sunDir", 1, -1, 0, 0 );
        self.add_camera("MainCamera", h3dres.forwardPipe)
        h3d.setNodeTransform(self.camera, 512.0, 120.0, 512.0, 0, 2250.0, 0, 1, 1, 1)
            
AddNewScene(Scene())

# def Terrain() : ChangeScene('Terrain')

