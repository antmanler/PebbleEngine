[[FX]]

// Samplers
sampler2D albedoMap = sampler_state
{
	Texture = "textures/common/white.tga";
};


samplerCube ambientMap = sampler_state
{
	Address = Clamp;
	Filter = Bilinear;
	MaxAnisotropy = 1;
};

samplerCube envMap = sampler_state
{
	Address = Clamp;
	Filter = Bilinear;
	MaxAnisotropy = 1;
};

// Uniforms
float4 PointParams <
	string desc_a = "a: size of the point";
	string desc_b = "b: scale of point size";
	string desc_c = "c: thickness scale";
	string desc_d = "d: shadow scale";
> = {1.1, 1.0, 1.0, 1.0};

// Uniforms
float4 matDiffuseCol <
	string desc_abc = "abc: diffuse color";
	string desc_d   = "d: alpha for opacity";
> = {1.0, 1.0, 1.0, 1.0};

float4 matSpecParams <
	string desc_abc = "abc: specular color";
	string desc_d   = "d: gloss";
> = {0.04, 0.04, 0.04, 0.5};

context ATTRIBPASS
{
	VertexShader = compile GLSL VS_DEPTHPASS;
	PixelShader = compile GLSL FS_ATTRIBPASS;
	
}

context SHADOWMAP
{
	VertexShader = compile GLSL VS_SHADOWMAP;
	PixelShader = compile GLSL FS_SHADOWMAP;
}

context LIGHTING
{
	VertexShader = compile GLSL VS_DEPTHPASS;
	PixelShader = compile GLSL FS_LIGHTING;
	
	ZWriteEnable = false;
	BlendMode = Add;
}

context AMBIENT
{
	VertexShader = compile GLSL VS_DEPTHPASS;
	PixelShader = compile GLSL FS_AMBIENT;
}

float ambientIntensity = 0.3;

// =================================================================================================
[[VS_DEPTHPASS]]
#version 120
#include "shaders/utilityLib/vertCommon.glsl"

uniform mat4 viewProjMat;
uniform vec2 frameBufSize;
attribute vec3 vertPos;

uniform vec4 PointParams;
varying vec3 eyeSpacePos;
// varying vec4 pos;

void main(void)
{
	vec4 pos = calcWorldPos( vec4( vertPos, 1.0 ) );
	eyeSpacePos = calcViewPos( pos ).xyz;
    
    float dist = length(eyeSpacePos);
    gl_PointSize = PointParams.x * (PointParams.y / dist);
	gl_Position = viewProjMat * pos;;
}

// =================================================================================================
[[FS_ATTRIBPASS]]
#version 120
#include "shaders/utilityLib/fragDeferredWrite.glsl" 

uniform vec3 viewerPos;
uniform vec4 matDiffuseCol;
uniform vec4 matSpecParams;

uniform mat4 projMat;
uniform mat4 viewMatInv;
uniform vec4 PointParams;

varying vec3 eyeSpacePos;
// varying vec4 pos;

uniform sampler2D albedoMap;

void main( void )
{	
	vec2 texCoords = gl_PointCoord.xy;
	
	vec3 N;
	N.xy = texCoords*vec2(2.0, -2.0) + vec2(-1.0, 1.0);;
	float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;  // kill pixels outside circle
    N.z = sqrt(1.0 - r2);
    
	// Point on surface of sphere in eye space
	vec4 spherePosEye = vec4(eyeSpacePos+N*PointParams.x, 1.0);
	// Calculate depth from sphere eye space
    vec4 clipSpacePos = projMat * spherePosEye;
    float normDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;
    gl_FragDepth = normDepth;
    
    vec3 newPos = (viewMatInv*spherePosEye).xyz;
    
    setMatID( 1.0 );
	setPos( newPos.xyz - viewerPos );
	setNormal( normalize(N) );
	texCoords.t *= -1.0;
    vec4 albedo = texture2D( albedoMap, texCoords.st ) * matDiffuseCol;
	setAlbedo( albedo.rgb );
	setSpecParams( matSpecParams.rgb, matSpecParams.a );
    
}

// =================================================================================================
[[VS_SHADOWMAP]]
#version 120
#include "shaders/utilityLib/vertCommon.glsl"

attribute vec3 vertPos;

uniform mat4 viewProjMat;
uniform vec2 frameBufSize;
uniform vec4 lightPos;
uniform vec4 PointParams;

varying vec3 eyeSpacePos;
varying vec4 screenSpacePos;
varying float dist;

void main(void)
{
	vec4 pos = calcWorldPos( vec4( vertPos, 1.0 ) );
	dist = length( lightPos.xyz - pos.xyz ) / lightPos.w;
	
	eyeSpacePos = calcViewPos( pos ).xyz;
	screenSpacePos = viewProjMat * pos;
	
    gl_PointSize = PointParams.x * ((PointParams.y*PointParams.w) / length(eyeSpacePos));
	gl_Position = screenSpacePos;
}

// =================================================================================================
[[FS_SHADOWMAP]]
#version 120
uniform float shadowBias;
varying float dist;

void main( void )
{
	vec2 texCoords = gl_PointCoord.xy;
	vec3 N;
	N.xy = texCoords*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	float r2 = dot(N.xy, N.xy);
	
    if (r2 > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0 - r2);

	gl_FragDepth = dist + shadowBias;
}

[[FS_LIGHTING]]
#version 120
// =================================================================================================
#include "shaders/utilityLib/fragLighting.glsl" 

uniform vec4 matDiffuseCol;
uniform vec4 matSpecParams;
uniform float ambientIntensity;
uniform sampler2D albedoMap;

uniform mat4 projMat;
uniform mat4 viewMatInv;
uniform vec4 PointParams;

varying vec3 eyeSpacePos;
// varying vec4 pos;


void main( void )
{

    vec2 texCoords = gl_PointCoord.xy;
	
	vec3 N;
	N.xy = texCoords*vec2(2.0, -2.0) + vec2(-1.0, 1.0);;
	float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;  // kill pixels outside circle
    N.z = sqrt(1.0 - r2);
    
	// Point on surface of sphere in eye space
	vec4 spherePosEye = vec4(eyeSpacePos+N*PointParams.x, 1.0);
	// Calculate depth from sphere eye space
    vec4 clipSpacePos = projMat * spherePosEye;
    float normDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;
    gl_FragDepth = normDepth;
    
    vec3 newPos = (viewMatInv*spherePosEye).xyz;
    
    vec3 normal = N;
    
    texCoords.t *= -1.0;
	vec4 albedo = texture2D( albedoMap, texCoords.st ) * matDiffuseCol;
	
	gl_FragColor.rgb =
		calcPhongSpotLight( newPos, normalize( normal ), albedo.rgb, matSpecParams.rgb,
		                    matSpecParams.a, -newPos.z, ambientIntensity );
}


[[FS_AMBIENT]]	
#version 120
// =================================================================================================
#ifdef _F03_ParallaxMapping
	#define _F02_NormalMapping
#endif

#include "shaders/utilityLib/fragLighting.glsl" 

uniform sampler2D albedoMap;
uniform samplerCube ambientMap;

uniform mat4 projMat;
uniform mat4 viewMatInv;
uniform vec4 PointParams;

varying vec3 eyeSpacePos;
// varying vec4 pos;


void main( void )
{

    vec2 texCoords = gl_PointCoord.xy;
	
	vec3 N;
	N.xy = texCoords*vec2(2.0, -2.0) + vec2(-1.0, 1.0);;
	float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;  // kill pixels outside circle
    N.z = sqrt(1.0 - r2);
    
	// Point on surface of sphere in eye space
	vec4 spherePosEye = vec4(eyeSpacePos+N*PointParams.x, 1.0);
	// Calculate depth from sphere eye space
    vec4 clipSpacePos = projMat * spherePosEye;
    float normDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;
    gl_FragDepth = normDepth;
    
    vec3 newCoords = vec3( texCoords, 0 );
    
    newCoords.t *= -1.0;
	vec4 albedo = texture2D( albedoMap, newCoords.st );
    
    vec3 normal = normalize( N );
	
	gl_FragColor.rgb = albedo.rgb * textureCube( ambientMap, normal ).rgb;

}