[[FX]]

// Samplers
sampler2D causticMap = sampler_state
{
    Texture = "textures/common/black.tga";
    Address = Clamp;
};

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

// Uniforms
float cameraFov = 45.0;
float4 bgColor = {0.25, 0.25, 0.25, 0};
float4 cmat_r1 = {1, 0, 0, 0};
float4 cmat_r2 = {0, 1, 0, 0};
float4 cmat_r3 = {0, 0, 1, 0};
float4 cmat_r4 = {0, 0, 0, 1};

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

context SPHLIGHTING
{
	VertexShader = compile GLSL VS_VOLUME;
	PixelShader = compile GLSL FS_SPHLIGHTING;

	ZWriteEnable = true;
}

[[VS_FSQUAD]]

uniform mat4 projMat;
attribute vec3 vertPos;
varying vec2 texCoords;
				
void main( void )
{
	texCoords = vertPos.xy; 
	gl_Position = projMat * vec4( vertPos, 1 );
}


[[VS_VOLUME]]

uniform mat4 viewProjMat;
uniform mat4 worldMat;
attribute vec3 vertPos;
varying vec4 vpos;
				
void main( void )
{
	vpos = viewProjMat * worldMat * vec4( vertPos, 1 );
	gl_Position = vpos;
}


[[FS_AMBIENT]]

#include "shaders/utilityLib/fragDeferredRead.glsl"

uniform samplerCube ambientMap;
uniform vec4 bgColor;
varying vec2 texCoords;

void main( void )
{
	if( getMatID( texCoords ) == 0.0 )	// Background
	{
		gl_FragColor.rgb = bgColor.rgb;
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
		vec3 final_color = calcPhongSpotLight( pos, normal, getAlbedo( fragCoord ), specParams.rgb, specParams.a, -vsPos, 0.8 );

        gl_FragColor.rgb = final_color ;
	
	}
#ifdef _F04_EnvMapping
	else if( mtl_id == 3.0 )	// Standard phong material with environment mapping
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

[[FS_SPHLIGHTING]]

#include "shaders/utilityLib/fragLighting.glsl"
#include "shaders/utilityLib/fragSphUtils.glsl"

#ifdef _F04_EnvMapping
	uniform samplerCube envMap;
#endif
uniform sampler2D gbuf0;
// uniform sampler2D gbuf1;   // fluid material
uniform sampler2D gbuf2;   // background
uniform sampler2D gbuf3;   // thickness

uniform mat4 viewMatInv;
uniform mat4 projMat;
varying vec4 vpos;

void main( void )
{
    
	vec2 fragCoord = (vpos.xy / vpos.w) * 0.5 + 0.5;
	vec3 esPos = getEyePosEx(gbuf0, fragCoord);
	if(dot(esPos, esPos) < 0.001) discard ;
       
	vec3 pos = (viewMatInv * vec4(esPos, 1.0)).xyz;
    vec3 normal = reconstructNormalEx(esPos, gbuf0, fragCoord);
	
	float thickness = 0.1*getFloatFromRGBA8(texture2D(gbuf3, fragCoord));
	if (thickness == 0.00) {
	    gl_FragColor = texture2D(gbuf2, fragCoord);
		return;
	}
	
	// unpack parameters
	vec3 params = texture2D(gbuf0, fragCoord).gba;
	vec3 fluid_color = unpackFloat(params.x);
	vec4 fluid_color_falloff = vec4(unpackFloat(params.y)*2.0, 0.0);
	vec3 ior_flag = unpackFloat(params.z)*10.0;
	fluid_color_falloff.a = ior_flag.z;
	float IOR = ior_flag.x;

    vec4 absorbed_color = vec4(fluid_color, 1.0)*exp(-thickness*vec4(fluid_color_falloff.rgb, ior_flag.y));    
	vec3 reflect_color = absorbed_color.rgb;
	
#ifdef _F04_EnvMapping
	vec3 refl = textureCube( envMap, reflect( pos.xyz - viewerPos, normal ) ).rgb;
	reflect_color = refl;
#endif
    
    // refraction
    vec3 incident = normalize(pos - lightPos.xyz);
    vec3 refract_ray = refraction(incident, normal, 1.0/IOR, 1.0/(IOR*IOR)).xyz;
    vec4 new_pos = projMat * vec4(esPos + (thickness)*refract_ray, 1.0);
    vec3 bg_color = texture2D(gbuf2, (new_pos.xy/new_pos.w)*0.5 + 0.5).xyz;
    
    float NdotV = dot(-normalize(esPos), normal);
	vec2 fresnel = fresnelApprox(NdotV);
	
    vec3 albedo =  max(fresnel.y*0.5, 0.85)*bg_color;
    vec3 final_color = max(fresnel.x, 0.15)*reflect_color + absorbed_color.rgb + calcPhongSpotLight(pos, normal, vec3(0.0), vec3(1.0), 0.532, -esPos.z, 0.5);
    
    if (fluid_color_falloff.a > 0.001) {
        float alpha = clamp(absorbed_color.w, 0.0, 1.0);
        gl_FragColor.rgb =  mix(final_color, albedo, alpha) ;
    } else {
        // clear water
        gl_FragColor.rgb = final_color + albedo ;
    }
    gl_FragColor.a = 1.0;
	
	// alert the depth value in zbuffer
	vec4 clipSpacePos = projMat * vec4(esPos, 1.0);
	gl_FragDepth = (clipSpacePos.z / clipSpacePos.w)*0.5+0.5;
}
