[[FX]]

// Samplers
sampler2D depthBuf = sampler_state
{
	Address = Clamp;
};

sampler2D gbuf0 = sampler_state
{
	Address = Clamp;
};

sampler2D gbuf1 = sampler_state
{
	Address = Clamp;
};

sampler2D gbuf2 = sampler_state
{
	Address = Clamp;
};

sampler2D gbuf3 = sampler_state
{
	Address = Clamp;
};

samplerCube ambientMap = sampler_state
{
    Texture = "textures/ambientMapWhite.dds";
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

// Contexts
context AMBIENT
{
	VertexShader = compile GLSL VS_FSQUAD;
	PixelShader = compile GLSL FS_AMBIENT;
	
	ZWriteEnable = false;
	BlendMode = Replace;
}

context LIGHTING
{
	VertexShader = compile GLSL VS_VOLUME;
	PixelShader = compile GLSL FS_LIGHTING;
	
	ZWriteEnable = false;
	BlendMode = Add;
}

context COPY_DEPTH
{
	VertexShader = compile GLSL VS_FSQUAD;
	PixelShader = compile GLSL FS_COPY_DEPTH;
}


[[VS_FSQUAD]]

uniform mat4 projMat;
attribute vec3 vertPos;
varying vec2 texCoords;
				
void main( void )
{
	texCoords = vertPos.xy; 
	gl_Position = projMat * vec4( vertPos, 1.0 );
}


[[VS_VOLUME]]

uniform mat4 viewProjMat;
uniform mat4 worldMat;
attribute vec3 vertPos;
varying vec4 vpos;
				
void main( void )
{
	vpos = viewProjMat * worldMat * vec4( vertPos, 1.0 );
	gl_Position = vpos;
}


[[FS_AMBIENT]]

#include "shaders/utilityLib/fragDeferredRead.glsl"

uniform samplerCube ambientMap;
varying vec2 texCoords;

void main( void )
{
	if( getMatID( texCoords ) == 0.0 )	// Background
	{
		gl_FragColor.rgb = vec3( 0, 0, 0 );
	}
	else if( getMatID( texCoords ) == 2.0 )	// Sky
	{
		gl_FragColor.rgb = getAlbedo( texCoords );
	}
	else
	{
		gl_FragColor.rgb = getAlbedo( texCoords ) * textureCube( ambientMap, getNormal( texCoords ) ).rgb;
	}
}


[[FS_LIGHTING]]
#include "shaders/utilityLib/fragLighting.glsl"
#include "shaders/utilityLib/fragDeferredRead.glsl"

#ifdef _F04_EnvMapping
	uniform samplerCube envMap;
#endif

uniform mat4 viewMat;
varying vec4 vpos;

void main( void )
{
    vec2 fragCoord = (vpos.xy / vpos.w) * 0.5 + 0.5;
	float mtl_id = getMatID( fragCoord );
	
	if( mtl_id == 1.0 )	// Standard phong material
	{
		vec3 pos = getPos( fragCoord ) + viewerPos;
		float vsPos = (viewMat * vec4( pos, 1.0 )).z;
		vec4 specParams = getSpecParams( fragCoord );
		
		vec3 normal = getNormal( fragCoord );
		vec3 final_color = calcPhongSpotLight( pos, normal, getAlbedo( fragCoord ), specParams.rgb, specParams.a, -vsPos, 0.3 );

		gl_FragColor.rgb = final_color ;
	
	}
#ifdef _F04_EnvMapping
	else if( mtl_id == 3.0 )	// Standard phong material
	{
		vec3 pos = getPos( fragCoord ) + viewerPos;
		float vsPos = (viewMat * vec4( pos, 1.0 )).z;
		vec4 specParams = getSpecParams( fragCoord );
		
		vec3 normal = getNormal( fragCoord );
		vec3 final_color = calcPhongSpotLight( pos, normal, getAlbedo( fragCoord ), specParams.rgb, specParams.a, -vsPos, 0.3 );
		
		vec3 refl = textureCube( envMap, reflect( pos.xyz - viewerPos, normalize( normal ) ) ).rgb;
		final_color += refl * 1.5;

		gl_FragColor.rgb = final_color ;
	}
#endif	
	else discard;
}


[[FS_COPY_DEPTH]]

uniform sampler2D depthBuf;
varying vec2 texCoords;

void main( void )
{
	gl_FragDepth = texture2D( depthBuf, texCoords ).r;
}
