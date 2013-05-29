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

// Uniforms
float cameraFov = 45.0;
float4 blurV = {1.0, 0.0, 0.0, 0.0};
float4 blurParams = {0, 0, 0, 0};

context BILATERALBLUR
{
    VertexShader = compile GLSL VS_FSQUAD;
    PixelShader = compile GLSL FS_BILATERALBLUR;

    ZWriteEnable = false;
}

// context CURVFLOW
// {
//     VertexShader = compile GLSL VS_FSQUAD;
//     PixelShader = compile GLSL FS_CURVFLOW;
// 
//     ZWriteEnable = false;
// }

context GAUSSBLUR
{
    VertexShader = compile GLSL VS_FSQUAD;
    PixelShader = compile GLSL FS_GAUSSBLUR;

    ZWriteEnable = false;
}

context RECOSNORMAL
{
    VertexShader = compile GLSL VS_FSQUAD;
    PixelShader = compile GLSL FS_RECONSNORMAL;
}

context COPY_DEPTH
{
	VertexShader = compile GLSL VS_FSQUAD;
	PixelShader = compile GLSL FS_COPY_DEPTH;
}

context COPY_TEXTURE
{
	VertexShader = compile GLSL VS_FSQUAD;
	PixelShader = compile GLSL FS_COPY_TEXTURE;
	ZEnable = false;
	ZWriteEnable = false;
}

[[VS_FSQUAD]]
// =================================================================================================
uniform mat4 projMat;
attribute vec3 vertPos;
varying vec2 texCoords;

void main( void )
{
    texCoords = vertPos.xy ;
    gl_Position = projMat * vec4( vertPos, 1 );
}

[[FS_BILATERALBLUR]]
// =================================================================================================
#include "shaders/utilityLib/fragSphUtils.glsl"

uniform sampler2D buf0;
uniform vec4 blurV;
uniform vec4 blurParams;
varying vec2 texCoords;

void main( void )
{	
    vec2 blurDir = vec2(blurV.x, 1.0-blurV.x);
    float filterRadius = blurParams.x;
    float blurScale = 2.0 / filterRadius;
    float blurDepthFalloff = blurParams.z;
    vec2 texelSize = vec2(1.0, 1.0) / frameBufSize ; 
    
    // bilateral filter
    if (0.0 == blurV.y) {
        // depth store in RGBA8
        float depth = getDepth(buf0, texCoords);
        if (depth > -0.001)  discard;
        float sum = 0.0;
        float wsum = 0.0;
        
        for(float x = -filterRadius; x <= filterRadius; x += 1.0) {
            float sample = getDepth(buf0, texCoords + x*texelSize*blurDir*blurParams.y);

            // spatial domain
            float r = x * blurScale;
            float w = exp(-r*r);

            // range domain
            float r2 = (sample - depth) * blurDepthFalloff; 
            r2 = r2*r2;
            float g = 0.0;
            if ( r2 < blurDepthFalloff*blurDepthFalloff* blurDepthFalloff) 
                g = exp(-r2);

            sum += sample * w * g; 
            wsum += w*g;
        }
        if (wsum > 0.0) {
            sum /= wsum;
        } 
        setDepth(sum);
        
    } else {
        // Depth store in R of RGBA32F
        vec4 depth_and_other = texture2D(buf0, texCoords);
        float depth = -1.0*depth_and_other.r;
        if (depth > -0.001)  discard;
        
        float sum = 0.0;
        float wsum = 0.0;
        for(float x = -filterRadius; x <= filterRadius; x += 1.0) {
            float sample = -1.0*texture2D(buf0, texCoords + x*texelSize*blurDir*blurParams.y).r;

            // spatial domain
            float r = x * blurScale;
            float w = exp(-r*r);

            // range domain
            float r2 = (sample - depth) * blurDepthFalloff; 
            r2 = r2*r2;
            float g = 0.0;
            if ( r2 < blurDepthFalloff*blurDepthFalloff* blurDepthFalloff) 
                g = exp(-r2);

            sum += sample * w * g; 
            wsum += w*g;
        }
        if (wsum > 0.0) {
            sum /= wsum;
        }
        
        gl_FragData[0].r = -1.0*sum;
        gl_FragData[0].gba = depth_and_other.gba;
    }
    
}

[[FS_CURVFLOW]]
// =================================================================================================
#include "shaders/utilityLib/fragSphUtils.glsl"

uniform sampler2D buf0;
varying vec2 texCoords;

void main( void )
{	
    float depth = getDepth(buf0, texCoords);
    if (depth > -0.001) {
        discard;
        return;
    }

    // calculate eye-space position from depth
    vec3 posEye = uv_to_eye(texCoords, depth);
    vec3 normal = reconstructNormal(posEye, buf0, texCoords);
    float dNdx = dFdx(normal.x);
    float dNdy = dFdy(normal.y);
    
    float flow = 0.5*(dNdx+dNdy);
    
    // boundary dNdx or dNdy is nan
    if (dNdx != dNdx || dNdy != dNdy) flow = 0.0;
    
    setDepth(depth - 0.003*flow);
}


[[FS_GAUSSBLUR]]
// =================================================================================================
#include "shaders/utilityLib/fragSphUtils.glsl"

uniform sampler2D buf0;
uniform vec4 blurV;
varying vec2 texCoords;

void main( void )
{	
    vec2 blurDir = vec2(blurV.x, 1.0-blurV.x);
    vec2 texelSize = blurDir * (vec2(1.0, 1.0) / frameBufSize) ; 
    
    vec2 uv = texCoords.st;
    vec4 val = texture2D( buf0, texCoords );
    vec4 color = vec4(0.0);
    
	color += texture2D( buf0, uv + vec2( -3.0*texelSize.x, -3.0*texelSize.y ) ) * 0.015625;
	color += texture2D( buf0, uv + vec2( -2.0*texelSize.x, -2.0*texelSize.y ) ) * 0.09375;
	color += texture2D( buf0, uv + vec2( -1.0*texelSize.x, -1.0*texelSize.y ) ) * 0.234375;
	color += val.x * 0.3125;
	color += texture2D( buf0, uv + vec2( 1.0*texelSize.x,  1.0*texelSize.y ) ) * 0.234375;
	color += texture2D( buf0, uv + vec2( 2.0*texelSize.x,  2.0*texelSize.y ) ) * 0.09375;
	color += texture2D( buf0, uv + vec2( 3.0*texelSize.x, -3.0*texelSize.y ) ) * 0.015625;
	
	gl_FragData[0] = color;
    // gl_FragData[0].bga = val.bga;
}


[[FS_RECONSNORMAL]]
// =================================================================================================
#include "shaders/utilityLib/fragSphUtils.glsl"

uniform sampler2D buf0;
varying vec2 texCoords;

void main()
{
    // read eye-space depth from texture
    float depth = getDepth(buf0, texCoords);
    if (depth > -0.001) {
        discard;
        return;
    }

    // calculate eye-space position from depth
    vec3 posEye = uv_to_eye(texCoords, depth);
    vec3 normal = reconstructNormal(posEye, buf0, texCoords);
    if (normal.z < 0.0) normal.z = 0.0;
    gl_FragData[0].xyz = normal;
    gl_FragData[1] = vec4(-depth/5.0);
}

[[FS_COPY_DEPTH]]

uniform sampler2D depthBuf;
varying vec2 texCoords;

void main( void )
{
	gl_FragDepth = texture2D( depthBuf, texCoords ).r;
}

[[FS_COPY_TEXTURE]]

uniform sampler2D buf0;
varying vec2 texCoords;

void main( void )
{
	gl_FragColor.rgb = texture2D( buf0, texCoords ).rgb;
	gl_FragColor.a = 0.0;
}
