import horde3d as h3d
from math import sin, cos, radians, degrees, sqrt, atan2
from random import random, seed

class Scene(SceneBase):

    def __init__(self) :
        SceneBase.__init__(self, 'Chicago')
        self.__mode = 0

    def load_res(self) :
        
        class H3DRes: pass
            
        h3dres = H3DRes()
        self._h3dres = h3dres
        # Pipelines
        h3dres.forwardPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/forward.pipeline.xml", 0)
        h3dres.deferredPipe = h3d.addResource(h3d.ResTypes.Pipeline, "pipelines/deferred.pipeline.xml", 0)
        # Overlays
        h3dres.fontMat = h3d.addResource(h3d.ResTypes.Material, "overlays/font.material.xml", 0)
        h3dres.panelMat = h3d.addResource(h3d.ResTypes.Material, "overlays/panel.material.xml", 0)
        h3dres.logoMat = h3d.addResource(h3d.ResTypes.Material, "overlays/logo.material.xml", 0)
        # Shader for deferred shading
        h3dres.lightMat = h3d.addResource(h3d.ResTypes.Material, "materials/light.material.xml", 0)
        # Environment
        h3dres.env = h3d.addResource(h3d.ResTypes.SceneGraph, "models/platform/platform.scene.xml", 0)
        # Skybox
        h3dres.skyBox = h3d.addResource(h3d.ResTypes.SceneGraph, "models/skybox/skybox_desert.scene.xml", 0)
        # Load character animations
        self._crowdSim = CrowdSim()
        if not h3d.utils.loadResourcesFromDisk(DataDir):
            print 'loading of some resources failed: See Horde3D_Log.html'

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
        # Init character positions
        self._crowdSim.init()
        self.add_camera("MainCamera", h3dres.deferredPipe)
        h3d.setNodeTransform(self.camera, 15, 3.3, -16, 0, 130, 0, 1, 1, 1)

            
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
        self._crowdSim.update(1.0/cur_fps)


class CrowdSim(object):    
    def __init__(self):
        # Load character with walk animation
        self.character = h3d.addResource(h3d.ResTypes.SceneGraph, "models/man/man.scene.xml", 0)
        self.characterWalk = h3d.addResource(h3d.ResTypes.Animation, "animations/man.anim", 0)
        self.particles = []
        self.setRange(32)

    def add(self, n):
        # Add characters
        self.particles += [Particle(self, 1.0 * i / n) for i in xrange(n)]
        self.findAllNeighbours()

    def findAllNeighbours(self):
        # find the neighbours of each particle
        self._next = 0
        for p in self.particles:
            p.findNeigbours(self.particles, self._range)

    def init(self):
        # Init random generator
        # Use fixed value for better performance comparisons
        seed(99777)
        self.add(50)
        self.findAllNeighbours()
        # print "Find!!"

    def remove(self, n):
        # remove characters
        size = max(1, len(self.particles) - n)
        for p in self.particles[size:]:
            p.remove()
        self.particles = self.particles[:size]
        self.findAllNeighbours()
        
    def setRange(self, range):
        # set the neighbourhood range
        self._range = range
        self.findAllNeighbours()
        
    def update(self, dt):
        # Find new neighbours for one particle
        self.particles[self._next].findNeigbours(self.particles, self._range)
        self._next = (self._next + 1) % len(self.particles)
        # Update all particles
        for p in self.particles: 
            p.update(dt)
            

# Parameters for three repulsion zones
d1 = 0.25 
d2 = 2.0 
d3 = 4.5
dms = d3 * d3

f1 = 3.0
f2 = 1.0
f3 = 0.1

m1 = (f1 - f2) / (0 - d1)
m2 = (f2 - f3) / (d1 - d2)
m3 = (f3 - 0) / (d2 - d3)

t1 = f2 - m1 * d1
t2 = f3 - m2 * d2
t3 = 0 - m3 * d3

class Particle(object):
    animTime = 0
    # Orientation vector
    ox = oz = 0
    # Force on particle
    fx = fy = 0
    # Destination position
    dx = dz = 0
    # Current postition
    px = pz = 0

    def __init__(self, parent, a):
        # Add character to scene and apply animation
        self.node = h3d.addNodes(h3d.RootNode, parent.character)
        h3d.setupModelAnimStage(self.node, 0, parent.characterWalk, 0, "", False)
        # print "create 1"
        # Characters start in a circle formation
        self.px = sin(a * 6.28) * 10.0
        self.pz = cos(a * 6.28) * 10.0
        self.chooseDestination()
        h3d.setNodeTransform(self.node, self.px, 0.02, self.pz, 0, 0, 0, 1, 1, 1)

    def chooseDestination(self):
        # Choose random destination within a circle
        ang = random() * 6.28
        rad = random() * 20.0
        self.dx = sin(ang) * rad
        self.dz = cos(ang) * rad

    def findNeigbours(self, particles, range):
        # find all particles in range
        range *= range
        self.near = []
        for p in particles:
            if p != self:
                x = p.px - self.px
                z = p.pz - self.pz
                if (x * x + z * z) < range:
                    self.near.append(p)
                    
    def remove(self):
        h3d.removeNode(self.node)
        
    def update(self, dt):
        # Calculate distance to destination
        x = self.dx - self.px
        z = self.dz - self.pz
        d = sqrt(x * x + z * z)

        # On arrival choose a new destination for the next step
        if d <= 3.0:
            self.chooseDestination()
            x = self.dx - self.px
            z = self.dz - self.pz
            d = sqrt(x * x + z * z)

        # Calculate normalized attraction force to destination
        self.fx = x * 0.035 / d
        self.fz = z * 0.035 / d

        # Repulsion forces from other nearby particles
        for p in self.near:
            if p == self:
                continue
            x = self.px - p.px
            z = self.pz - p.pz
            d = x * x + z * z
            if d > dms:
                continue
            d = sqrt(d)
            # Use three zones with different repulsion strengths
            if d <= d1:
                s = m1 + t1 / d
            elif d <= d2:
                s = m2 + t2 / d
            else:
                s = m3 + t3 / d
            self.fx += x * s 
            self.fz += z * s
        
        # Make movement frame rate independent
        dt *= 30
        self.fx *= dt
        self.fz *= dt
        
        # Set new position
        self.px += self.fx 
        self.pz += self.fz
        
        # Calculate orientation
        self.ox = (self.ox + self.fx) / 2
        self.oz = (self.oz + self.fz) / 2

        # Get rotation from orientation
        if self.oz == 0:
            ry = 0
        else:
            ry = degrees(atan2(self.ox, self.oz))
        
        # Update character scene node position
        h3d.setNodeTransform(self.node, self.px, 0.02, self.pz, 0, ry, 0, 1, 1, 1)
        
        # Update animation
        vel = sqrt(self.fx * self.fx + self.fz * self.fz)
        self.animTime += vel * 35.0
        h3d.setModelAnimParams(self.node, 0, self.animTime, 1.0)
        h3d.updateModel( self.node, h3d.ModelUpdateFlags.Animation | h3d.ModelUpdateFlags.Geometry );



AddNewScene(Scene())
# def Chicago() : ChangeScene('Chicago')
