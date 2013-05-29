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

#ifndef _egRendererBase_H_
#define _egRendererBase_H_

#include "egPrerequisites.h"
#include "utMath.h"
#include "utOpenGL.h"
#include <string>
#include <vector>


namespace Horde3D {

const uint32 MaxNumVertexLayouts = 16;


// =================================================================================================
// GPUTimer
// =================================================================================================

class GPUTimer
{
public:
	GPUTimer();
	~GPUTimer();
	
	void beginQuery( uint32 frameID );
	void endQuery();
	bool updateResults();
	
	void reset();
	float getTimeMS() { return _time; }

private:
	std::vector < uint32 >  _queryPool;
	uint32                  _numQueries;
	uint32                  _queryFrame;
	float                   _time;
	bool                    _activeQuery;
};


// =================================================================================================
// Render Device Interface
// =================================================================================================

// ---------------------------------------------------------
// General
// ---------------------------------------------------------

template< class T > class RDIObjects
{
public:

	uint32 add( const T &obj )
	{
		if( !_freeList.empty() )
		{
			uint32 index = _freeList.back();
			_freeList.pop_back();
			_objects[index] = obj;
			return index + 1;
		}
		else
		{
			_objects.push_back( obj );
			return (uint32)_objects.size();
		}
	}

	void remove( uint32 handle )
	{
		ASSERT( handle > 0 && handle <= _objects.size() );
		
		_objects[handle - 1] = T();  // Destruct and replace with default object
		_freeList.push_back( handle - 1 );
	}

	T &getRef( uint32 handle )
	{
		ASSERT( handle > 0 && handle <= _objects.size() );
		
		return _objects[handle - 1];
	}

	T *get( uint32 idx )
	{
		if ( idx < _objects.size() )
			return &_objects[idx];
		
		return NULL;
	}

	friend class RenderDevice;

private:
	std::vector< T >       _objects;
	std::vector< uint32 >  _freeList;
};


struct DeviceCaps
{
	bool  texFloat;
	bool  texNPOT;
	bool  rtMultisampling;
};


// ---------------------------------------------------------
// Vertex layout
// ---------------------------------------------------------

struct VertexLayoutAttrib
{
	std::string  semanticName;
	uint32       vbSlot;
	uint32       size;
	uint32       offset;
	uint32		 type; //0 - FLOAT, 1 - UNSIGNED_BYTE_NORMALIZED
};

struct RDIVertexLayout
{
	uint32              numAttribs;
	VertexLayoutAttrib  attribs[16];
};


// ---------------------------------------------------------
// Buffers
// ---------------------------------------------------------

struct RDIBuffer
{
	uint32  type;
	uint32  glObj;
	uint32  size;
	void*   buffer;
};

struct RDIVertBufSlot
{
	uint32  vbObj;
	uint32  offset;
	uint32  stride;

	RDIVertBufSlot() : vbObj( 0 ), offset( 0 ), stride( 0 ) {}
	RDIVertBufSlot( uint32 vbObj, uint32 offset, uint32 stride ) :
		vbObj( vbObj ), offset( offset ), stride( stride ) {}
};


// ---------------------------------------------------------
// Textures
// ---------------------------------------------------------

struct TextureTypes
{
	enum List
	{
		Tex2D = GL_TEXTURE_2D,
		Tex3D = GL_TEXTURE_3D,
		TexCube = GL_TEXTURE_CUBE_MAP
	};
};

struct TextureFormats
{
	enum List
	{
		Unknown,
		BGRA8,
		DXT1,
		DXT3,
		DXT5,
		RGBA16F,
		RGBA32F,
		DEPTH,

        PVRTC2,
        PVRTCA2,
        PVRTC4,
        PVRTCA4,
		RGB5_A1,
		RGB565,
		RGBA4,
	};
};

struct RDITexture
{
	uint32                glObj;
	uint32                glFmt;
	int                   type;
	TextureFormats::List  format;
	int                   width, height, depth;
	int                   memSize;
	uint32                samplerState;
	bool                  sRGB;
	bool                  hasMips, genMips;
};

struct RDITexSlot
{
	uint32  texObj;
	uint32  samplerState;

	RDITexSlot() : texObj( 0 ), samplerState( 0 ) {}
	RDITexSlot( uint32 texObj, uint32 samplerState ) :
		texObj( texObj ), samplerState( samplerState ) {}
};


// ---------------------------------------------------------
// Shaders
// ---------------------------------------------------------

enum RDIShaderConstType
{
	CONST_FLOAT,
	CONST_FLOAT2,
	CONST_FLOAT3,
	CONST_FLOAT4,
	CONST_FLOAT44,
	CONST_FLOAT33
};

struct RDIInputLayout
{
	bool  valid;
	int8  attribIndices[16];
};

struct RDIShader
{
	uint32          oglProgramObj;
	RDIInputLayout  inputLayouts[MaxNumVertexLayouts];
};


// ---------------------------------------------------------
// Render buffers
// ---------------------------------------------------------

struct RDIRenderBuffer
{
	static const uint32 MaxColorAttachmentCount = 4;

	uint32  fbo, fboMS;  // fboMS: Multisampled FBO used when samples > 0
	uint32  width, height;
	uint32  samples;

	uint32  depthTex, colTexs[MaxColorAttachmentCount];
	uint32  depthBuf, colBufs[MaxColorAttachmentCount];  // Used for multisampling

	RDIRenderBuffer() : fbo( 0 ), fboMS( 0 ), width( 0 ), height( 0 ), depthTex( 0 ), depthBuf( 0 )
	{
		for( uint32 i = 0; i < MaxColorAttachmentCount; ++i ) colTexs[i] = colBufs[i] = 0;
	}
};


// ---------------------------------------------------------
// Render states
// ---------------------------------------------------------

enum RDISamplerState
{
	SS_FILTER_BILINEAR   = 0x0,
	SS_FILTER_TRILINEAR  = 0x0001,
	SS_FILTER_POINT      = 0x0002,
	SS_ANISO1            = 0x0,
	SS_ANISO2            = 0x0004,
	SS_ANISO4            = 0x0008,
	SS_ANISO8            = 0x0010,
	SS_ANISO16           = 0x0020,
	SS_ADDRU_CLAMP       = 0x0,
	SS_ADDRU_WRAP        = 0x0040,
	SS_ADDRU_CLAMPCOL    = 0x0080,
	SS_ADDRV_CLAMP       = 0x0,
	SS_ADDRV_WRAP        = 0x0100,
	SS_ADDRV_CLAMPCOL    = 0x0200,
	SS_ADDRW_CLAMP       = 0x0,
	SS_ADDRW_WRAP        = 0x0400,
	SS_ADDRW_CLAMPCOL    = 0x0800,
	SS_ADDR_CLAMP        = SS_ADDRU_CLAMP | SS_ADDRV_CLAMP | SS_ADDRW_CLAMP,
	SS_ADDR_WRAP         = SS_ADDRU_WRAP | SS_ADDRV_WRAP | SS_ADDRW_WRAP,
	SS_ADDR_CLAMPCOL     = SS_ADDRU_CLAMPCOL | SS_ADDRV_CLAMPCOL | SS_ADDRW_CLAMPCOL,
	SS_COMP_LEQUAL       = 0x1000
};

const uint32 SS_FILTER_START = 0;
const uint32 SS_FILTER_MASK = SS_FILTER_BILINEAR | SS_FILTER_TRILINEAR | SS_FILTER_POINT;
const uint32 SS_ANISO_START = 2;
const uint32 SS_ANISO_MASK = SS_ANISO1 | SS_ANISO2 | SS_ANISO4 | SS_ANISO8 | SS_ANISO16;
const uint32 SS_ADDRU_START = 6;
const uint32 SS_ADDRU_MASK = SS_ADDRU_CLAMP | SS_ADDRU_WRAP | SS_ADDRU_CLAMPCOL;
const uint32 SS_ADDRV_START = 8;
const uint32 SS_ADDRV_MASK = SS_ADDRV_CLAMP | SS_ADDRV_WRAP | SS_ADDRV_CLAMPCOL;
const uint32 SS_ADDRW_START = 10;
const uint32 SS_ADDRW_MASK = SS_ADDRW_CLAMP | SS_ADDRW_WRAP | SS_ADDRW_CLAMPCOL;
const uint32 SS_ADDR_START = 6;
const uint32 SS_ADDR_MASK = SS_ADDR_CLAMP | SS_ADDR_WRAP | SS_ADDR_CLAMPCOL;


// ---------------------------------------------------------
// Draw calls and clears
// ---------------------------------------------------------

enum RDIClearFlags
{
	CLR_COLOR = 0x00000001,
	CLR_DEPTH = 0x00000002
};

enum RDIIndexFormat
{
	IDXFMT_16 = GL_UNSIGNED_SHORT,
	IDXFMT_32 = GL_UNSIGNED_INT
};

enum RDIPrimType
{
	PRIM_TRILIST = GL_TRIANGLES,
	PRIM_TRISTRIP = GL_TRIANGLE_STRIP,
	PRIM_LINELIST = GL_LINES,
    // hack by antmanelr
    PRIM_PTLIST = GL_POINTS
};

// =================================================================================================


class RenderDevice
{
public:

	RenderDevice();
	virtual ~RenderDevice();
	
	void initStates();
	virtual bool init();
	void handleContextLost();
	
// -----------------------------------------------------------------------------
// Resources
// -----------------------------------------------------------------------------

	// Vertex layouts
	uint32 registerVertexLayout( uint32 numAttribs, VertexLayoutAttrib *attribs );
	
	// Buffers
	void beginRendering();
	uint32 createVertexBuffer( uint32 size, const void *data, bool sysmem = false);
	uint32 createIndexBuffer( uint32 size, const void *data );
	void destroyBuffer( uint32 bufObj );
	void updateBufferData( uint32 bufObj, uint32 offset, uint32 size, void *data );
	uint32 getBufferMem() { return _bufferMem; }

	// Textures
	uint32 calcTextureSize( TextureFormats::List format, int width, int height, int depth );
	uint32 createTexture( TextureTypes::List type, int width, int height, int depth, TextureFormats::List format,
	                      bool hasMips, bool genMips, bool compress, bool sRGB );
	void uploadTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels );
	void destroyTexture( uint32 texObj );
	void updateTextureData( uint32 texObj, int slice, int mipLevel, const void *pixels );
	bool getTextureData( uint32 texObj, int slice, int mipLevel, void *buffer );
	uint32 getTextureMem() { return _textureMem; }

	// Shaders
	uint32 createShader( const char *vertexShaderSrc, const char *fragmentShaderSrc );
	void destroyShader( uint32 shaderId );
	void bindShader( uint32 shaderId );
	std::string &getShaderLog() { return _shaderLog; }
	int getShaderConstLoc( uint32 shaderId, const char *name );
	int getShaderSamplerLoc( uint32 shaderId, const char *name );
	void setShaderConst( int loc, RDIShaderConstType type, void *values, uint32 count = 1 );
	void setShaderSampler( int loc, uint32 texUnit );
    uint32 getShaderNativeHandler(uint32 shaderId);

	// Renderbuffers
	uint32 createRenderBuffer( uint32 width, uint32 height, TextureFormats::List format,
	                           bool depth, uint32 numColBufs, uint32 samples, bool depthUseRenderBuffer = false );
	void destroyRenderBuffer( uint32 rbObj );
	uint32 getRenderBufferTex( uint32 rbObj, uint32 bufIndex );
	void setRenderBuffer( uint32 rbObj );
	bool getRenderBufferData( uint32 rbObj, int bufIndex, int *width, int *height,
	                          int *compCount, void *dataBuffer, int bufferSize );

	// Queries
	uint32 createOcclusionQuery();
	void destroyQuery( uint32 queryObj );
	void beginQuery( uint32 queryObj );
	void endQuery( uint32 queryObj );
	uint32 getQueryResult( uint32 queryObj );

// -----------------------------------------------------------------------------
// Commands
// -----------------------------------------------------------------------------
	
	void setViewport( int x, int y, int width, int height )
		{ _vpX = x; _vpY = y; _vpWidth = width; _vpHeight = height; _pendingMask |= PM_VIEWPORT; }
	void setScissorRect( int x, int y, int width, int height )
		{ _scX = x; _scY = y; _scWidth = width; _scHeight = height; _pendingMask |= PM_SCISSOR; }
	void setIndexBuffer( uint32 bufObj, RDIIndexFormat idxFmt )
		{ _indexFormat = (uint32)idxFmt; _newIndexBuf = bufObj; _pendingMask |= PM_INDEXBUF; }
	void setVertexBuffer( uint32 slot, uint32 vbObj, uint32 offset, uint32 stride )
		{ ASSERT( slot < 16 ); _vertBufSlots[slot] = RDIVertBufSlot( vbObj, offset, stride );
	      _pendingMask |= PM_VERTLAYOUT; }
	void setVertexLayout( uint32 vlObj )
		{ _newVertLayout = vlObj; }
	void setTexture( uint32 slot, uint32 texObj, uint16 samplerState )
		{ ASSERT( slot < 16 ); _texSlots[slot] = RDITexSlot( texObj, samplerState );
	      _pendingMask |= PM_TEXTURES; }
	void clearPendingMask( uint32 mask ) { _pendingMask &= ~mask; } 

	bool commitStates( uint32 filter = 0xFFFFFFFF );
	void resetStates();
	
	// Draw calls and clears
	void clear( uint32 flags, float *colorRGBA = 0x0, float depth = 1.0f );
	void draw( RDIPrimType primType, uint32 firstVert, uint32 numVerts );
	void drawIndexed( RDIPrimType primType, uint32 firstIndex, uint32 numIndices,
	                  uint32 firstVert, uint32 numVerts );

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

	const DeviceCaps &getCaps() { return _caps; }
	const RDIBuffer &getBuffer( uint32 bufObj ) { return _buffers.getRef( bufObj ); }
	const RDITexture &getTexture( uint32 texObj ) { return _textures.getRef( texObj ); }
	const RDIRenderBuffer &getRenderBuffer( uint32 rbObj ) { return _rendBufs.getRef( rbObj ); }

	friend class Renderer;

protected:

	enum RDIPendingMask
	{
		PM_VIEWPORT    = 0x00000001,
		PM_INDEXBUF    = 0x00000002,
		PM_VERTLAYOUT  = 0x00000004,
		PM_TEXTURES    = 0x00000008,
		PM_SCISSOR     = 0x00000010
	};

protected:

	uint32 createShaderProgram( const char *vertexShaderSrc, const char *fragmentShaderSrc );
	bool linkShaderProgram( uint32 programObj );
	void resolveRenderBuffer( uint32 rbObj );

	void checkGLError();
	bool applyVertexLayout();
	void applySamplerState( RDITexture &tex );

protected:

	DeviceCaps    _caps;
	
	uint32        _depthFormat;
	int           _vpX, _vpY, _vpWidth, _vpHeight;
	int           _scX, _scY, _scWidth, _scHeight;
	int           _fbWidth, _fbHeight;
	std::string   _shaderLog;
	uint32        _curRendBuf;
	int           _outputBufferIndex;  // Left and right eye for stereo rendering
	uint32        _textureMem, _bufferMem;
	GLint		  _maxTextureSize;
	GLint		  _maxCubeTextureSize;

	int                            _defaultFBO;
	uint32                         _numVertexLayouts;
	RDIVertexLayout                _vertexLayouts[MaxNumVertexLayouts];
	RDIObjects< RDIBuffer >        _buffers;
	RDIObjects< RDITexture >       _textures;
	RDIObjects< RDIShader >        _shaders;
	RDIObjects< RDIRenderBuffer >  _rendBufs;

	RDIVertBufSlot    _vertBufSlots[16];
	RDITexSlot        _texSlots[16];
	uint32            _prevShaderId, _curShaderId;
	uint32            _curVertLayout, _newVertLayout;
	uint32            _curIndexBuf, _newIndexBuf;
	uint32            _indexFormat;
	uint32            _activeVertexAttribsMask;
	uint32            _pendingMask;
};

}
#endif // _egRendererBase_H_
