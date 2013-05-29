[[FX]]

// Supported Flags
/* ---------------
	_F16_McGeometryShaderInjected
*/

// Samplers
sampler2D spotLightMap = sampler_state
{
	Address = Clamp;
};

sampler2D buf0 = sampler_state
{
	Address = Clamp;
};

sampler2D buf1 = sampler_state
{
	Address = Clamp;
};

sampler2D buf2 = sampler_state
{
	Address = Clamp;
};

float cameraFov = 45.0;
float refractIdx = 1.333;
float isolevel = 120.0;

// Uniforms
float4 PointParams <
	string desc_a = "a: size of the point";
	string desc_b = "b: scale of point size";
	string desc_c = "c: thickness scale";
	string desc_d = "d: shadow scale";
> = {1.1, 1.0, 1.0, 1.0};


float4 absorptionIdx = {0.0, 0.0, 0.0, 0.0};

float4 FluidColor  <
	string desc_a = "a: red channel";
	string desc_b = "b: green channel";
	string desc_c = "c: blue channel";
	string desc_d = "d: opacity default is 0, it's transparent";
> = {0.0, 0.0, 0.0, 0.0};

context ATTRIBPASS
{
	VertexShader = compile GLSL VS_SPHGENERIC;
	PixelShader = compile GLSL FS_ATTRIBPASS;
	
	ZEnable = true;
	ZWriteEnable = true;
}

context DEPTHPASS
{
	VertexShader = compile GLSL VS_SPHGENERIC;
	PixelShader = compile GLSL FS_DEPTHPASS;
	
	ZEnable = true;
	ZWriteEnable = true;
}

context THICKNESSPASS
{
	VertexShader = compile GLSL VS_THICKNESSPASS;
	PixelShader = compile GLSL FS_THICKNESSPASS;
	
	ZEnable = true;
	ZWriteEnable = false;
	BlendMode = Add;
}

context CAUSTICMAP
{
    VertexShader = compile GLSL VS_CAUSTICMAP;
    PixelShader = compile GLSL FS_CAUSTICMAP;
    
    ZEnable = false;
    ZWriteEnable = false;
    BlendMode = Add;
}

context SHADOWMAP
{
	VertexShader = compile GLSL VS_SHADOWMAP;
	PixelShader = compile GLSL FS_SHADOWMAP;
}

// =================================================================================================
[[VS_SPHGENERIC]]
#version 120

#ifndef _F16_McGeometryShaderInjected
#include "shaders/utilityLib/vertCommon.glsl"
uniform mat4 viewProjMat;
uniform vec2 frameBufSize;
uniform vec4 PointParams;
varying vec4 screenSpacePos;
#endif
varying vec3 eyeSpacePos;
attribute vec3 vertPos;

void main(void)
{
#ifdef _F16_McGeometryShaderInjected
	eyeSpacePos = vertPos;
	gl_Position = vec4( vertPos, 1.0 );
#else
	// texCoords = gl_PointCoord;
	vec4 pos = calcWorldPos( vec4( vertPos, 1.0 ) );
	eyeSpacePos = calcViewPos( pos ).xyz;
    screenSpacePos = viewProjMat * pos;
    
    float dist = length(eyeSpacePos);
    gl_PointSize = PointParams.x * (PointParams.y / dist);
	gl_Position = screenSpacePos;
#endif
}

// =================================================================================================
[[FS_ATTRIBPASS]]
#version 120

#include "shaders/utilityLib/fragSphUtils.glsl"

#ifdef _F16_McGeometryShaderInjected
varying vec4 position;
uniform float isolevel;
#else
uniform mat4 projMat;
#endif
// uniform vec3 viewerPos;
varying vec3 eyeSpacePos;
uniform mat4 viewMat;
uniform vec4 PointParams;

uniform float refractIdx;
uniform vec4 absorptionIdx;
uniform vec4 FluidColor;

void main( void )
{	
#ifdef _F16_McGeometryShaderInjected
    gl_FragData[0].r = -1.0*eyeSpacePos.z + 0.001 * isolevel;
#else
	vec2 texCoords = gl_PointCoord.xy;
	
	vec3 N;
	N.xy = texCoords*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;  // kill pixels outside circle
    N.z = sqrt(1.0 - r2);
    
	// Point on surface of sphere in eye space
	vec4 spherePosEye = vec4(eyeSpacePos+N*PointParams.x, 1.0);
	// Calculate depth from sphere eye space
	vec4 clipSpacePos = projMat * spherePosEye;
	// Normal depth
	float normDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;
	gl_FragDepth = normDepth;

    // depth
    gl_FragData[0].r = -1.0*(spherePosEye.z);
#endif
    // fluid color
    gl_FragData[0].g = packToFloat(FluidColor.rgb);
    gl_FragData[0].b = packToFloat(vec3(absorptionIdx.rgb*0.5));
    gl_FragData[0].a = packToFloat(vec3(refractIdx*0.1, absorptionIdx.a*0.1, FluidColor.a*0.1));
    
}

// =================================================================================================
[[FS_DEPTHPASS]]
#version 120

#include "shaders/utilityLib/fragSphUtils.glsl"

#ifdef _F16_McGeometryShaderInjected
varying vec4 position;
uniform float isolevel;
#else
uniform mat4 projMat;
#endif
uniform mat4 viewMat;
uniform vec4 PointParams;
varying vec3 eyeSpacePos;

void main( void )
{	
#ifdef _F16_McGeometryShaderInjected
    setDepth(eyeSpacePos.z + 0.001 * isolevel);
#else
	vec2 texCoords = gl_PointCoord.xy;
	
	vec3 N;
	N.xy = texCoords*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;  // kill pixels outside circle
    N.z = sqrt(1.0 - r2);
    
	// Point on surface of sphere in eye space
	vec4 spherePosEye = vec4(eyeSpacePos+N*PointParams.x, 1.0);
	// Calculate depth from sphere eye space
	vec4 clipSpacePos = projMat * spherePosEye;
	// Normal depth
	float normDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;
	gl_FragDepth = normDepth;
    setDepth(spherePosEye.z) ;
#endif
}


// =================================================================================================
[[VS_THICKNESSPASS]]
#version 120
#include "shaders/utilityLib/vertCommon.glsl"
uniform mat4 viewProjMat;
uniform vec2 frameBufSize;
uniform vec4 PointParams;
varying vec4 screenSpacePos;

varying vec3 eyeSpacePos;
attribute vec3 vertPos;

void main(void)
{
	// texCoords = gl_PointCoord;
	vec4 pos = calcWorldPos( vec4( vertPos, 1.0 ) );
	eyeSpacePos = calcViewPos( pos ).xyz;
    screenSpacePos = viewProjMat * pos;
    
    float dist = length(eyeSpacePos);
    gl_PointSize = PointParams.x * (2.0*PointParams.y / dist);
	gl_Position = screenSpacePos;
}


// =================================================================================================
[[FS_THICKNESSPASS]]
#version 120
#include "shaders/utilityLib/fragSphUtils.glsl"

uniform vec3 viewerPos;
uniform mat4 projMat;
uniform mat4 viewMat;
uniform float refractIdx;
uniform vec4 absorptionIdx;
uniform vec4 FluidColor;
uniform vec4 PointParams;
varying vec3 eyeSpacePos;

void main( void )
{	
	
	vec2 texCoords = gl_PointCoord.xy;
	vec3 N;
	N.xy = texCoords*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	float r2 = dot(N.xy, N.xy);
    if (r2 > 1.0) discard;   // kill pixels outside circle
	N.z = sqrt(1.0 - r2);
	
	// Point on surface of sphere in eye space
	vec4 spherePosEye = vec4(eyeSpacePos+N*PointParams.x, 1.0);
	// Calculate depth from sphere eye space
	vec4 clipSpacePos = projMat * spherePosEye;
	// Normal depth
	gl_FragDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;
	
	float alpha = exp(-r2*2.0);
    gl_FragData[0] = floatToRGBA8(N.z*PointParams.x*2.0*PointParams.z*alpha); 
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


// =================================================================================================
[[VS_CAUSTICMAP]]
#version 120 
#extension GL_EXT_gpu_shader4 : require

#include "shaders/utilityLib/vertCommon.glsl"
#include "shaders/utilityLib/sphUtils.glsl"

attribute vec3 vertPos;

uniform mat4 projMat;

uniform sampler2D buf0; // transparent eye space position
uniform sampler2D buf1; // background depth

uniform float pointsCount;
uniform float refractIdx;
uniform vec4 absorptionIdx;

varying vec4 color;

// Take the eye-space position and project it into a 2D image coordinate
vec2 projectToTexCoord( vec4 esPos )
{
    vec4 projLoc = projMat * esPos;
    return ( 0.5*(projLoc.xy / projLoc.w) + 0.5 );
}

vec2 calcIntersectionPtUV(vec3 v, vec3 pre_pos, vec3 refract_vec)
{   
    vec3 p = v + distance(v, pre_pos) * refract_vec;
    return projectToTexCoord( vec4(p, 1.0) );
}

void main( void )
{
    vec4 pos = calcWorldPos( vec4( vertPos, 1.0 ) );
    vec4 eyeSpacePos = calcViewPos( pos );
    vec2 uv = projectToTexCoord(eyeSpacePos);
    
    vec3 surfacePt = getEyePos(buf0, uv);
    if (surfacePt.z - eyeSpacePos.z < 1.0) {
        vec3 surfaceNormal = reconstructNormal(surfacePt, buf0, uv);//normalize(texture2D(buf2, uv).xyz);

        vec3 incidentLight = normalize(eyeSpacePos.xyz);
        vec3 refractLight = refraction(incidentLight, surfaceNormal, 1.0/refractIdx, 1.0/(refractIdx*refractIdx)).xyz;

        vec3 causticsPt = vec3(surfacePt.xy, surfacePt.z + 1.0);

        // step 2
        vec2 incUv = calcIntersectionPtUV(surfacePt, causticsPt, refractLight.xyz);
        causticsPt = getEyePos(buf1, incUv);
        incUv = calcIntersectionPtUV(surfacePt, causticsPt, refractLight.xyz);
        causticsPt = getEyePos(buf1, incUv);
        // incUv = calcIntersectionPtUV(surfacePt, causticsPt, refractLight.xyz);
        // causticsPt = getEyePos(buf1, incUv);

        float dist = distance(causticsPt, surfacePt);
        float alpha = dot(surfaceNormal, incidentLight);
        color =  exp(-dist * absorptionIdx) ;
        color.a = alpha;
        gl_PointSize = 3.0;
        gl_Position = projMat*vec4(causticsPt, 1.0);
    } else {
        gl_PointSize = 0.0;
        color =  vec4(0.0);
        gl_Position = projMat*eyeSpacePos ;
    }

}

// =================================================================================================
[[FS_CAUSTICMAP]]
#version 120
#extension GL_EXT_gpu_shader4 : require

#include "shaders/utilityLib/sphUtils.glsl"

uniform sampler2D spotLightMap; // spot light texture
varying vec4 color;

void main(void)
{   
    vec2 texCoords = gl_PointCoord.xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	float r2 = dot(texCoords, texCoords);
	
    if (r2 > 1.0) discard;   // kill pixels outside circle
    
    gl_FragColor = color*texture2D(spotLightMap, texCoords)*simpleCAA(r2);
}
