
#include "shaders/utilityLib/sphUtils.glsl"

void setDepth( const float depth )
{
   vec4 pAcked = floatToRGBA8( -1.0*depth / 1000.0 );
   gl_FragData[0] = pAcked;
}

