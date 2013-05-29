[[FX]]

// Samplers
sampler2D heightNormMap;
sampler2D detailMap;

// Uniforms
float4 sunDir = {1.0, -1.0, 0, 0};

/*samplerCube envMap = sampler_state
{
	Address = Clamp;
	Filter = Bilinear;
	MaxAnisotropy = 1;
};*/

// Contexts
context ATTRIBPASS
{
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_ATTRIBPASS;
}



context LIGHTING
{
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_LIGHTING;
	
	ZWriteEnable = false;
	BlendMode = Add;
}

context AMBIENT
{
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_AMBIENT;
}


[[VS_GENERAL]]
// =================================================================================================

#include "shaders/utilityLib/vertCommon.glsl"

uniform mat4 viewProjMat;
uniform vec4 terBlockParams;
attribute vec3 vertPos;
attribute float terHeight;
varying vec4 pos, vsPos;
varying vec2 texCoords;

void main( void )
{
	vec4 newPos = vec4( vertPos.x * terBlockParams.z + terBlockParams.x, terHeight,
						vertPos.z * terBlockParams.z + terBlockParams.y, 1.0 );
						
	pos = calcWorldPos( newPos );
	vsPos = calcViewPos( pos );
						
	texCoords = vec2( newPos.x, newPos.z );
	gl_Position = viewProjMat * pos;
}


[[FS_ATTRIBPASS]]
// =================================================================================================

#include "shaders/utilityLib/fragDeferredWrite.glsl"

uniform vec3 viewerPos;
uniform vec4 sunDir;
uniform sampler2D heightNormMap, detailMap;
varying vec4 pos;
varying vec2 texCoords;

vec3 light = -normalize( sunDir.xyz );

void main( void )
{
	vec3 detailCol = texture2D( detailMap, texCoords * 200.0 ).rgb;
	vec4 texel = texture2D( heightNormMap, texCoords ) * 2.0 - 1.0;
	float ny = sqrt( max( 1.0 - texel.b*texel.b - texel.a*texel.a, 0.0 ) );		// Use max because of numerical issues
	vec3 normal = vec3( texel.b, ny, texel.a );
	
	setMatID( 1.0 );
	setPos( pos.xyz - viewerPos );
	setNormal( normalize( normal ) );
	
	// Wrap lighting fur sun
	float l = max( dot( normal, light ), 0.0 ) * 0.5 + 0.5;
	setAlbedo( detailCol*l );
	setSpecParams( vec3(0.0), 0.0 );
	//setSpecMask( 0.0 );
}
				

[[VS_SHADOWMAP]]
// =================================================================================================

#include "shaders/utilityLib/vertCommon.glsl"

uniform mat4 viewProjMat;
uniform vec4 lightPos;
uniform vec4 terBlockParams;
attribute vec3 vertPos;
attribute float terHeight;
varying float dist;

void main( void )
{
	vec4 newPos = vec4( vertPos.x * terBlockParams.z + terBlockParams.x, terHeight,
						vertPos.z * terBlockParams.z + terBlockParams.y, 1.0 );
						
	vec4 pos = calcWorldPos( newPos );
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


[[FS_LIGHTING]]
// =================================================================================================

#include "shaders/utilityLib/fragLighting.glsl"

uniform sampler2D heightNormMap, detailMap;
varying vec4 pos, vsPos;
varying vec2 texCoords;

void main( void )
{
	vec3 detailCol = texture2D( detailMap, texCoords * 300.0 ).rgb;
	vec4 texel = texture2D( heightNormMap, texCoords ) * 2.0 - 1.0;
	float ny = sqrt( max( 1.0 - texel.b*texel.b - texel.a*texel.a, 0.0 ) );		// Use max because of numerical issues
	vec3 normal = vec3( texel.b, ny, texel.a );
	
	gl_FragColor.rgb =
		calcPhongSpotLight( pos.xyz, normalize( normal ), detailCol, 0.0, 16.0, -vsPos.z, 0.3 );
}


[[FS_AMBIENT]]
// =================================================================================================

#ifdef _F04_EnvMapping
	uniform samplerCube envMap;
#endif

uniform vec4 sunDir;
uniform sampler2D heightNormMap, detailMap;
varying vec2 texCoords;

vec3 light = -normalize( sunDir.xyz );

void main( void )
{
	vec3 detailCol = texture2D( detailMap, texCoords * 300.0 ).rgb;
	vec4 texel = texture2D( heightNormMap, texCoords ) * 2.0 - 1.0;
	float ny = sqrt( max( 1.0 - texel.b*texel.b - texel.a*texel.a, 0.0 ) );		// Use max because of numerical issues
	vec3 normal = vec3( texel.b, ny, texel.a );
	
	// Wrap lighting fur sun
	float l = max( dot( normal, light ), 0.0 ) * 0.5 + 0.5;
	
	gl_FragColor = vec4( detailCol * l, 1.0 );
	
/*#ifdef _F04_EnvMapping
	vec3 refl = textureCube( envMap, reflect( pos.xyz - viewerPos, normalize( normal ) ) ).rgb;
	gl_FragColor.rgb = refl * 0.2;
#endif*/
}
