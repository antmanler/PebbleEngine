# *************************************************************************************************
#
# Horde3D
#   Next-Generation Graphics Engine
# --------------------------------------
# Copyright (C) 2006-2009 Nicolas Schulz
#               2008-2010 Florian Noeding (Python wrapper)
#               2009      Alex Fuller (updates for SVN 331)
#
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# *************************************************************************************************

# Horde3D version beta 5

from ctypes import *
import sys

if not 'c_bool' in globals():
	c_bool = c_int


__all__ = []

__platform = sys.platform

try:
    import utils
    __all__.append('utils')
except:
    pass

try:
    import terrain
    __all__.append('terrain')
except:
    pass

try:
    import sound
    __all__.append('sound')
except:
    pass
    
try:
    import sph
    __all__.append('sph')
except:    
    pass


try:
    if __platform == 'darwin' :
        h3d = cdll.LoadLibrary('libHorde3D.dylib')
    elif __platform[:3] == 'win' :
        h3d = cdll.LoadLibrary('Horde3D.dll')
    else:
        h3d = cdll.LoadLibrary('libHorde3D.so')
except OSError:
    print '---------------------------------------------------'
    print '---            Horde3D Python Wrapper           ---'
    print '---                                             ---'
    print '--- could not find libHorde3D.so or Horde3D.dll ---'
    print '--- check the search path for library files!    ---'
    print '--- or modify horde3d/__init__.py               ---'
    print '---------------------------------------------------'
    raise


RootNode = 1
__all__.append('RootNode')


class Options(object):
	__doc__ = '''The available engine option parameters.
	
			MaxLogLevel         - Defines the maximum log level; only messages which are smaller or equal to this value
			                      (hence more important) are published in the message queue. (Default: 4)
			MaxNumMessages      - Defines the maximum number of messages that can be stored in the message queue (Default: 512)
			TrilinearFiltering  - Enables or disables trilinear filtering for textures. (Values: 0, 1; Default: 1)
			MaxAnisotropy       - Sets the maximum quality for anisotropic filtering. (Values: 1, 2, 4, 8; Default: 1)
			TexCompression      - Enables or disables texture compression; only affects textures that are
			                      loaded after setting the option. (Values: 0, 1; Default: 0)
			SRGBLinearization   - Eanbles or disables gamma-to-linear-space conversion of input textures that are tagged as sRGB (Values: 0, 1; Default: 0)
			LoadTextures        - Enables or disables loading of textures referenced by materials; this can be useful to reduce
			                      loading times for testing. (Values: 0, 1; Default: 1)
			FastAnimation       - Disables or enables inter-frame interpolation for animations. (Values: 0, 1; Default: 1)
			ShadowMapSize       - Sets the size of the shadow map buffer (Values: 128, 256, 512, 1024, 2048; Default: 1024)
			SampleCount         - Maximum number of samples used for multisampled render targets; only affects pipelines
			                      that are loaded after setting the option. (Values: 0, 2, 4, 8, 16; Default: 0)
			WireframeMode       - Enables or disables wireframe rendering
			DebugViewMode       - Enables or disables debug view where geometry is rendered in wireframe without shaders and
			                      lights are visualized using their screen space bounding box. (Values: 0, 1; Default: 0)
			DumpFailedShaders   - Enables or disables storing of shader code that failed to compile in a text file; this can be
			                      useful in combination with the line numbers given back by the shader compiler. (Values: 0, 1; Default: 0)
			GatherTimeStats     - Enables or disables gathering of time stats that are useful for profiling (Values: 0, 1; Default: 1)
	'''
	MaxLogLevel = 1
	MaxNumMessages = 2
	TrilinearFiltering = 3
	MaxAnisotropy = 4
	TexCompression = 5
	SRGBLinearization = 6
	LoadTextures = 7
	FastAnimation = 8
	ShadowMapSize = 9
	SampleCount = 10
	WireframeMode = 11
	DebugViewMode = 12
	DumpFailedShaders = 13
	GatherTimeStats = 14
__all__.append('Options')


class Stats(object):
	__doc__ = '''The available engine statistic parameters.

			TriCount          - Number of triangles that were pushed to the renderer
			BatchCount        - Number of batches (draw calls)
			LightPassCount    - Number of lighting passes
			FrameTime         - Time in ms between two h3dFinalizeFrame calls
			AnimationTime     - CPU time in ms spent for animation
			GeoUpdateTime     - CPU time in ms spent for software skinning and morphing
			ParticleSimTime   - CPU time in ms spent for particle simulation and updates
			FwdLightsGPUTime  - GPU time in ms spent for forward lighting passes
			DefLightsGPUTime  - GPU time in ms spent for drawing deferred light volumes
			ShadowsGPUTime    - GPU time in ms spent for generating shadow maps
			ParticleGPUTime   - GPU time in ms spent for drawing particles
			TextureVMem       - Estimated amount of video memory used by textures (in Mb)
			GeometryVMem      - Estimated amount of video memory used by geometry (in Mb)
	
	'''
	TriCount = 100
	BatchCount = 101
	LightPassCount = 102
	FrameTime = 103
	AnimationTime = 104
	GeoUpdateTime = 105
	ParticleSimTime = 106
	FwdLightsGPUTime = 107
	DefLightsGPUTime = 108
	ShadowsGPUTime = 109
	ParticleGPUTime = 110
	TextureVMem = 111
	GeometryVMem = 112
__all__.append('Stats')


class ResTypes(object):
	__doc__ = '''The available resource types.

			Undefined       - An undefined resource, returned by getResourceType in case of error
			SceneGraph      - Scene graph subtree stored in XML format
			Geometry        - Geometrical data containing bones, vertices and triangles
			Animation       - Animation data
			Material        - Material script
			Code            - Text block containing shader source code
			Shader          - Shader program
			Texture         - Texture map
			ParticleEffect  - Particle configuration
			Pipeline        - Rendering pipeline
	'''
	Undefined = 0
	SceneGraph = 1
	Geometry = 2
	Animation = 3
	Material = 4
	Code = 5
	Shader = 6
	Texture = 7
	ParticleEffect = 8
	Pipeline = 9
__all__.append('ResTypes')


class ResFlags(object):
	__doc__ = '''The available flags used when adding a resource.

			NoQuery           - Excludes resource from being listed by queryUnloadedResource function.
			NoTexCompression  - Disables texture compression for Texture resource.
			NoTexMipmaps      - Disables generation of mipmaps for Texture resource.
			TexCubemap        - Sets Texture resource to be a cubemap.
			TexDynamic        - Enables more efficient updates of Texture resource streams.
			TexRenderable     - Makes Texture resource usable as render target.
			TexSRGB           - Indicates that Texture resource is in sRGB color space and should be converted
			                    to linear space when being sampled.
	'''
	NoQuery = 1
	NoTexCompression = 2
	NoTexMipmaps = 4
	TexCubemap = 8
	TexDynamic = 16
	TexRenderable = 32
	TexSRGB = 64
__all__.append('ResFlags')


class Formats(object):
	__doc__ = '''The available resource stream formats.

			Unknown      - Unknown format
			TEX_BGRA8    - 8-bit BGRA texture
			TEX_DXT1     - DXT1 compressed texture
			TEX_DXT3     - DXT3 compressed texture
			TEX_DXT5     - DXT5 compressed texture
			TEX_RGBA16F  - Half float RGBA texture
			TEX_RGBA32F  - Float RGBA texture
	'''
	Unknown = 0
	TEX_BGRA8 = 1
	TEX_DXT1 = 2
	TEX_DXT3 = 3
	TEX_DXT5 = 4
	TEX_RGBA16F = 5
	TEX_RGBA32F = 6
__all__.append('Formats')


class GeoRes(object):
	__doc__ = '''The available Geometry resource accessors.

			GeometryElem         - Base element
			GeoIndexCountI       - Number of indices [read-only]
			GeoVertexCountI      - Number of vertices [read-only]
			GeoIndices16I        - Flag indicating whether index data is 16 or 32 bit [read-only]
			GeoIndexStream       - Triangle index data (uint16 or uint32, depending on flag)
			GeoVertPosStream     - Vertex position data (float x, y, z)
			GeoVertTanStream     - Vertex tangent frame data (float nx, ny, nz, tx, ty, tz, tw)
			GeoVertStaticStream  - Vertex static attribute data (float u0, v0,
			                         float4 jointIndices, float4 jointWeights, float u1, v1)
	'''
	GeometryElem = 200
	GeoIndexCountI = 201
	GeoVertexCountI = 202
	GeoIndices16I = 203
	GeoIndexStream = 204
	GeoVertPosStream = 205
	GeoVertTanStream = 206
	GeoVertStaticStream = 207
__all__.append('GeoRes')


class AnimRes(object):
	__doc__ = '''The available Animation resource accessors.	  

			EntityElem      - Stored animation entities (joints and meshes)
			EntFrameCountI  - Number of frames stored for a specific entity [read-only]
	'''
	EntityElem = 300
	EntFrameCountI = 301
__all__.append('AnimRes')


class MatRes(object):
	__doc__ = '''The available Material resource accessors.

			MaterialElem  - Base element
			SamplerElem   - Sampler element
			UniformElem   - Uniform element
			MatClassStr   - Material class
			MatLinkI      - Material resource that is linked to this material
			MatShaderI    - Shader resource
			SampNameStr   - Name of sampler [read-only]
			SampTexResI   - Texture resource bound to sampler
			UnifNameStr   - Name of uniform [read-only]
			UnifValueF4   - Value of uniform (a, b, c, d)
	'''
	MaterialElem = 400
	SamplerElem = 401
	UniformElem = 402
	MatClassStr = 403
	MatLinkI = 404
	MatShaderI = 405
	SampNameStr = 406
	SampTexResI = 407
	UnifNameStr = 408
	UnifValueF4 = 409
__all__.append('MatRes')


class ShaderRes(object):
	__doc__  = '''The available Shader resource accessors.

			ContextElem     - Context element 
			SamplerElem     - Sampler element
			UniformElem     - Uniform element
			ContNameStr     - Name of context [read-only]
			SampNameStr     - Name of sampler [read-only]
			SampDefTexResI  - Default texture resouce of sampler [read-only]
			UnifNameStr     - Name of uniform [read-only]
			UnifSizeI       - Size (number of components) of uniform [read-only]
			UnifDefValueF4  - Default value of uniform (a, b, c, d)
	'''
	ContextElem = 600
	SamplerElem = 601
	UniformElem = 602
	ContNameStr = 603
	SampNameStr = 604
	SampDefTexResI = 605
	UnifNameStr = 606
	UnifSizeI = 607
	UnifDefValueF4 = 608
__all__.append('ShaderRes')


class TexRes(object):
	__doc__ = '''The available Texture resource accessors.

			TextureElem     - Base element
			ImageElem       - Subresources of the texture. A texture consists, depending on the type,
			                  of a number of equally sized slices which again can have a fixed number
			                  of mipmaps. Each image element represents the base image of a slice or
			                  a single mipmap level of the corresponding slice.
			TexFormatI      - Texture format [read-only]
			TexSliceCountI  - Number of slices (1 for 2D texture and 6 for cubemap) [read-only]
			ImgWidthI       - Image width [read-only]
			ImgHeightI      - Image height [read-only]
			ImgPixelStream  - Pixel data of an image. The data layout matches the layout specified
			                  by the texture format with the exception that half-float is converted
			                  to float. The first element in the data array corresponds to the lower
			                  left corner.
	
	'''
	TextureElem = 700
	ImageElem = 701
	TexFormatI = 702
	TexSliceCountI = 703
	ImgWidthI = 704
	ImgHeightI = 705
	ImgPixelStream = 706
__all__.append('TexRes')


class PartEffRes(object):
	__doc__ = '''The available ParticleEffect resource accessors.

			ParticleElem     - General particle configuration
			ChanMoveVelElem  - Velocity channel
			ChanRotVelElem   - Angular velocity channel
			ChanSizeElem     - Size channel
			ChanColRElem     - Red color component channel
			ChanColGElem     - Green color component channel
			ChanColBElem     - Blue color component channel
			ChanColAElem     - Alpha channel
			PartLifeMinF     - Minimum value of random life time (in seconds)
			PartLifeMaxF     - Maximum value of random life time (in seconds)
			ChanStartMinF    - Minimum for selecting initial random value of channel
			ChanStartMaxF    - Maximum for selecting initial random value of channel
			ChanEndRateF     - Remaining percentage of initial value when particle is dying
	'''
	ParticleElem = 800
	ChanMoveVelElem = 801
	ChanRotVelElem = 802
	ChanSizeElem = 803
	ChanColRElem = 804
	ChanColGElem = 805
	ChanColBElem = 806
	ChanCalAElem = 807
	PartLifeMinF = 808
	PartLifeMaxF = 809
	ChanStartMinF = 810
	ChanStartMaxF = 811
	ChanEndRateF = 812
__all__.append('PartEffRes')


class PipeRes(object):
	__doc__ = '''The available Pipeline resource accessors.

			StageElem         - Pipeline stage
			StageNameStr      - Name of stage [read-only]
			StageActivationI  - Flag indicating whether stage is active
		Antmanler patched:
            TargetElem        - Pipeline render target
            TargetNameStr     - Name of render target [read-only]
            TargetColBufNumI  - Number of color buffer the target has
            TargetWidhtI      - Width of target
            TargetHeightI     - Height of target
            TargetHasDepthI   - Flag indicating whether target has depth buffer
            TargetScaleF      - Scale of target [read-only]
	'''
	StageElem = 900
	StageNameStr = 901
	StageActivationI = 902
	TargetElem = 903
	TargetNameStr = 904
	TargetColBufNumI = 905
	TargetWidthI = 906
	TargetHeightI = 907
	TargetHasDepthI = 908
	TargetScaleF = 909
__all__.append('PipeRes')


class NodeTypes(object):
	__doc__ = '''The available scene node types.

			Undefined  - An undefined node type, returned by getNodeType in case of error
			Group      - Group of different scene nodes
			Model      - 3D model with optional skeleton
			Mesh       - Subgroup of a model with triangles of one material
			Joint      - Joint for skeletal animation
			Light      - Light source
			Camera     - Camera giving view on scene
			Emitter    - Particle system emitter
	'''
	Undefined = 0
	Group = 1
	Model = 2
	Mesh = 3
	Joint = 4
	Light = 5
	Camera = 6
	Emitter = 7
__all__.append('NodeTypes')


class NodeFlags(object):
	__doc__ = '''The available scene node flags.

			NoDraw         - Excludes scene node from all rendering
			NoCastShadow   - Excludes scene node from list of shadow casters
			NoRayQuery     - Excludes scene node from ray intersection queries
			Inactive       - Deactivates scene node so that it is completely ignored
			                 (combination of all flags above)
	'''
	NoDraw = 1
	NoCastShadow = 2
	NoRayQuery = 4
	Inactive = 7   #NoDraw | NoCastShadow | NoRayQuery
__all__.append('NodeFlags')


class NodeParams(object):
	__doc__ = '''The available scene node parameters.

			NameStr        - Name of the scene node
			AttachmentStr  - Optional application-specific meta data for a node encapsulated
			                 in an 'Attachment' XML string
			TagStr         - Optional user defined short string
	'''
	NameStr = 1
	AttachmentStr = 2
	TagStr = 3
__all__.append('NodeParams')


class Model(object):
	__doc__ = '''The available Model node parameters

			GeoResI      - Geometry resource used for the model
			SWSkinningI  - Enables or disables software skinning (default: 0)
			LodDist1F    - Distance to camera from which on LOD1 is used (default: infinite)
			               (must be a positive value larger than 0.0)
			LodDist2F    - Distance to camera from which on LOD2 is used
			               (may not be smaller than LodDist1) (default: infinite)
			LodDist3F    - Distance to camera from which on LOD3 is used
			               (may not be smaller than LodDist2) (default: infinite)
			LodDist4F    - Distance to camera from which on LOD4 is used
			               (may not be smaller than LodDist3) (default: infinite)
	'''
	GeoResI = 200
	SWSkinningI = 201
	LodDist1F = 202
	LodDist2F = 203
	LodDist3F = 204
	LodDist4F = 205
__all__.append('Model')


class Mesh(object):
	__doc__ = '''The available Mesh node parameters.

			MatResI      - Material resource used for the mesh
			BatchStartI  - First triangle index of mesh in Geometry resource of parent Model node [read-only]
			BatchCountI  - Number of triangle indices used for drawing mesh [read-only]
			VertRStartI  - First vertex in Geometry resource of parent Model node [read-only]
			VertREndI    - Last vertex in Geometry resource of parent Model node [read-only]
			LodLevelI    - LOD level of Mesh; the mesh is only rendered if its LOD level corresponds to
			               the model's current LOD level which is calculated based on the LOD distances (default: 0)
	'''
	MatResI = 300
	BatchStartI = 301
	BatchCountI = 302
	VertRStartI = 303
	VertREndI = 304
	LodLevelI = 305
__all__.append('Mesh')


class Joint(object):
	__doc__ = '''The available Joint node parameters.

			JointIndexI  - Index of joint in Geometry resource of parent Model node [read-only]
	'''
	JointIndexI = 400
__all__.append('Joint')


class Light(object):
	__doc__ = '''The available Light node parameters.

			MatResI             - Material resource used for the light
			RadiusF             - Radius of influence (default: 100.0)
			FovF                - Field of view (FOV) angle (default: 90.0)
			ColorF3             - Diffuse color RGB (default: 1.0, 1.0, 1.0)
			ColorMultiplierF    - Diffuse color multiplier for altering intensity, mainly useful for HDR (default: 1.0)
			ShadowMapCountI     - Number of shadow maps used for light source (values: 0, 1, 2, 3, 4; default: 0)]
			ShadowSplitLambdaF  - Constant determining segmentation of view frustum for Parallel Split Shadow Maps (default: 0.5)
			ShadowMapBiasF      - Bias value for shadow mapping to reduce shadow acne (default: 0.005)
			LightingContextStr  - Name of shader context used for computing lighting
			ShadowContextStr    - Name of shader context used for generating shadow map
	'''
	MatResI = 500
	RadiusF = 501
	FovF = 502
	ColorF3 = 503
	ColorMultiplierF = 504
	ShadowMapCountI = 505
	ShadowSplitLambdaF = 506
	ShadowMapBiasF = 507
	LightingContextStr = 508
	ShadowContextStr = 509
__all__.append('Light')


class Camera(object):
	__doc__ = '''The available Camera node parameters.

			PipeResI         - Pipeline resource used for rendering
			OutTexResI       - 2D Texture resource used as output buffer (can be 0 to use main framebuffer) (default: 0)
			OutBufIndexI     - Index of the output buffer for stereo rendering (values: 0 for left eye, 1 for right eye) (default: 0)
			LeftPlaneF       - Coordinate of left plane relative to near plane center (default: -0.055228457)
			RightPlaneF      - Coordinate of right plane relative to near plane center (default: 0.055228457)
			BottomPlaneF     - Coordinate of bottom plane relative to near plane center (default: -0.041421354f)
			TopPlaneF        - Coordinate of top plane relative to near plane center (default: 0.041421354f)
			NearPlaneF       - Distance of near clipping plane (default: 0.1)
			FarPlaneF        - Distance of far clipping plane (default: 1000)
			ViewportXI       - Position x-coordinate of the lower left corner of the viewport rectangle (default: 0)
			ViewportYI       - Position y-coordinate of the lower left corner of the viewport rectangle (default: 0)
			ViewportWidthI   - Width of the viewport rectangle (default: 320)
			ViewportHeightI  - Height of the viewport rectangle (default: 240)
			OrthoI           - Flag for setting up an orthographic frustum instead of a perspective one (default: 0)
			OccCullingI      - Flag for enabling occlusion culling (default: 0)
	'''
	PipeResI = 600
	OutTexResI = 601
	OutBufIndexI = 602
	LeftPlaneF = 603
	RightPlaneF = 604
	BottomPlaneF = 605
	TopPlaneF = 606
	NearPlaneF = 607
	FarPlaneF = 608
	ViewportXI = 609
	ViewportYI = 610
	ViewportWidthI = 611
	ViewportHeightI = 612
	OrthoI = 613
	OccCullingI = 614
__all__.append('Camera')


class Emitter(object):
	__doc__ = '''The available Emitter node parameters.

			MatResI        - Material resource used for rendering
			PartEffResI    - ParticleEffect resource which configures particle properties
			MaxCountI      - Maximal number of particles living at the same time
			RespawnCountI  - Number of times a single particle is recreated after dying (-1 for infinite)
			DelayF         - Time in seconds before emitter begins creating particles (default: 0.0)
			EmissionRateF  - Maximal number of particles to be created per second (default: 0.0)
			SpreadAngleF   - Angle of cone for random emission direction (default: 0.0)
			ForceF3        - Force vector XYZ applied to particles (default: 0.0, 0.0, 0.0)
	'''
	MatResI = 700
	PartEffResI = 701
	MaxCountI = 702
	RespawnCountI = 703
	DelayF = 704
	EmissionRateF = 705
	SpreadAngleF = 706
	ForceF3 = 707
__all__.append('Emitter')


class ModelUpdateFlags(object):
	__doc__ = '''Enum: H3DModelUpdateFlags
			The available flags for h3dUpdateModel.
		
			Animation  - Apply animation
			Geometry   - Apply morphers and software skinning
	'''
	Animation = 1
	Geometry = 2
	

getVersionString = h3d.h3dGetVersionString
getVersionString.restype = c_char_p
getVersionString.argtypes = []
__all__.append('getVersionString')


checkExtension = h3d.h3dCheckExtension
checkExtension.restype = c_bool
checkExtension.argtypes = [c_char_p]
__all__.append('checkExtension')


getError = h3d.h3dGetError
getError.restype = c_bool
getError.argtypes = []
__all__.append('getError')


init = h3d.h3dInit
init.restype = c_bool
init.argtypes = []
__all__.append('init')


release = h3d.h3dRelease
release.restype = None
release.argtypes = []
__all__.append('release')


#setupViewport = h3d.h3dSetupViewport
#setupViewport.restype = None
#setupViewport.argtypes = [c_int, c_int, c_int, c_int, c_bool]
#__all__.append('setupViewport')


render = h3d.h3dRender
render.restype = c_bool
render.argtypes = [c_int]
__all__.append('render')


finalizeFrame = h3d.h3dFinalizeFrame
finalizeFrame.restype = None
finalizeFrame.argtypes = []
__all__.append('finalizeFrame')


clear = h3d.h3dClear
clear.restype = None
clear.argtypes = []
__all__.append('clear')


_getMessage = h3d.h3dGetMessage
_getMessage.restype = c_char_p
_getMessage.argtypes = [POINTER(c_int), POINTER(c_float)]
def getMessage():
	i = c_int()
	f = c_float()
	s = _getMessage(byref(i), byref(f))

	return (s, i.value, f.value)
__all__.append('getMessage')


getOption = h3d.h3dGetOption
getOption.restype = c_float
getOption.argtypes = [c_int]
__all__.append('getOption')


_setOption = h3d.h3dSetOption
_setOption.restype = c_bool
_setOption.argtypes = [c_int, c_float]
def setOption(param, value):
	return _setOption(param, c_float(value))
__all__.append('setOption')


getStat = h3d.h3dGetStat
getStat.restype = c_float
getStat.argtypes = [c_int, c_bool]
# def getStat(param, reset):
#   return _getStat(param, reset).value
__all__.append('getStat')


_showOverlays = h3d.h3dShowOverlays
_showOverlays.restype = None
_showOverlays.argtypes = [
		POINTER(c_float), c_int,
		c_float, c_float, c_float, c_float,
		c_int, c_int]
def showOverlays(
		verts,
		r, g, b, a,
		materialRes, flags):

	cVerts = (c_float * len(verts))()
	for i in range(len(verts)):
		cVerts[i] = verts[i]

	return _showOverlays(
			cast(byref(cVerts), POINTER(c_float)), c_int(len(verts) // 4),
			c_float(r), c_float(g), c_float(b), c_float(a),
			materialRes, flags
			)
__all__.append('showOverlays')


clearOverlays = h3d.h3dClearOverlays
clearOverlays.restype = None
clearOverlays.argtypes = []
__all__.append('clearOverlays')


getResType = h3d.h3dGetResType
getResType.restype = c_int
getResType.argtypes = [c_int]
__all__.append('getResType')


getResName = h3d.h3dGetResName
getResName.restype = c_char_p
getResName.argtypes = [c_int]
__all__.append('getResName')


getNextResource = h3d.h3dGetNextResource
getNextResource.restype = c_int
getNextResource.append = [c_int, c_int]
__all__.append('getNextResource')


findResource = h3d.h3dFindResource
findResource.restype = c_int
findResource.argtypes = [c_int, c_char_p]
__all__.append('findResource')


addResource = h3d.h3dAddResource
addResource.restype = c_int
addResource.argtypes = [c_int, c_char_p, c_int]
__all__.append('addResource')


cloneResource = h3d.h3dCloneResource
cloneResource.restype = c_int
cloneResource.argtypes = [c_int, c_char_p]
__all__.append('cloneResource')


removeResource = h3d.h3dRemoveResource
removeResource.restype = c_int
removeResource.argtypes = [c_int]
__all__.append('removeResource')


isResLoaded = h3d.h3dIsResLoaded
isResLoaded.restype = c_bool
isResLoaded.argtypes = [c_int]
__all__.append('isResLoaded')


_loadResource = h3d.h3dLoadResource
_loadResource.restype = c_bool
_loadResource.argtypes = [c_int, c_void_p, c_int]
def loadResource(res, data):
	return _loadResource(res, data, len(data))
__all__.append('loadResource')


unloadResource = h3d.h3dUnloadResource
unloadResource.restype = None
unloadResource.argtypes = [c_int]
__all__.append('unloadResource')


getResElemCount = h3d.h3dGetResElemCount
getResElemCount.restype = c_int
getResElemCount.argtypes = [c_int, c_int]
__all__.append('getResElemCount')


findResElem = h3d.h3dFindResElem
findResElem.restype = c_int
findResElem.argtypes = [c_int, c_int, c_int, c_char_p]
__all__.append('findResElem')


getResParamI = h3d.h3dGetResParamI
getResParamI.restype = c_int
getResParamI.argtypes = [c_int, c_int, c_int, c_int]
__all__.append('getResParamI')


setResParamI = h3d.h3dSetResParamI
setResParamI.restype = None
setResParamI.argtypes = [c_int, c_int, c_int, c_int, c_int]
__all__.append('setResParamI')


_getResParamF = h3d.h3dGetResParamF
_getResParamF.restype = c_float
_getResParamF.argtypes = [c_int, c_int, c_int, c_int, c_int]
def getResParamF(res, param):
	return _getResParamF(res, param).value
__all__.append('getResParamF')


_setResParamF = h3d.h3dSetResParamF
_setResParamF.restype = None
_setResParamF.argtypes = [c_int, c_int, c_int, c_int, c_int, c_float]
def setResParamF(res, elem, elemIdx, param, compIdx, value):
	return _setResParamF(res, elem, elemIdx, param, compIdx, c_float(value))
__all__.append('setResParamF')


getResParamStr = h3d.h3dGetResParamStr
getResParamStr.restype = c_char_p
getResParamStr.argtypes = [c_int, c_int, c_int, c_int]
__all__.append('getResParamStr')


setResParamStr = h3d.h3dSetResParamStr
setResParamStr.restype = c_bool
setResParamStr.argtypes = [c_int, c_int, c_int, c_int, c_char_p]
__all__.append('setResParamStr')


mapResStream = h3d.h3dMapResStream
mapResStream.restype = c_void_p
mapResStream.argtypes = [c_int, c_int, c_int, c_int, c_bool, c_bool]
__all__.append('mapResStream')


unmapResStream = h3d.h3dUnmapResStream
unmapResStream.restype = None
unmapResStream.argtypes = [c_int]
__all__.append('unmapResStream')


queryUnloadedResource = h3d.h3dQueryUnloadedResource
queryUnloadedResource.restype = c_int
queryUnloadedResource.argtypes = [c_int]
__all__.append('queryUnloadedResource')


releaseUnusedResources = h3d.h3dReleaseUnusedResources
releaseUnusedResources.restype = None
releaseUnusedResources.argtypes = []
__all__.append('releaseUnusedResources')


createTexture = h3d.h3dCreateTexture
createTexture.restype = c_int
createTexture.argtypes = [c_char_p, c_int, c_int, c_int, c_int]
__all__.append('createTexture')


setShaderPreambles = h3d.h3dSetShaderPreambles
setShaderPreambles.restype = None
setShaderPreambles.argtypes = [c_char_p, c_char_p]
__all__.append('setShaderPreambles')


_setMaterialUniform = h3d.h3dSetMaterialUniform
_setMaterialUniform.restype = c_bool
_setMaterialUniform.argtypes = [c_int, c_char_p, c_float, c_float, c_float, c_float]
def setMaterialUniform(materialRes, name, a, b, c, d):
	return _setMaterialUniform(materialRes, name, c_float(a), c_float(b), c_float(c), c_float(d))
__all__.append('setMaterialUniform')


_resizePipelineBuffers = h3d.h3dResizePipelineBuffers
_resizePipelineBuffers.restype = None
_resizePipelineBuffers.argtypes = [c_int, c_int, c_int]
def resizePipelineBuffers(pipelineRes, width, height):
	return _resizePipelineBuffers(pipelineRes, width, height)
__all__.append('resizePipelineBuffers')


_getRenderTargetData = h3d.h3dGetRenderTargetData
_getRenderTargetData.restype = c_bool
_getRenderTargetData.argtypes = [c_int, c_char_p, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_float), c_int]
def getRenderTargetData(pipelineRes, targetName, bufIndex):
	width = c_int()
	height = c_int()
	compCount = c_int()

	# get values for width, height, compCount to calculate needed buffer size
	_getRenderTargetData(pipelineRes, targetName, bufIndex, byref(width), byref(height), byref(compCount), None, 0)

	# get data
	bufferSize = 4 * width * height * compCount
	buffer = (c_float * (bufferSize / 4))()
	_getRenderTargetData(pipelineRes, targetName, buffer, None, None, None, buffer)

	return [x for x in buffer] # FIXME really make a normal list of floats out of this array?
__all__.append('getRenderTargetData')


getNodeType = h3d.h3dGetNodeType
getNodeType.restype = c_int
getNodeType.argtypes = [c_int]
__all__.append('getNodeType')


getNodeParent = h3d.h3dGetNodeParent
getNodeParent.restype = c_int
getNodeParent.argtypes = [c_int]
__all__.append('getNodeParent')


setNodeParent = h3d.h3dSetNodeParent
setNodeParent.restype = c_bool
setNodeParent.argtypes = [c_int, c_int]
__all__.append('setNodeParent')


getNodeChild = h3d.h3dGetNodeChild
getNodeChild.restype = c_int
getNodeChild.argtypes = [c_int, c_int]
__all__.append('getNodeChild')


addNodes = h3d.h3dAddNodes
addNodes.restype = c_int
addNodes.argtypes = [c_int, c_int]
__all__.append('addNodes')


removeNode = h3d.h3dRemoveNode
removeNode.restype = None
removeNode.argtypes = [c_int]
__all__.append('removeNode')


checkNodeTransFlag = h3d.h3dCheckNodeTransFlag
checkNodeTransFlag.restype = c_bool
checkNodeTransFlag.argtypes = [c_int, c_bool]
__all__.append('checkNodeTransFlag')


_getNodeTransform = h3d.h3dGetNodeTransform
_getNodeTransform.restype = None
_getNodeTransform.argtypes = [c_int,
		POINTER(c_float), POINTER(c_float), POINTER(c_float),
		POINTER(c_float), POINTER(c_float), POINTER(c_float),
		POINTER(c_float), POINTER(c_float), POINTER(c_float),
		]
def getNodeTransform(node):
	tx, ty, tz = c_float(), c_float(), c_float()
	rx, ry, rz = c_float(), c_float(), c_float()
	sx, sy, sz = c_float(), c_float(), c_float()

	_getNodeTransform(node,
			byref(tx), byref(ty), byref(tz),
			byref(rx), byref(ry), byref(rz),
			byref(sx), byref(sy), byref(sz),
			)

	return [
			[tx.value, ty.value, tz.value],
			[rx.value, ry.value, rz.value],
			[sx.value, sy.value, sz.value]
			]
	__all__.append('getNodeTransform')


_setNodeTransform = h3d.h3dSetNodeTransform
_setNodeTransform.restype = None
_setNodeTransform.argtypes = [c_int,
		c_float, c_float, c_float,
		c_float, c_float, c_float,
		c_float, c_float, c_float,
		]
def setNodeTransform(node, tx, ty, tz, rx, ry, rz, sx, sy, sz):
	_setNodeTransform(node,
			c_float(tx), c_float(ty), c_float(tz),
			c_float(rx), c_float(ry), c_float(rz),
			c_float(sx), c_float(sy), c_float(sz),
			)
	__all__.append('setNodeTransform')


try:
    import numpy as np
    def __cmat16_npmat4x4(cmat16):
        mat = [cmat16[i] for i in range(16)]
        return np.mat(mat, dtype=float).reshape([4,4], order='F')
    def __npmat4x4_cmat16(npmat4x4):
        if type(npmat4x4) is np.matrix :
            m, n = np.shape(npmat4x4)
            if not ((m, n) == (1, 16)) :
                npmat4x4 = np.array(npmat4x4.reshape((1,16), order='F'))[0]
        f16 = (c_float * 16)()
        for i in range(16):
            f16[i] = npmat4x4[i]
        return f16
except ImportError :
    def __cmat16_npmat4x4(cmat16): 
        return [cmat16[i] for i in range(16)]
    def __npmat4x4_cmat16(npmat4x4):
        f16 = (c_float * 16)()
        for i in range(16):
            f16[i] = npmat4x4[i]
        return f16

_getNodeTransMats = h3d.h3dGetNodeTransMats
_getNodeTransMats.restype = None
_getNodeTransMats.argtypes = [c_int, POINTER(POINTER(c_float)), POINTER(POINTER(c_float))]
def getNodeTransMats(node):
	tt = c_float
	rel = pointer(tt())
	abs = pointer(tt())
	_getNodeTransMats(node, byref(rel), byref(abs))

	#rel = [rel[i] for i in range(16)]
	#abs = [abs[i] for i in range(16)]
	return (__cmat16_npmat4x4(rel), __cmat16_npmat4x4(abs))
__all__.append('getNodeTransMats')


_setNodeTransMat = h3d.h3dSetNodeTransMat
_setNodeTransMat.restype = None
_setNodeTransMat.argtypes = [c_int, c_float * 16]
def setNodeTransMat(node, mat):
	#t = c_float * 16
	#f16 = t()
	#for i in range(16):
	#	f16[i] = mat[i]
	_setNodeTransMat(node, __npmat4x4_cmat16(mat))
__all__.append('setNodeTransMat')


getNodeParamI = h3d.h3dGetNodeParamI
getNodeParamI.restype = c_int
getNodeParamI.argtypes = [c_int, c_int]
__all__.append('getNodeParamI')

setNodeParamI = h3d.h3dSetNodeParamI
setNodeParamI.restype = None
setNodeParamI.argtypes = [c_int, c_int, c_int]
__all__.append('setNodeParamI')


_getNodeParamF = h3d.h3dGetNodeParamF
_getNodeParamF.restype = c_float
_getNodeParamF.argtypes = [c_int, c_int, c_int]
def getNodeParamF(node, param, idx):
	return _getNodeParamF(node, param, idx).value
__all__.append('getNodeParamF')


_setNodeParamF = h3d.h3dSetNodeParamF
_setNodeParamF.restype = None
_setNodeParamF.argtypes = [c_int, c_int, c_int, c_float]
def setNodeParamF(node, param, idx, value):
	return _setNodeParamF(node, param, idx, c_float(value))
__all__.append('setNodeParamF')


getNodeParamStr = h3d.h3dGetNodeParamStr
getNodeParamStr.restype = c_char_p
getNodeParamStr.argtypes = [c_int, c_int]
__all__.append('getNodeParamStr')


setNodeParamStr = h3d.h3dSetNodeParamStr
setNodeParamStr.restype = None
setNodeParamStr.argtypes = [c_int, c_int, c_char_p]
__all__.append('setNodeParamStr')


getNodeFlags = h3d.h3dGetNodeFlags
getNodeFlags.restype = c_int
getNodeFlags.argtypes = [c_int]


setNodeFlags = h3d.h3dSetNodeFlags
setNodeFlags.restype = None
setNodeFlags.argtypes = [c_int, c_int, c_bool]


_getNodeAABB = h3d.h3dGetNodeAABB
_getNodeAABB.restype = None
_getNodeAABB.argtypes = [c_int,
		POINTER(c_float), POINTER(c_float), POINTER(c_float),
		POINTER(c_float), POINTER(c_float), POINTER(c_float),
		]
def getNodeAABB(node):
	minX, minY, minZ = c_float(), c_float(), c_float()
	maxX, maxY, maxZ = c_float(), c_float(), c_float()

	_getNodeAABB(node,
			byref(minX), byref(minY), byref(minZ),
			byref(maxX), byref(maxY), byref(maxZ),
			)
	return [[minX.value, minY.value, minZ.value], [maxX.value, maxY.value, maxZ.value]]
__all__.append('getNodeAABB')


findNodes = h3d.h3dFindNodes
findNodes.restype = c_int
findNodes.argtypes = [c_int, c_char_p, c_int]
__all__.append('findNodes')


getNodeFindResult = h3d.h3dGetNodeFindResult
getNodeFindResult.restype = c_int
getNodeFindResult.argtypes = [c_int]
__all__.append('getNodeFindResult')


#   Function: h3dSetNodeUniforms
#   Sets per-instance uniform data for a node.
# 
# Details:
#   This function sets the custom per-instance uniform data for a node that can be accessed
#   from within a shader. The specified number of floats is copied from the specified memory location.
#   Currently only Model nodes will store this data.
# 
# Parameters:
#   node         - node for which data will be set
#   uniformData  - pointer to float array
#   count        - number of floats to be copied
#   
# Returns:
#   nothing
_setNodeUniforms = h3d.h3dSetNodeUniforms
_setNodeUniforms.restype = None
_setNodeUniforms.argtypes = [c_int, POINTER(c_float), c_int]
def setNodeUniforms(node, uniformData):
    _len = getattr(uniformData, "__len__", None)
    if _len is None : uniformData = [uniformData]
    _len = len(uniformData)
    floats = (c_float * _len)()
    for i in range(_len): floats[i] = uniformData[i]
    _setNodeUniforms(node, floats, _len)
__all__.append('setNodeUniforms')


_castRay = h3d.h3dCastRay
_castRay.restype = c_int
_castRay.argtypes = [c_int,
		c_float, c_float, c_float,
		c_float, c_float, c_float,
		c_int]
def castRay(node, ox, oy, oz, dx, dy, dz, numNearest):
	return _castRay(node,
			c_float(ox), c_float(oy), c_float(oz),
			c_float(dx), c_float(dy), c_float(dz),
			numNearest)
	__all__.append('castRay')


_getCastRayResult = h3d.h3dGetCastRayResult
_getCastRayResult.restype = c_bool
_getCastRayResult.argtypes = [c_int, POINTER(c_int), POINTER(c_float), POINTER(c_float * 3)]
def getCastRayResult(index):
	i = c_int()
	f = c_float()
	f3 = (c_float * 3)()

	# FIXME really assert? throw exception?
	assert(_getCastRayResult(index, byref(i), byref(f), byref(f3)))

	return [i.value, f.value, tuple([x for x in f3])]
__all__.append('getCastRayResult')


checkNodeVisibility = h3d.h3dCheckNodeVisibility
checkNodeVisibility.restype = c_int
checkNodeVisibility.argtypes = [c_int, c_int, c_bool, c_bool]
__all__.append('checkNodeVisibility')


addGroupNode = h3d.h3dAddGroupNode
addGroupNode.restype = c_int
addGroupNode.argtypes = [c_int, c_char_p]
__all__.append('addGroupNode')


addModelNode = h3d.h3dAddModelNode
addModelNode.restype = c_int
addModelNode.argtypes = [c_int, c_char_p, c_int]
__all__.append('addModelNode')


setupModelAnimStage = h3d.h3dSetupModelAnimStage
setupModelAnimStage.restype = None
setupModelAnimStage.argtypes = [c_int, c_int, c_int, c_int, c_char_p, c_bool]
__all__.append('setupModelAnimStage')


_setModelAnimParams = h3d.h3dSetModelAnimParams
_setModelAnimParams.restype = None
_setModelAnimParams.argtypes = [c_int, c_int, c_float, c_float]
def setModelAnimParams(modelNode, stage, time, weight):
	_setModelAnimParams(modelNode, stage, c_float(time), c_float(weight))
__all__.append('setModelAnimParams')



_setModelMorper = h3d.h3dSetModelMorpher
_setModelMorper.restype = c_bool
_setModelMorper.argtypes = [c_int, c_char_p, c_float]
def setModelMorpher(modelNode, target, weight):
	return _setModelMorper(modelNode, target, c_float(weight))
__all__.append('setModelMorpher')


#   Function: h3dUpdateModel
#   Applies animation and/or geometry updates.
# 
# Details:
#   This function applies skeletal animation and geometry updates to the specified model, depending on
#   the specified update flags. Geometry updates include morph targets and software skinning if enabled.
#   If the animation or morpher parameters did not change, the function returns immediately. This function
#   has to be called so that changed animation or morpher parameters will take effect.
# 
# Parameters:
#   modelNode  - handle to the Model node to be updated
#   flags      - combination of H3DModelUpdate flags
#   
# Returns:
#   nothing

updateModel = h3d.h3dUpdateModel
updateModel.restype = None
updateModel.argtypes = [c_int, c_int]
__all__.append('updateModel')

addMeshNode = h3d.h3dAddMeshNode
addMeshNode.restype = c_int
addMeshNode.argtypes = [c_int, c_char_p, c_int, c_int, c_int, c_int, c_int]
__all__.append('addMeshNode')


addJointNode = h3d.h3dAddJointNode
addJointNode.restype = c_int
addJointNode.argtypes = [c_int, c_char_p, c_int]
__all__.append('addJointNode')


addLightNode = h3d.h3dAddLightNode
addLightNode.restype = c_int
addLightNode.argtypes = [c_int, c_char_p, c_int, c_char_p, c_char_p]
__all__.append('addLightNode')


addCameraNode = h3d.h3dAddCameraNode
addCameraNode.restype = c_int
addCameraNode.argtypes = [c_int, c_char_p, c_int]
__all__.append('addCameraNode')


_setupCameraView = h3d.h3dSetupCameraView
_setupCameraView.restype = None
_setupCameraView.argtypes = [c_int, c_float, c_float, c_float, c_float]
def setupCameraView(cameraNode, fov, aspect, nearDist, farDist):
	_setupCameraView(cameraNode, c_float(fov), c_float(aspect), c_float(nearDist), c_float(farDist))
__all__.append('setupCameraView')


_getCameraProjMat = h3d.h3dGetCameraProjMat
_getCameraProjMat.restype = None
_getCameraProjMat.argtypes = [c_int, POINTER(c_float * 16)]
def getCameraProjMat(node):
	buffer = (c_float * 16)()
	_getCameraProjMat(node, byref(buffer))
        return __cmat16_npmat4x4(buffer)
	#return [x for x in buffer]
__all__.append('getCameraProjMat')


addEmitterNode = h3d.h3dAddEmitterNode
addEmitterNode.restype = c_int
addEmitterNode.argtypes = [c_int, c_char_p, c_int, c_int, c_int, c_int]
__all__.append('addEmitterNode')


_updateEmitter = h3d.h3dUpdateEmitter
_updateEmitter.restype = None
_updateEmitter.argtypes = [c_int, c_float]
def updateEmitter(emitterNode, timeDelta):
	_updateEmitter(emitterNode, c_float(timeDelta))
__all__.append('updateEmitter')


hasEmitterFinished = h3d.h3dHasEmitterFinished
hasEmitterFinished.restype = c_bool
hasEmitterFinished.argtypes = [c_int]
__all__.append('hasEmitterFinished')



