[[FX]]

// Samplers
sampler2D albedoMap = sampler_state
{
	Filter = Bilinear;
	MaxAnisotropy = 1;
};

samplerCube envMap = sampler_state
{
	Address = Clamp;
	Filter = Bilinear;
	MaxAnisotropy = 1;
};

sampler2D buf0 = sampler_state
{
	Address = Clamp;
};

sampler2D depthBuf = sampler_state
{
	Address = Clamp;
};

sampler3D volMap = sampler_state
{
	Address = Clamp;
	Filter = None;
};

// Uniforms
float4 sunDir = {1.0, -1.0, 0, 0};
float sphereRadius = 0.01;
float blurV = 1.0;
float4 blurParams = {0, 0, 0, 0};

float StepSize = 0.02 ;
float Steps = 50.0 ;

// Contexts
/*context SHADOWMAP
{
	VertexShader = compile GLSL VS_SHADOWMAP;
	PixelShader = compile GLSL FS_SHADOWMAP;
}*/

context DRAWBACFACE
{
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_DRAWBACFACE;

	CullMode = Front;	
	ZEnable = true;
	ZWriteEnable = true;
	// BlendMode = AddBlended;
}

context DEPTHPASS
{
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_DEPTHPASS;
	
	ZEnable = true;
	ZWriteEnable = true;
	// BlendMode = AddBlended;
}

context BLUR
{
	VertexShader = compile GLSL VS_FSQUAD;
	PixelShader = compile GLSL FS_BLUR;
	
	ZWriteEnable = false;
}

context RECOSNORMAL
{
	VertexShader = compile GLSL VS_FSQUAD;
	PixelShader = compile GLSL FS_RECONSNORMAL;
}

[[VS_GENERAL]]
// =================================================================================================
#include "shaders/utilityLib/vertCommon.glsl"

uniform mat4 viewProjMat;
uniform vec3 viewerPos;
attribute vec3 vertPos;
attribute vec2 texCoords0;
attribute vec3 normal;

varying vec4 pos, vsPos, ssPos;
varying vec3 lsPos;
varying vec2 texCoords;

void main( void )
{
	lsPos = vertPos;
	pos = calcWorldPos( vec4( lsPos, 1.0 ) );
	vsPos = calcViewPos( pos );
	texCoords = texCoords0;
	ssPos = viewProjMat * pos;
	gl_Position = ssPos;
}

[[FS_DRAWBACFACE]]
// =================================================================================================
varying vec3 lsPos;

void main(void)
{
	gl_FragColor.xyz = lsPos*0.5 + 0.5;
	gl_FragColor.a = 1.0;
}


[[FS_DEPTHPASS]]
// =================================================================================================
#include "shaders/utilityLib/fragSphUtils.glsl"

uniform sampler3D volMap;
uniform sampler2D buf0;
uniform mat4 worldMat;
uniform mat4 projMat;
uniform mat4 viewMat;

uniform float StepSize;
uniform float Steps;

varying vec3 lsPos;
varying vec4 ssPos, vsPos;
varying vec2 texCoords;

void main( void )
{	
	vec2  coord = (ssPos.xy / ssPos.w)*0.5 + 0.5 ;
	
	vec3  ray_origin = lsPos.xyz*0.5 + 0.5;
	
	vec3  ray_end = texture2D(buf0, coord).xyz  ;
	vec3  ray_dir = normalize(ray_end - ray_origin) ;
    float len = length(ray_dir) ;
    vec3  detla_dir = normalize(ray_dir) * StepSize ;
    vec3  pos = ray_origin;

	// float density = 0.0;
	vec4 density = vec4(0.0);
    for ( float i = 0.0; i < Steps; i += 1.0 ) {
        // density = texture3D(volMap, pos).a;
		density = texture3D(volMap, pos);
        // if (density > 0.0) {
		if(density.a > 0.001) {
			// gl_FragColor.xyz = vec3(density.xyz - pos); return;
            // vec3 epos = (viewMat* worldMat * vec4(pos*2.0 - 1.0, 1.0)).xyz;
			vec3 epos = (viewMat * worldMat * vec4(density.xyz, 1.0)).xyz;
			vec4 clipSpacePos = projMat*vec4(epos,1.0);
			gl_FragDepth = clipSpacePos.z / clipSpacePos.w;
			setDepth(epos.z) ;
			return;
        }
		pos += detla_dir ;
	}
	discard;
	
	// vec3 N;
	// N.xy = texCoords*2.0-1.0;
	// float r2 = dot(N.xy, N.xy);
	// 
	//     if (r2 > 1.0) discard;   // kill pixels outside circle
	//     N.z = sqrt(1.0 - r2);
	// 
	// // calculate depth
	// vec4 pixelPos = vec4(eyeSpacePos + N*radius, 1.0); 
	// vec4 clipSpacePos = projMat*pixelPos;
	// gl_FragDepth = clipSpacePos.z / clipSpacePos.w;
	// 
	// float depth = pixelPos.z ;
	// setDepth(depth) ;
}

[[VS_FSQUAD]]

uniform mat4 projMat;
attribute vec3 vertPos;
varying vec2 texCoords;
				
void main( void )
{
	texCoords = vertPos.xy ;
	gl_Position = projMat * vec4( vertPos, 1 );
}

[[FS_BLUR]]
// =================================================================================================

#include "shaders/utilityLib/fragSphUtils.glsl"

uniform sampler2D buf0;
uniform float blurV;
uniform vec4 blurParams;
varying vec2 texCoords;

void main( void )
{	
	vec2 blurDir = vec2(blurV, 1.0-blurV);
	float filterRadius = blurParams.x;
	float blurScale = blurParams.y;
	float blurDepthFalloff = blurParams.z;
	vec2 texelSize = vec2(1.0, 1.0) / frameBufSize ; 
	
	// bilateral filter
	float depth = getDepth(buf0, texCoords);
	float sum = 0.0;
	float wsum = 0.0;
	for(float x = -filterRadius; x <= filterRadius; x += 1.0) {
		float sample = getDepth(buf0, texCoords + x*texelSize*blurDir);
		
		// spatial domain
		float r = x * blurScale;
		float w = exp(-r*r);
		
		// range domain
		float r2 = (sample - depth) * blurDepthFalloff; 
		float g = exp(-r2*r2);
		
		sum += sample * w * g; 
		wsum += w * g;
	}
	if (wsum > 0.0) {
		sum /= wsum;
	} 
	
	setDepth(sum);
}

[[FS_RECONSNORMAL]]
// =================================================================================================

#include "shaders/utilityLib/fragSphUtils.glsl"

uniform sampler2D depthBuf;
varying vec2 texCoords;

void main()
{
	// read eye-space depth from texture
	float depth = getDepth(depthBuf, texCoords);
	if (depth > 0.001) {
		discard;
		return;
	}
	
	// calculate eye-space position from depth
	vec3 posEye = uv_to_eye(texCoords, depth);
	vec2 texelSize = vec2(1.0, 1.0) / frameBufSize ;
	
	vec3 ddx = getEyePos(depthBuf, texCoords + vec2(texelSize.x, 0.0)) - posEye; 
	vec3 ddx2 = posEye - getEyePos(depthBuf, texCoords + vec2(-texelSize.x, 0.0)); 
	if (abs(ddx.z) > abs(ddx2.z)) {
		ddx = ddx2;
	}
	
	vec3 ddy = getEyePos(depthBuf, texCoords + vec2(0.0, texelSize.y)) - posEye;
	vec3 ddy2 = posEye - getEyePos(depthBuf, texCoords + vec2(0.0, -texelSize.y));
	if (abs(ddy2.z) < abs(ddy.z)) {
		ddy = ddy2;
	}
	
	// calculate normal
	vec3 n = cross(ddx, ddy);
	n = normalize(n);
	gl_FragColor.xyz = n ;
	gl_FragColor.w = 1.0 ;

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