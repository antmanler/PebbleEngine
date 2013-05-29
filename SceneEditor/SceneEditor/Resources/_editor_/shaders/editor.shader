[[FX]]

// Uniforms
float4 NormalCol = {1.0, 1.0, 1.0, 1.0};

float4 SelectedCol = {1.0, 1.0, 0.0, 1.0};

// float IsSeleced = 0.0;

context _EDITOR_
{
	VertexShader = compile GLSL VS_GENERAL;
	PixelShader = compile GLSL FS_AMBIENT;
}

[[VS_GENERAL]]
// =================================================================================================

uniform mat4 viewMat;
uniform mat4 worldMat;
uniform	mat3 worldNormalMat;
uniform mat4 viewProjMat;

vec4 calcWorldPos( const vec4 pos )
{
	return worldMat * pos;
}

vec4 calcViewPos( const vec4 pos )
{
	return viewMat * pos;
}

attribute vec3 vertPos;

varying vec4 pos;

void main( void )
{
	pos = calcWorldPos( vec4( vertPos, 1.0 ) );
	gl_Position = viewProjMat * pos;
}

[[FS_AMBIENT]]	
// =================================================================================================

// varying vec4 pos;
// uniform float IsSeleced;
uniform vec4 NormalCol;
uniform vec4 customInstData[1];
uniform vec4 SelectedCol;

void main( void )
{
    vec3 color = NormalCol.rgb;
	if (customInstData[0].x > 0.9) color = SelectedCol.rgb;
	
	gl_FragColor.rgb = color;
}
