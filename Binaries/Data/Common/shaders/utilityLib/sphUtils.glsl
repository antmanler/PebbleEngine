uniform vec2 frameBufSize;
uniform float cameraFov;

vec4 floatToRGBA8(const float value)
{
    const vec4 bitSh = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
    const vec4 mask = vec4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
    vec4 res = fract(value * bitSh);//error on ATI here?
    res -= res.xxyz * mask;
    return res;
}

float getFloatFromRGBA8(const vec4 value)
{
    const vec4 bitSh = vec4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    return(dot(value, bitSh));
}

const float _base_ = 100.0;

float packToFloat(vec3 c)
{   
    int value = int(floor(c.x * _base_ * _base_ * _base_));  
    value += int(floor(c.y * _base_ * _base_));  
    return (float(value) + c.z*_base_)*50.0 ;
}

// unpacks three 0..255 values from the lower-order bytes of a float and normalizes them
vec3 unpackFloat(float c)
{
    vec3 unpacked;
    float cRemaining = c/50.0;

    unpacked.x = floor(cRemaining/_base_/_base_/_base_*100.0)/100.0;
    unpacked.y = floor((cRemaining - (unpacked.x)*_base_*_base_*_base_)/_base_/_base_*100.0)/100.0;
    unpacked.z = floor((cRemaining - (unpacked.x)*_base_*_base_*_base_ - (unpacked.y)*_base_*_base_)/_base_*100.0)/100.0;
    return unpacked;
}

vec3 uv_to_eye(vec2 uv, float eye_z)
{
    float fovy = radians(cameraFov);
    float aspect = frameBufSize.x / frameBufSize.y;
    float invFocalLenX   = tan(fovy * 0.5) * aspect;
    float invFocalLenY   = tan(fovy * 0.5);
    uv = (uv * vec2(2.0, 2.0) - vec2(1.0, 1.0));
    return vec3(-1.0 * uv * vec2(invFocalLenX, invFocalLenY) * eye_z, eye_z);
}

float getDepth( sampler2D buf, const vec2 coord )
{
    vec4 vec = texture2D( buf, coord );
    return -1.0 * getFloatFromRGBA8(vec) * 1000.0;
}

vec3 getEyePos(sampler2D depthTex, vec2 uv) 
{
    float depth = getDepth(depthTex, uv);
    if (depth > 0.001) return vec3(0.0);
    return uv_to_eye(uv, depth).xyz;
}

vec3 getEyePosEx(sampler2D depthTex, vec2 uv) 
{
    float depth = -1.0*texture2D(depthTex, uv).r;
    if (depth > 0.001) return vec3(0.0);
    return uv_to_eye(uv, depth).xyz;
}

// Approximate the fresnel coefficients using Schlick's approximation
vec2 fresnelApprox( float cosAng )
{
    float oneMinus = 1.0-cosAng;
    float approx = 0.05 + 0.95*(oneMinus*oneMinus*oneMinus*oneMinus*oneMinus);
    return vec2( approx, 1.0-approx );
}

// A simple refraction shader similar to the built in GLSL one (only this one is real)
vec4 refraction( vec3 incident, vec3 normal, float ni_nt, float ni_nt_sqr )
{
    vec4 returnVal;
    float tmp = 1.0;
    float IdotN = dot( -incident, normal );
    // compute the cosine square of the theta_t
    float cosSqr = 1.0 - ni_nt_sqr*(1.0 - IdotN*IdotN);
    return ( cosSqr <= 0.0 ? 
        vec4( normalize(reflect( incident, normal )), -1.0 ) :
        vec4( normalize( ni_nt * incident + (ni_nt* IdotN - sqrt( cosSqr )) * normal ), 1.0 ) );
}

float simpleCAA(float r2)
{
    // We'll fix our Gaussian splat size at just under 3 pixels.  Set Gaussian distribution parameters.
    float splatSize = 2.5;
    float sizeSqr = splatSize*splatSize;
    float isInsideGaussian = 0.0;

    // Gaussian from Graphics Gems I, "Convenient anti-aliasing filters that minimize bumpy sampling"
    float alpha = 0.918;
    float beta_x2 = 3.906;     // == beta*2 == 1.953 * 2; 
    float denom = 0.858152111; // == 1 - exp(-beta); 
    float expResults = exp( -beta_x2*r2/sizeSqr );

    //  Are we even inside the Gaussian?
    isInsideGaussian = ( r2/sizeSqr < 0.25 ? 1.0 : 0.0 ) ;

    // Make sure the Gaussian intensity is properly normalized.
    float normalizeFactor = 10.5 * sizeSqr / 25.0;

    // Compute the Gaussian intensity
    expResults = alpha + alpha*((expResults-1.0)/denom);
    return isInsideGaussian * expResults / normalizeFactor / frameBufSize.x;
}

vec3 reconstructNormal(const vec3 posEye, const sampler2D buf0, const vec2 texCoords) 
{
   	vec2 texelSize = vec2(1.0, 1.0) / frameBufSize ;

    vec3 ddx = getEyePos(buf0, texCoords + vec2(texelSize.x, 0.0)) - posEye; 
    vec3 ddx2 = posEye - getEyePos(buf0, texCoords + vec2(-texelSize.x, 0.0)); 
   	if (abs(ddx.z) > abs(ddx2.z)) {
   		ddx = ddx2;
   	}

    vec3 ddy = getEyePos(buf0, texCoords + vec2(0.0, texelSize.y)) - posEye;
    vec3 ddy2 = posEye - getEyePos(buf0, texCoords + vec2(0.0, -texelSize.y));
   	if (abs(ddy2.z) < abs(ddy.z)) {
   		ddy = ddy2;
   	}

   	// calculate normal
   	vec3 n = cross(ddx, ddy);
   	return normalize(n);
}

vec3 reconstructNormalEx(const vec3 posEye, const sampler2D buf0, const vec2 texCoords) 
{
   	vec2 texelSize = vec2(1.0, 1.0) / frameBufSize ;

    vec3 ddx = getEyePosEx(buf0, texCoords + vec2(texelSize.x, 0.0)) - posEye; 
    vec3 ddx2 = posEye - getEyePosEx(buf0, texCoords + vec2(-texelSize.x, 0.0)); 
   	if (abs(ddx.z) > abs(ddx2.z)) {
   		ddx = ddx2;
   	}

    vec3 ddy = getEyePosEx(buf0, texCoords + vec2(0.0, texelSize.y)) - posEye;
    vec3 ddy2 = posEye - getEyePosEx(buf0, texCoords + vec2(0.0, -texelSize.y));
   	if (abs(ddy2.z) < abs(ddy.z)) {
   		ddy = ddy2;
   	}

   	// calculate normal
   	vec3 n = cross(ddx, ddy);
   	return normalize(n);
}