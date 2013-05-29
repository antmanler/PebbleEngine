// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/legal/epl-v10.html
//
// *************************************************************************************************

#include "egTexture.h"
#include "egModules.h"
#include "egCom.h"
#include "egRenderer.h"
#include "utImage.h"
#include <cstring>

#include "utDebug.h"


namespace Horde3D {

using namespace std;

// *************************************************************************************************
// Class TextureResource
// *************************************************************************************************

#define FOURCC( c0, c1, c2, c3 ) ((c0) | (c1<<8) | (c2<<16) | (c3<<24))

#define DDSD_MIPMAPCOUNT      0x00020000

#define DDPF_ALPHAPIXELS      0x00000001
#define DDPF_FOURCC           0x00000004
#define DDPF_RGB              0x00000040

#define DDSCAPS2_CUBEMAP      0x00000200
#define DDSCAPS2_CM_COMPLETE  (0x00000400 | 0x00000800 | 0x00001000 | 0x00002000 | 0x00004000 | 0x00008000)
#define DDSCAPS2_VOLUME       0x00200000

#define D3DFMT_A16B16G16R16F  113
#define D3DFMT_A32B32G32R32F  116


struct DDSHeader
{
	uint32  dwMagic;
	uint32  dwSize;
	uint32  dwFlags;
	uint32  dwHeight, dwWidth;
	uint32  dwPitchOrLinearSize;
	uint32  dwDepth;
	uint32  dwMipMapCount;
	uint32  dwReserved1[11];

	struct {
		uint32  dwSize;
		uint32  dwFlags;
		uint32  dwFourCC;
		uint32  dwRGBBitCount;
		uint32  dwRBitMask, dwGBitMask, dwBBitMask, dwABitMask;
	} pixFormat;

	struct {
		uint32  dwCaps, dwCaps2, dwCaps3, dwCaps4;
	} caps;

	uint32  dwReserved2;
} ddsHeader;

struct PVRHeader
{
        uint32 dwHeaderSize;
        uint32 dwHeight;
        uint32 dwWidth;
        uint32 dwMipMapCount;
        uint32 dwFlags;
        uint32 dwTextureDataSize;
        uint32 dwBitCount;
        uint32 dwRBitMask;
        uint32 dwGBitMask;
        uint32 dwBBitMask;
        uint32 dwABitMask;
        uint32 dwPVR;
        uint32 dwNumSurfs;
} pvrHeader;

enum PVRPixType
{
        OGL_RGBA_4444= 0x10,
        OGL_RGBA_5551,
        OGL_RGBA_8888,
        OGL_RGB_565,
        OGL_RGB_555,
        OGL_RGB_888,
        OGL_I_8,
        OGL_AI_88,
        OGL_PVRTC2,
        OGL_PVRTC4,
        OGL_PVRTC2_2,
        OGL_PVRTC2_4,

        D3D_DXT1 = 0x20,
        D3D_DXT2,
        D3D_DXT3,
        D3D_DXT4,
        D3D_DXT5,

        D3D_RGB_332,
        D3D_AI_44,
        D3D_LVU_655,
        D3D_XLVU_8888,
        D3D_QWVU_8888,

        //10 bits per channel
        D3D_ABGR_2101010,
        D3D_ARGB_2101010,
        D3D_AWVU_2101010,

        //16 bits per channel
        D3D_GR_1616,
        D3D_VU_1616,
        D3D_ABGR_16161616,

        //HDR formats
        D3D_R16F,
        D3D_GR_1616F,
        D3D_ABGR_16161616F,

        //32 bits per channel
        D3D_R32F,
        D3D_GR_3232F,
        D3D_ABGR_32323232F,

        // Ericsson
        ETC_RGB_4BPP,
        ETC_RGBA_EXPLICIT,
        ETC_RGBA_INTERPOLATED,

        MGLPT_NOTYPE = 0xff

} pvrPixType;

const unsigned int PVRTEX_MIPMAP		= (1<<8);		// has mip map levels
const unsigned int PVRTEX_TWIDDLE		= (1<<9);		// is twiddled
const unsigned int PVRTEX_BUMPMAP		= (1<<10);		// has normals encoded for a bump map
const unsigned int PVRTEX_TILING		= (1<<11);		// is bordered for tiled pvr
const unsigned int PVRTEX_CUBEMAP		= (1<<12);		// is a cubemap/skybox
const unsigned int PVRTEX_FALSEMIPCOL	= (1<<13);		//
const unsigned int PVRTEX_VOLUME		= (1<<14);
const unsigned int PVRTEX_PIXELTYPE		= 0xff;			// pixel type is always in the last 16bits of the flags
const unsigned int PVRTEX_IDENTIFIER	= 0x21525650;	// the pvr identifier is the characters 'P','V','R'

unsigned char *TextureResource::mappedData = 0x0;
int TextureResource::mappedWriteImage = -1;
uint32 TextureResource::defTex2DObject = 0;
uint32 TextureResource::defTex3DObject = 0;
uint32 TextureResource::defTexCubeObject = 0;


void TextureResource::initializationFunc()
{
	unsigned char texData[] = 
		{ 128,192,255,255, 128,192,255,255, 128,192,255,255, 128,192,255,255,
		  128,192,255,255, 128,192,255,255, 128,192,255,255, 128,192,255,255,
		  128,192,255,255, 128,192,255,255, 128,192,255,255, 128,192,255,255,
		  128,192,255,255, 128,192,255,255, 128,192,255,255, 128,192,255,255 };

	// Upload default textures
	defTex2DObject = gRDI->createTexture( TextureTypes::Tex2D, 4, 4, 1,
	                                      TextureFormats::BGRA8, true, true, false, false );
	gRDI->uploadTextureData( defTex2DObject, 0, 0, texData );
	
	defTexCubeObject = gRDI->createTexture( TextureTypes::TexCube, 4, 4, 1,
	                                        TextureFormats::BGRA8, true, true, false, false );
	for( uint32 i = 0; i < 6; ++i ) 
	{
		gRDI->uploadTextureData( defTexCubeObject, i, 0, texData );
	}

	unsigned char *texData2 = new unsigned char[256];
	memcpy( texData2, texData, 64 ); memcpy( texData2 + 64, texData, 64 );
	memcpy( texData2 + 128, texData, 64 ); memcpy( texData2 + 192, texData, 64 );

	defTex3DObject = gRDI->createTexture( TextureTypes::Tex3D, 4, 4, 4,
	                                      TextureFormats::BGRA8, true, true, false, false );
	if ( defTex3DObject != 0)
		gRDI->uploadTextureData( defTex3DObject, 0, 0, texData2 );
	delete[] texData2;
}


void TextureResource::releaseFunc()
{
	gRDI->destroyTexture( defTex2DObject );
	gRDI->destroyTexture( defTex3DObject );
	gRDI->destroyTexture( defTexCubeObject );
}


TextureResource::TextureResource( const string &name, int flags ) :
	Resource( ResourceTypes::Texture, name, flags )
{
	_texType = TextureTypes::Tex2D;
	initDefault();
}


TextureResource::TextureResource( const string &name, uint32 width, uint32 height, uint32 depth,
                                  TextureFormats::List fmt, int flags ) :
	Resource( ResourceTypes::Texture, name, flags ),
	_width( width ), _height( height ), _depth( depth ), _rbObj( 0 )
{	
	_loaded = true;
	_texFormat = fmt;
	
	if( flags & ResourceFlags::TexRenderable )
	{
		_flags &= ~ResourceFlags::TexCubemap;
		_flags &= ~ResourceFlags::TexSRGB;
		_flags |= ResourceFlags::NoTexCompression;
		_flags |= ResourceFlags::NoTexMipmaps;

		_texType = TextureTypes::Tex2D;
		_sRGB = false;
		_hasMipMaps= false;
		_rbObj = gRDI->createRenderBuffer( width, height, fmt, false, 1, 0 ); 
		_texObject = gRDI->getRenderBufferTex( _rbObj, 0 );
	}
	else
	{
		uint32 size = gRDI->calcTextureSize( _texFormat, width, height, depth );
		unsigned char *pixels = new unsigned char[size];
		memset( pixels, 0, size );
		
		_texType = flags & ResourceFlags::TexCubemap ? TextureTypes::TexCube : TextureTypes::Tex2D;
		if( depth > 1 ) _texType = TextureTypes::Tex3D;
		_sRGB = (_flags & ResourceFlags::TexSRGB) != 0;
		_hasMipMaps = !(_flags & ResourceFlags::NoTexMipmaps);
		_texObject = gRDI->createTexture( _texType, _width, _height, _depth, _texFormat,
		                                  _hasMipMaps, _hasMipMaps, false, _sRGB );
		if (_texObject!=0)
			gRDI->uploadTextureData( _texObject, 0, 0, pixels );
		
		delete[] pixels;
		if( _texObject == 0 ) initDefault();
	}
}


TextureResource::~TextureResource()
{
	release();
}


void TextureResource::initDefault()
{
	_rbObj = 0;
	_texFormat = TextureFormats::BGRA8;
	_width = 0; _height = 0; _depth = 0;
	_sRGB = false;
	_hasMipMaps = true;
	
	if( _texType == TextureTypes::TexCube )
		_texObject = defTexCubeObject;
	else if( _texType == TextureTypes::Tex3D )
		_texObject = defTex3DObject;
	else
		_texObject = defTex2DObject;
}


void TextureResource::release()
{
	if( _rbObj != 0 )
	{
		// In this case _texObject is just points to the render buffer
		gRDI->destroyRenderBuffer( _rbObj );
	}
	else if( _texObject != 0 && _texObject != defTex2DObject && _texObject != defTexCubeObject )
	{
		gRDI->destroyTexture( _texObject );
	}

	_texObject = 0;
}


bool TextureResource::raiseError( const string &msg )
{
	// Reset
	release();
	initDefault();

	Modules::log().writeError( "Texture resource '%s': %s", _name.c_str(), msg.c_str() );
	
	return false;
}


bool TextureResource::checkDDS( const char *data, int size )
{
	return size > 128 && *((uint32 *)data) == FOURCC( 'D', 'D', 'S', ' ' );
}


bool TextureResource::loadDDS( const char *data, int size )
{
	ASSERT_STATIC( sizeof( DDSHeader ) == 128 );

	memcpy( &ddsHeader, data, 128 );
	
	// Check header
	// There are some flags that are required to be set for every dds but we don't check them
	if( ddsHeader.dwSize != 124 )
	{
		return raiseError( "Invalid DDS header" );
	}

	// Store properties
	_width = ddsHeader.dwWidth;
	_height = ddsHeader.dwHeight;
	_depth = 1;
	_texFormat = TextureFormats::Unknown;
	_texObject = 0;
	_sRGB = (_flags & ResourceFlags::TexSRGB) != 0;
	int mipCount = ddsHeader.dwFlags & DDSD_MIPMAPCOUNT ? ddsHeader.dwMipMapCount : 1;
	_hasMipMaps = mipCount > 1 ? true : false;

	// Get texture type
	if( ddsHeader.caps.dwCaps2 == 0 )
	{
		_texType = TextureTypes::Tex2D;
	}
	else if( ddsHeader.caps.dwCaps2 & DDSCAPS2_CUBEMAP )
	{
		if( (ddsHeader.caps.dwCaps2 & DDSCAPS2_CM_COMPLETE) != DDSCAPS2_CM_COMPLETE )
			raiseError( "DDS cubemap does not contain all cube sides" );
		_texType = TextureTypes::TexCube;
	}
	else if( ddsHeader.caps.dwCaps2 & DDSCAPS2_VOLUME )
	{
		_depth = ddsHeader.dwDepth;
		_texType = TextureTypes::Tex3D;
	}
	else
	{
		return raiseError( "Unsupported DDS texture type" );
	}
	
	// Get pixel format
	int blockSize = 1, bytesPerBlock = 4;
	enum { pfBGRA, pfBGR, pfBGRX, pfRGB, pfRGBX, pfRGBA } pixFmt = pfBGRA;
	
	if( ddsHeader.pixFormat.dwFlags & DDPF_FOURCC )
	{
		switch( ddsHeader.pixFormat.dwFourCC )
		{
		case FOURCC( 'D', 'X', 'T', '1' ):
			_texFormat = TextureFormats::DXT1;
			blockSize = 4; bytesPerBlock = 8;
			break;
		case FOURCC( 'D', 'X', 'T', '3' ):
			_texFormat = TextureFormats::DXT3;
			blockSize = 4; bytesPerBlock = 16;
			break;
		case FOURCC( 'D', 'X', 'T', '5' ):
			_texFormat = TextureFormats::DXT5;
			blockSize = 4; bytesPerBlock = 16;
			break;
		case D3DFMT_A16B16G16R16F: 
			_texFormat = TextureFormats::RGBA16F;
			bytesPerBlock = 8;
			break;
		case D3DFMT_A32B32G32R32F: 
			_texFormat = TextureFormats::RGBA32F;
			bytesPerBlock = 16;
			break;
		}
	}
	else if( ddsHeader.pixFormat.dwFlags & DDPF_RGB )
	{
		bytesPerBlock = ddsHeader.pixFormat.dwRGBBitCount / 8;
		
		if( ddsHeader.pixFormat.dwRBitMask == 0x00ff0000 &&
		    ddsHeader.pixFormat.dwGBitMask == 0x0000ff00 &&
		    ddsHeader.pixFormat.dwBBitMask == 0x000000ff ) pixFmt = pfBGR;
		else
		if( ddsHeader.pixFormat.dwRBitMask == 0x000000ff &&
		    ddsHeader.pixFormat.dwGBitMask == 0x0000ff00 &&
		    ddsHeader.pixFormat.dwBBitMask == 0x00ff0000 ) pixFmt = pfRGB;

		if( pixFmt == pfBGR || pixFmt == pfRGB )
		{
			if( ddsHeader.pixFormat.dwRGBBitCount == 24 )
			{
				_texFormat = TextureFormats::BGRA8;
			}
			else if( ddsHeader.pixFormat.dwRGBBitCount == 32 )
			{
				if( !(ddsHeader.pixFormat.dwFlags & DDPF_ALPHAPIXELS) ||
				    ddsHeader.pixFormat.dwABitMask == 0x00000000 )
				{
					_texFormat = TextureFormats::BGRA8;
					pixFmt = pixFmt == pfBGR ? pfBGRX : pfRGBX;
				}
				else
				{	
					_texFormat = TextureFormats::BGRA8;
					pixFmt = pixFmt == pfBGR ? pfBGRA : pfRGBA;
				}
			}
		}
	}

	if( _texFormat == TextureFormats::Unknown )
		return raiseError( "Unsupported DDS pixel format" );

	// Create texture
	_texObject = gRDI->createTexture( _texType, _width, _height, _depth, _texFormat,
	                                  mipCount > 1, false, false, _sRGB );
	
	// Upload texture subresources
	int numSlices = _texType == TextureTypes::TexCube ? 6 : 1;
	unsigned char *pixels = (unsigned char *)(data + 128);

	for( int i = 0; i < numSlices; ++i )
	{
		int width = _width, height = _height, depth = _depth;
		uint32 *dstBuf = 0x0;

		for( int j = 0; j < mipCount; ++j )
		{
			size_t mipSize = std::max( width / blockSize, 1 ) * std::max( height / blockSize, 1 ) *
			                 depth * bytesPerBlock;
			
			if( pixels + mipSize > (unsigned char *)data + size )
				return raiseError( "Corrupt DDS" );

			if( _texFormat == TextureFormats::BGRA8 && pixFmt != pfBGRA )
			{
				// Convert 8 bit DDS formats to BGRA
				uint32 pixCount = width * height * depth;
				if( dstBuf == 0x0 ) dstBuf = new uint32[pixCount * 4];
				uint32 *p = dstBuf;

				if( pixFmt == pfBGR )
					for( uint32 k = 0; k < pixCount * 3; k += 3 )
						*p++ = pixels[k+0] | pixels[k+1]<<8 | pixels[k+2]<<16 | 0xFF000000;
				else if( pixFmt == pfBGRX )
					for( uint32 k = 0; k < pixCount * 4; k += 4 )
						*p++ = pixels[k+0] | pixels[k+1]<<8 | pixels[k+2]<<16 | 0xFF000000;
				else if( pixFmt == pfRGB )
					for( uint32 k = 0; k < pixCount * 3; k += 3 )
						*p++ = pixels[k+2] | pixels[k+1]<<8 | pixels[k+0]<<16 | 0xFF000000;
				else if( pixFmt == pfRGBX )
					for( uint32 k = 0; k < pixCount * 4; k += 4 )
						*p++ = pixels[k+2] | pixels[k+1]<<8 | pixels[k+0]<<16 | 0xFF000000;
				else if( pixFmt == pfRGBA )
					for( uint32 k = 0; k < pixCount * 4; k += 4 )
						*p++ = pixels[k+2] | pixels[k+1]<<8 | pixels[k+0]<<16 | pixels[k+3]<<24;
				
				gRDI->uploadTextureData( _texObject, i, j, dstBuf );
			}
			else
			{
#ifdef PLATFORM_ES2
				// Swizzle RGBA -> BGRA
				uint32 *ptr = (uint32 *)pixels;
				for( uint32 k = 0, sk = width * height; k < sk; ++k )
				{
					uint32 col = *ptr;
					*ptr++ = (col & 0xFF00FF00) | ((col & 0x000000FF) << 16) | ((col & 0x00FF0000) >> 16);
				}
#endif
				gRDI->uploadTextureData( _texObject, i, j, pixels );
			}

			pixels += mipSize;
			if( width > 1 ) width >>= 1;
			if( height > 1 ) height >>= 1;
			if( depth > 1 ) depth >>= 1;
		}

		if( dstBuf != 0x0 ) delete[] dstBuf;
	}

	ASSERT( pixels == (unsigned char *)data + size );

	return true;
}


bool TextureResource::loadSTBI( const char *data, int size )
{
	bool hdr = false;
	if( stbi_is_hdr_from_memory( (unsigned char *)data, size ) > 0 ) hdr = true;
	
	int comps;
	void *pixels = 0x0;
	if( hdr )
		pixels = stbi_loadf_from_memory( (unsigned char *)data, size, &_width, &_height, &comps, 4 );
	else
		pixels = stbi_load_from_memory( (unsigned char *)data, size, &_width, &_height, &comps, 4 );

	if( pixels == 0x0 )
		return raiseError( "Invalid image format (" + string( stbi_failure_reason() ) + ")" );

// #ifdef PLATFORM_ES2
//  // Flip V
//  uint32 *ptr = (uint32 *)pixels;
//  uint32 *bptr = ((uint32 *)pixels) + _width * (_height-1);
//  for( uint32 j = 0, s = _height / 2; j < s; ++j)
//  {
//      for( int i = 0; i < _width; ++i)
//          std::swap(ptr[i], bptr[i]);
//      ptr += _width;
//      bptr -= _width;
//  }
// #else
//  // Flip V and Swizzle RGBA -> BGRA
//  uint32 *ptr = (uint32 *)pixels;
//  uint32 *bptr = ((uint32 *)pixels) + _width * (_height-1);
//  for( uint32 j = 0, s = _height / 2; j < s; ++j)
//  {
//      for( int i = 0; i < _width; ++i)
//      {
//          std::swap(*ptr, *bptr);
//          uint32 col = *ptr;
//          *ptr++ = (col & 0xFF00FF00) | ((col & 0x000000FF) << 16) | ((col & 0x00FF0000) >> 16);
//          ++bptr;
//      }
//      bptr -= 2 * _width;
//  }
// 
//  for( uint32 j = 0, s = _height - _height / 2 ; j < s; ++j)
//  {
//      for( int i = 0; i < _width; ++i)
//      {
//          uint32 col = *ptr;
//          *ptr++ = (col & 0xFF00FF00) | ((col & 0x000000FF) << 16) | ((col & 0x00FF0000) >> 16);
//      }
//  }
// #endif

#ifndef PLATFORM_ES2
	// Swizzle RGBA -> BGRA
	uint32 *ptr = (uint32 *)pixels;
	for( uint32 i = 0, si = _width * _height; i < si; ++i )
	{
		uint32 col = *ptr;
		*ptr++ = (col & 0xFF00FF00) | ((col & 0x000000FF) << 16) | ((col & 0x00FF0000) >> 16);
	}
#endif

	_texType = TextureTypes::Tex2D;
	_texFormat = hdr ? TextureFormats::RGBA16F : TextureFormats::BGRA8;
	_sRGB = (_flags & ResourceFlags::TexSRGB) != 0;
	_hasMipMaps = !(_flags & ResourceFlags::NoTexMipmaps);
	
	// Create and upload texture
	_texObject = gRDI->createTexture( _texType, _width, _height, _depth, _texFormat,
		_hasMipMaps, _hasMipMaps, !(_flags & ResourceFlags::NoTexCompression), _sRGB );
	if (_texObject!=0)
		gRDI->uploadTextureData( _texObject, 0, 0, pixels );

	stbi_image_free( pixels );

	return true;
}

bool TextureResource::checkPVR( const char *data, int size )
{
	return size > 52 && *((uint32 *)data) == FOURCC( 0x34, 0x00, 0x00, 0x00 );
}


bool TextureResource::loadPVR( const char *data, int size )
{
	// Load pvr
    ASSERT_STATIC( sizeof( PVRHeader ) == 52 );

    memcpy( &pvrHeader, data, 52 );

    // Check header
    if( pvrHeader.dwPVR != PVRTEX_IDENTIFIER )
    {
		return raiseError( "Invalid PVR header" );
    }

    // Store properties
    _width = pvrHeader.dwWidth;
    _height = pvrHeader.dwHeight;
	_depth = 0;
    _texFormat = TextureFormats::Unknown;
    _texObject = 0;
    _sRGB = (_flags & ResourceFlags::TexSRGB) != 0;
    int mipCount = pvrHeader.dwFlags & PVRTEX_MIPMAP ? (pvrHeader.dwMipMapCount+1) : 1;
    _hasMipMaps = mipCount > 1 ? true : false;

    // Get texture type
    if( pvrHeader.dwFlags & PVRTEX_CUBEMAP )
    {
		_texType = TextureTypes::TexCube;
    }
    else
    {
		_texType = TextureTypes::Tex2D;
    }

    // Get pixel format
    if( pvrHeader.dwABitMask > 0 )
    {
		switch( pvrHeader.dwFlags & PVRTEX_PIXELTYPE )
		{
		case OGL_RGBA_4444:
			_texFormat = TextureFormats::RGBA4;
			break;
		case OGL_RGBA_5551:
			_texFormat = TextureFormats::RGB5_A1;
			break;
		case OGL_RGBA_8888:
			_texFormat = TextureFormats::BGRA8;
			break;
		case OGL_PVRTC2:
			_texFormat = TextureFormats::PVRTCA2;
			break;
		case OGL_PVRTC4:
			_texFormat = TextureFormats::PVRTCA4;
			break;
		case D3D_DXT1:
			_texFormat = TextureFormats::DXT1;
			break;
		case D3D_DXT3:
			_texFormat = TextureFormats::DXT3;
			break;
		case D3D_DXT5:
			_texFormat = TextureFormats::DXT5;
			break;
		}
    }
    else
    {
		switch( pvrHeader.dwFlags & PVRTEX_PIXELTYPE )
		{
		case OGL_RGB_565:
			_texFormat = TextureFormats::RGB565;
			break;
		case OGL_PVRTC2:
			_texFormat = TextureFormats::PVRTC2;
			break;
		case OGL_PVRTC4:
			_texFormat = TextureFormats::PVRTC4;
			break;
		}
    }

	//handle missing pvrtc texture support
	TextureFormats::List  fileTexFormat = _texFormat;
	bool convertToBGRA8 = !glExt::EXT_texture_compression_pvrtc &&
        (_texFormat == TextureFormats::PVRTC2 || _texFormat == TextureFormats::PVRTC4 ||
        _texFormat == TextureFormats::PVRTCA2 || _texFormat == TextureFormats::PVRTCA4 );
	if (convertToBGRA8) _texFormat = TextureFormats::BGRA8;

    if( _texFormat == TextureFormats::Unknown )
            return raiseError( "Unsupported PVR pixel format" );

    // Create texture
    _texObject = gRDI->createTexture( _texType, _width, _height, _depth, _texFormat, mipCount > 1, false, false, _sRGB );

    // Upload texture subresources
    int numSlices = _texType == TextureTypes::TexCube ? 6 : 1;
    unsigned char *pixels = (unsigned char *)(data + 52);

    for( int i = 0; i < numSlices; ++i )
    {
            int width = _width, height = _height;

            for( int j = 0; j < mipCount; ++j )
            {
                    size_t mipSize =  gRDI->calcTextureSize( fileTexFormat, width, height, 0 );
                    if( pixels + mipSize > (unsigned char *)data + size )
                            return raiseError( "Corrupt PVR" );

					if (convertToBGRA8)
					{
						unsigned char* decompressedImg = new unsigned char[width*height*4];
						PVRTDecompressPVRTC(pixels, (_texFormat == TextureFormats::PVRTC2 || _texFormat == TextureFormats::PVRTCA2)?1:0, 
									width, height, decompressedImg);
	                     gRDI->uploadTextureData( _texObject, i, j, decompressedImg );

						delete[] decompressedImg;
					}
					else
					{
                     // Upload data directly
	                     gRDI->uploadTextureData( _texObject, i, j, pixels );
					}

                    pixels += mipSize;
                    if( width > 1 ) width >>= 1;
                    if( height > 1 ) height >>= 1;
            }
    }

    ASSERT( pixels == (unsigned char *)data + size );
	return true;
}

bool TextureResource::load( const char *data, int size )
{
	if( !Resource::load( data, size ) ) return false;

	if( checkDDS( data, size ) )
		return loadDDS( data, size );
	else if( checkPVR( data, size ) )
		return loadPVR( data, size );
	else
		return loadSTBI( data, size );
}

int TextureResource::getMipCount()
{
	if( _hasMipMaps )
		return ftoi_t( log( (float)std::max( _width, _height ) ) / log( 2.0f ) );
	else
		return 0;
}


int TextureResource::getElemCount( int elem )
{
	switch( elem )
	{
	case TextureResData::TextureElem:
		return 1;
	case TextureResData::ImageElem:
		return _texType == TextureTypes::TexCube ? 6 * (getMipCount() + 1) : getMipCount() + 1;
	default:
		return Resource::getElemCount( elem );
	}
}


int TextureResource::getElemParamI( int elem, int elemIdx, int param )
{
	switch( elem )
	{
	case TextureResData::TextureElem:
		switch( param )
		{
		case TextureResData::TexFormatI:
			return _texFormat;
		case TextureResData::TexSliceCountI:
			return _texType == TextureTypes::TexCube ? 6 : 1;
		}
		break;
	case TextureResData::ImageElem:
		switch( param )
		{
		case TextureResData::ImgWidthI:
			if( elemIdx < getElemCount( elem ) )
			{
				int mipLevel = elemIdx % (getMipCount() + 1);
				return std::max( 1, _width >> mipLevel );
			}
			break;
		case TextureResData::ImgHeightI:
			if( elemIdx < getElemCount( elem ) )
			{
				int mipLevel = elemIdx % (getMipCount() + 1);
				return std::max( 1, _height >> mipLevel );
			}
			break;
		}
		break;
	}
	
	return Resource::getElemParamI( elem, elemIdx, param );
}


void *TextureResource::mapStream( int elem, int elemIdx, int stream, bool read, bool write )
{
	if( (read || write) && mappedData == 0x0 )
	{
		if( elem == TextureResData::ImageElem && stream == TextureResData::ImgPixelStream &&
		    elemIdx < getElemCount( elem ) )
		{
			mappedData = Modules::renderer().useScratchBuf(
				gRDI->calcTextureSize( _texFormat, _width, _height, _depth ) );
			
			if( read )
			{	
				int slice = elemIdx / (getMipCount() + 1);
				int mipLevel = elemIdx % (getMipCount() + 1);
				gRDI->getTextureData( _texObject, slice, mipLevel, mappedData );
			}

			if( write )
				mappedWriteImage = elemIdx;
			else
				mappedWriteImage = -1;

			return mappedData;
		}
	}

	return Resource::mapStream( elem, elemIdx, stream, read, write );
}


void TextureResource::unmapStream()
{
	if( mappedData != 0x0 )
	{
		if( mappedWriteImage >= 0 )
		{
			int slice = mappedWriteImage / (getMipCount() + 1);
			int mipLevel = mappedWriteImage % (getMipCount() + 1);
			gRDI->updateTextureData( _texObject, slice, mipLevel, mappedData );
			mappedWriteImage = -1;
		}
		
		mappedData = 0x0;
		return;
	}

	Resource::unmapStream();
}

}  // namespace
