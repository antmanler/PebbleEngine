[[FX]]

// Samplers
sampler2D albedoMap = sampler_state
{
	Filter = Bilinear;
	MaxAnisotropy = 1;
};

float refractIdx = 1.333;
float4 FluidColor = {0.20, 0.30, 0.9, 1.5};

// Contexts
/*context SHADOWMAP
{
	VertexShader = compile GLSL VS_SHADOWMAP;
	PixelShader = compile GLSL FS_SHADOWMAP;
}*/

context TRANSLUCENT
{
	VertexShader = compile GLSL VS_TRANSLUCENT;
	PixelShader = compile GLSL FS_TRANSLUCENT;
	
	ZWriteEnable = false;
	BlendMode = AddBlended;
}

context DEPTHPASS
{
    VertexShader = compile GLSL VS_DEPTHPASS;
    PixelShader = compile GLSL FS_DEPTHPASS;

    ZEnable = true;
    ZWriteEnable = true;
}

context THICKNESSPASS
{
	VertexShader = compile GLSL VS_DEPTHPASS;
	PixelShader = compile GLSL FS_THICKNESSPASS;
	
	ZEnable = true;
	ZWriteEnable = false;
	BlendMode = Add;
}

[[VS_SHADOWMAP]]
// =================================================================================================

#include "shaders/utilityLib/vertParticle.glsl"

uniform mat4 viewProjMat;
uniform vec4 lightPos;
attribute vec2 texCoords0;
varying float dist;

void main(void)
{
	vec4 pos = vec4( calcParticlePos( texCoords0 ), 1 );
	dist = length( lightPos.xyz - pos.xyz ) / lightPos.w;
	
	gl_Position = viewProjMat * pos;
}
				
				
[[FS_SHADOWMAP]]
// =================================================================================================

uniform float shadowBias;
varying float dist;

void main( void )
{
	gl_FragDepth = dist + shadowBias;
}


[[VS_TRANSLUCENT]]
// =================================================================================================

#include "shaders/utilityLib/vertParticle.glsl"

uniform mat4 viewProjMat;
attribute vec2 texCoords0;
varying vec4 color;
varying vec2 texCoords;

void main(void)
{
	color = getParticleColor();
	texCoords = vec2( texCoords0.s, -texCoords0.t );
	gl_Position = viewProjMat * vec4( calcParticlePos( texCoords0 ), 1 );
}


[[FS_TRANSLUCENT]]
// =================================================================================================

uniform sampler2D albedoMap;
varying vec4 color;
varying vec2 texCoords;

void main( void )
{
	vec4 albedo = texture2D( albedoMap, texCoords );
	
	gl_FragColor = albedo * color;
}

[[VS_DEPTHPASS]]
// =================================================================================================

#include "shaders/utilityLib/vertParticle.glsl"

uniform mat4 viewProjMat;
attribute vec2 texCoords0;
varying vec2 texCoords;

uniform mat4 viewMat;
varying vec3 eyeSpacePos;
varying float radius;

void main(void)
{
    texCoords = vec2( texCoords0.s, texCoords0.t );

    int idx = int( parIdx );
    eyeSpacePos = (viewMat * vec4(parPosArray[idx], 1.0)).xyz;
    radius = parSizeAndRotArray[idx].x / sqrt(2.0);

    gl_Position = viewProjMat * vec4( calcParticlePos( texCoords0 ), 1.0 );
}


[[FS_DEPTHPASS]]
// =================================================================================================
#include "shaders/utilityLib/fragSphUtils.glsl"

uniform mat4 projMat;
uniform mat4 viewMat;
varying vec2 texCoords;

varying vec3 eyeSpacePos;
varying float radius;

void main( void )
{	
    vec3 N;
    N.xy = texCoords*2.0-1.0;
    float r2 = dot(N.xy, N.xy);

    if (r2 > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0 - r2);

    // calculate depth
    vec4 pixelPos = vec4(eyeSpacePos + N*radius/frameBufSize.y, 1.0); 
    vec4 clipSpacePos = projMat*pixelPos;
    gl_FragDepth = clipSpacePos.z / clipSpacePos.w + 0.002;;

    float depth = pixelPos.z ;
    setDepth(depth) ;
}

[[FS_THICKNESSPASS]]
// =================================================================================================
#version 120

#include "shaders/utilityLib/fragSphUtils.glsl"

uniform mat4 projMat;
uniform mat4 viewMat;

uniform float refractIdx;
uniform vec4 absorptionIdx;
uniform vec4 FluidColor;

varying vec2 texCoords;

varying vec3 eyeSpacePos;
varying float radius;

void main( void )
{	
	vec3 N;
    N.xy = texCoords*2.0-1.0;
    float r2 = dot(N.xy, N.xy);
	
    if (r2 > 1.0) discard;   // kill pixels outside circle
	
    gl_FragData[0].r = r2/(radius*eyeSpacePos.z*eyeSpacePos.z)/FluidColor.w;
    gl_FragData[0].g = packToFloat(FluidColor.xyz);
    gl_FragData[0].b = refractIdx;
    gl_FragData[0].a = 1.0;
}

