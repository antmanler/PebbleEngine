// ****************************************************************************************
//
// Pebble Scene Editor
// --------------------------------------
// Copyright (C) 2011 Antmanler
//
// This file is part of the Pebble Scene Editor.
//
// The Pebble Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License
//
// The Pebble Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Credits : Volker Wiendl who created the major work of the original editor and Horde3D
// is a flexible graphics engine. see <http://www.horde3d.org> and <http://http://goo.gl/Me2HT>
//
// ****************************************************************************************


#ifndef _M_MATHS_H
#define _M_MATHS_H

#include <math.h>

// PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Degree/Radian conversions
#define DEG_TO_RAD ((M_PI * 2) / 360.0)
#define RAD_TO_DEG (1.0 / DEG_TO_RAD)

// MIN
#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

// MAX
#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

// CLAMP
#ifndef CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

// ABS
#ifndef ABS
#define ABS(a)  (((a) < 0) ? -(a) : (a))
#endif

// Power of two
#define isPowerOfTwo(x)	(((x&(x - 1)) == 0) && (x != 0))


// classes
class MVector2;
class MVector3;
class MVector4;
class MColor;
class MMatrix4x4;
class MQuaternion;

typedef MVector2         vec2;
typedef MVector3         vec3;
typedef MVector4         vec4;
typedef MColor           color;
typedef MMatrix4x4       mat4;
typedef MQuaternion      quaternion;

class MVector2
{
public:

	float x;
	float y;

public:

	MVector2(void):
		x(0.0f), 
		y(0.0f)
	{}

	MVector2(float newX, float newY):
		x(newX), 
		y(newY)
	{}

	MVector2(const float value):
		x(value), 
		y(value)
	{}

	MVector2(const float * values):
		x((*values)), 
		y((*values)+1)
	{}

	MVector2(const MVector2 & vec):
		x(vec.x), 
		y(vec.y)
	{}

	MVector2(const MVector3 & vec);

public:

	inline MVector2 operator + (const float value) const
	{	
		return MVector2(x + value, y + value);
	}

	inline MVector2 operator - (const float value) const
	{	
		return MVector2(x - value, y - value);
	}

	inline MVector2 operator + (const MVector2 & vec) const
	{	
		return MVector2(x + vec.x, y + vec.y);
	}

	inline MVector2 operator - (const MVector2 & vec) const
	{	
		return MVector2(x - vec.x, y - vec.y);
	}

	inline MVector2 operator * (const float value) const
	{	
		return MVector2(x * value, y * value);
	}
	
	inline MVector2 operator * (const MVector2 & vec) const
	{	
		return MVector2(x * vec.x, y * vec.y);
	}

	inline MVector2 operator / (const MVector2 & vec) const
	{	
		return MVector2(x / vec.x, y / vec.y);
	}

	inline MVector2 operator / (const float value) const
	{	
		if(value == 0.0f) return MVector2(0.0f, 0.0f);
		return MVector2(x / value, y / value);
	}

	friend inline MVector2 operator * (float factor, const MVector2 & vec) 
	{
		return vec * factor;
	}

	inline bool operator == (const MVector2 & vec) const
	{
		if(x == vec.x && y == vec.y){
			return true;
		}

		return false;
	}

	inline void operator = (const float value)
	{	
		x = value;	
		y = value;	
	}

	inline bool operator != (const MVector2 & vec) const
	{	
		return !((*this) == vec);
	}

	inline void operator += (const MVector2 & vec) 
	{	
		x += vec.x;
		y += vec.y;
	}

	inline void operator -= (const MVector2 & vec)
	{	
		x -= vec.x;	
		y -= vec.y;
	}

	inline void operator *= (const MVector2 & vec) 
	{	
		x *= vec.x;	
		y *= vec.y;
	}

	inline void operator /= (const MVector2 & vec) 
	{	
		x /= vec.x;	
		y /= vec.y;
	}

	inline void operator *= (const float value) 
	{	
		x *= value;	
		y *= value;
	}

	inline void operator /= (const float value) 
	{	
		if(value == 0.0f){
			x = 0.0f;
			y = 0.0f;
		}
		else
		{	
			x /= value; 
			y /= value;
		}
	}

	inline MVector2 operator - (void) const 
	{
		return MVector2(-x, -y);
	}

	inline MVector2 operator + (void) const 
	{
		return *this;
	}

	inline operator float* () const 
	{
		return (float*) this;
	}

	inline operator const float* () const 
	{
		return (const float*) this;
	}

public:

	inline void set(float newX, float newY)
	{	
		x = newX;	
		y = newY;
	}
	
	inline void loadIdentity(void)
	{
		x = 0.0f;
		y = 0.0f;
	}
	
	void normalize();
	void rotate(const float angle);	

	MVector2 getNormalized() const;
	MVector2 getRotated(const float angle) const;	

	inline float getAngleBetween(const MVector2 & vec)
	{
		float angle = (atan2(y, x) - atan2(vec.y, vec.x))*RAD_TO_DEG;
		
		if(angle > 180)
			angle -= 360;
		if(angle < -180)
			angle += 360;
		
		return angle;
	}

	inline float getLength() const
	{	
		return sqrtf((x*x) + (y*y));
	}

	inline float getSquaredLength() const
	{	
		return ((x*x) + (y*y));
	}

	inline float dotProduct(const MVector2 & vec) const
	{	
		return (x*vec.x) + (y*vec.y);
	}

	inline MVector2 lerp(const MVector2 & vec, float factor) const
	{	
		return ((*this) * (1.0f - factor)) + (vec * factor);
	}
};

class MVector3
{
public:

	float x;
	float y;
	float z;

public:

	MVector3(void):	
		x(0.0f), 
		y(0.0f), 
		z(0.0f)
	{}

	MVector3(float newX, float newY, float newZ):	
		x(newX), 
		y(newY), 
		z(newZ)
	{}

	MVector3(const float value):	
		x(value), 
		y(value), 
		z(value)
	{}

	MVector3(const float * values):	
		x(*(values)), 
		y(*(values+1)), 
		z(*(values+2))
	{}

	MVector3(const MVector3 & vec):	
		x(vec.x), 
		y(vec.y), 
		z(vec.z)
	{}

	MVector3(const MVector4 & vec);
	
	MVector3(const MVector2 & vec):	
		x(vec.x), 
		y(vec.y), 
		z(0)
	{}

public:

	inline MVector3 operator + (const MVector3 & vec) const
	{	
		return MVector3(x + vec.x, y + vec.y, z + vec.z);
	}
	
	inline MVector3 operator - (const MVector3 & vec) const
	{	
		return MVector3(x - vec.x, y - vec.y, z - vec.z);
	}

	inline MVector3 operator + (const float & value) const
	{	
		return MVector3(x + value, y + value, z + value);
	}

	inline MVector3 operator - (const float & value) const
	{	
		return MVector3(x - value, y - value, z - value);
	}

	inline MVector3 operator * (const float value) const
	{	
		return MVector3(x * value, y * value, z * value);
	}

	inline MVector3 operator * (const MVector3 & vec) const
	{	
		return MVector3(x * vec.x, y * vec.y, z * vec.z);
	}

	inline MVector3 operator / (const MVector3 & vec) const
	{	
		return MVector3(x / vec.x, y / vec.y, z / vec.z);
	}
	
	inline MVector3 operator / (const float value) const
	{	
		if(value == 0) return MVector3(0.0f, 0.0f, 0.0f);
		float i = 1.0f / value;
		return MVector3(x*i, y*i, z*i);
	}

	friend inline MVector3 operator * (float factor, const MVector3 & vec)
	{
		return vec * factor;
	}

	inline bool operator == (const MVector3 & vec) const
	{
		if(x == vec.x && y == vec.y && z == vec.z)
			return true;

		return false;
	}

	inline bool operator != (const MVector3 & vec) const
	{	
		return !((*this) == vec);
	}

	inline void operator += (const MVector3 & vec)
	{	
		x += vec.x;
		y += vec.y;	
		z += vec.z;
	}

	inline void operator -= (const MVector3 & vec)
	{	
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
	}

	inline void operator *= (const float value)
	{	
		x *= value;	
		y *= value;	
		z *= value;	
	}

	inline void operator = (const float value)
	{	
		x = value;	
		y = value;	
		z = value;	
	}
	
	inline void operator /= (const float value)
	{	
		if(value == 0.0f)
			return;

		float i = 1.0f / value;
		x *= i; 
		y *= i; 
		z *= i;	
	}

	inline MVector3 operator - (void) const
	{
		return MVector3(-x, -y, -z);
	}

	inline MVector3 operator + (void) const 
	{
		return *this;
	}

	inline operator float* () const 
	{
		return (float*) this;
	}

	inline operator const float* () const 
	{
		return (const float*) this;
	}

public:

	inline void set(const float newX, const float newY, const float newZ)
	{	
		x = newX;	
		y = newY;	
		z = newZ;
	}

	inline void loadIdentity(void)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	
	inline MVector3 crossProduct(const MVector3 & vec) const
	{	
		return MVector3((y*vec.z) - (z*vec.y), (z*vec.x) - (x*vec.z), (x*vec.y) - (y*vec.x));
	}

	inline float dotProduct(const MVector3 & vec) const
	{	
		return (x*vec.x) + (y*vec.y) + (z*vec.z);
	}
	
	void normalize();

	MVector3 getNormalized() const;

	inline float getLength() const
	{	
		return sqrtf((x*x) + (y*y) + (z*z));
	}
	
	inline float getSquaredLength() const
	{	
		return (x*x) + (y*y) + (z*z);
	}

	void rotateX(double angle);
	void rotateY(double angle);
	void rotateZ(double angle);
	void rotateAxis(double angle, const MVector3 & axis);

	MVector3 getRotatedX(double angle) const;
	MVector3 getRotatedY(double angle) const;
	MVector3 getRotatedZ(double angle) const;
	MVector3 getRotatedAxis(double angle, const MVector3 & axis) const;

	inline MVector3 lerp(const MVector3 & vec, float factor) const
	{	
		return ((*this) * (1.0f - factor)) + (vec * factor);
	}
};

class MVector4
{
public:

	float x;
	float y;
	float z;
	float w;

public:

	MVector4(void):
		x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{}

	MVector4(float newX, float newY, float newZ, float newW):
		x(newX),
		y(newY),
		z(newZ),
		w(newW)
	{}

	MVector4(const float value):
		x(value),
		y(value),
		z(value),
		w(value)
	{}
	
	MVector4(const float * values):
		x(*(values)),
		y(*(values + 1)),
		z(*(values + 2)),
		w(*(values + 3))
	{}

	MVector4(const MVector4 & vec):
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(vec.w)
	{}

	MVector4(const MVector3 & vec):
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(1.0f)
	{}

	//MVector4(const MColor & color);

public:
    
    MVector3 xyz() 
    {
        return MVector3(x, y, z);
    }

	MVector4 operator + (const MVector4 & vec) const
	{
		return MVector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	MVector4 operator - (const MVector4 & vec) const
	{
		return MVector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	MVector4 operator * (const float value) const
	{
		return MVector4(x * value, y * value, z * value, w * value);
	}

	MVector4 operator / (const float value) const
	{
		if(value == 0.0f) return MVector4(0.0f, 0.0f, 0.0f, 0.0f);
		return MVector4(x / value, y / value, z / value, w / value);
	}

	friend MVector4 operator * (float factor, const MVector4 & vec)
	{
		return vec * factor;
	}

	bool operator == (const MVector4 & vec) const
	{
		if(x == vec.x && y == vec.y && z == vec.z && w == vec.w){
			return true;
		}

		return false;
	}

	bool operator != (const MVector4 & vec) const
	{
		return !((*this) == vec);
	}

	void operator += (const MVector4 & vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
	}

	void operator -= (const MVector4 & vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
	}

	void operator *= (const float value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
	}

	void operator /= (const float value)
	{
		if(value == 0.0f){
			return;
		}
		else
		{
			x /= value;
			y /= value;
			z /= value;
			w /= value;
		}
	}

	MVector4 operator - (void) const
	{
		return MVector4(-x, -y, -z, -w);
	}

	MVector4 operator + (void) const
	{
		return (*this);
	}

	operator float* () const
	{
		return (float*) this;
	}

	operator const float* () const
	{
		return (const float*) this;
	}

	operator MVector3();

public:

	void set(float newX, float newY, float newZ, float newW)
	{
		x = newX;
		y = newY;
		z = newZ;
		w = newW;
	}

	void loadIdentity(void)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	float dotProduct(const MVector4 & vec)
	{
		return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
	}

	MVector4 lerp(const MVector4 & vec, float factor) const
	{
		return ((*this) * (1.0f - factor)) + (vec * factor);
	}
};

class MMatrix4x4
{
public:

	float entries[16];

public:

	MMatrix4x4(void){	
		loadIdentity();
	}

	MMatrix4x4(	float e0,	float e1,	float e2,	float e3,
				float e4,	float e5,	float e6,	float e7,
				float e8,	float e9,	float e10,	float e11,
				float e12,	float e13,	float e14,	float e15);

	MMatrix4x4(const float * value);
	MMatrix4x4(const MMatrix4x4 & mat);

	~MMatrix4x4(void){}

public:

	MMatrix4x4 operator + (const MMatrix4x4 & mat) const;
	MMatrix4x4 operator - (const MMatrix4x4 & mat) const;
	MMatrix4x4 operator * (const MMatrix4x4 & mat) const;
	MMatrix4x4 operator * (const float value) const;
	MMatrix4x4 operator / (const float value) const;
	
	friend MMatrix4x4 operator * (float factor, const MMatrix4x4 & mat);

	void operator += (const MMatrix4x4 & mat);
	void operator -= (const MMatrix4x4 & mat);
	void operator *= (const MMatrix4x4 & mat);
	void operator *= (const float value);
	void operator /= (const float value);

	bool operator == (const MMatrix4x4 & mat) const;
	bool operator != (const MMatrix4x4 & mat) const;

	MMatrix4x4 operator - (void) const;
	MMatrix4x4 operator + (void) const 
	{
		return (*this);
	}

	MVector4 operator * (const MVector4 mat) const;
	MVector3 operator * (const MVector3 mat) const;

	float & operator () (int col, int row){
		return entries[row*4+col];
	} 
    
    float operator () (int col, int row) const{
		return entries[row*4+col];
	}

	operator float* () const {
		return (float*) this;
	}

	operator const float* () const {
		return (const float*) this;
	}

public:

	void loadIdentity(void);
	void loadZero(void);

	void invert(void);
	void transpose(void);
	void affineInvert(void);
	void invertTranspose(void);
	void affineInvertTranspose(void);

	void scale(const MVector3 & scaleFactor);
	void rotate(const MVector3 & axis, const float angle);
	void translate(const MVector3 translate);

	void setEntry(int position, float value);
	void setScale(const MVector3 & scaleFactor);
	void setUniformScale(const float size);
	void setTranslation(const MVector3 & translation);
	void setTranslationPart(const MVector3 & translation);
	void setRotationAxis(const float angle, const MVector3 & axis);
	void setRotationX(const float angle);
	void setRotationY(const float angle);
	void setRotationZ(const float angle);
	void setRotationEuler(const float angleX, const float angleY, const float angleZ);
	void setRotationPartEuler(const float angleX, const float angleY, const float angleZ);
	void setRotationPartEuler(const MVector3 & rotations)
	{
		setRotationPartEuler(rotations.x, rotations.y, rotations.z);
	}

	float getEntry(int position) const;

	MVector3 getRotatedVector3(const MVector3 & vec) const;
	MVector3 getInverseRotatedVector3(const MVector3 & vec) const;
	MVector3 getTranslatedVector3(const MVector3 & vec) const;
	MVector3 getInversetranslatedVector3(const MVector3 & vec) const;

	MVector3 getTranslationPart(void){ return MVector3(entries[12], entries[13], entries[14]); }

	MVector3 getEulerAngles(void) const;

	MVector4 getRow(int position) const;
	MVector4 getColumn(int position) const;

	MMatrix4x4 getInverse(void) const;
	MMatrix4x4 getTranspose(void) const;
	MMatrix4x4 getInversetranspose(void) const;
	MMatrix4x4 getAffineInverse(void) const;
	MMatrix4x4 getAffineInverseTranspose(void) const;

	void rotateVector3(MVector3 & vec) const
	{
		vec = getRotatedVector3(vec);
	}

	void inverseRotateVector3(MVector3 & vec) const
	{
		vec = getInverseRotatedVector3(vec);
	}

	void translateVector3(MVector3 & vec) const
	{
		vec = getTranslatedVector3(vec);
	}

	void inversetranslateVector3(MVector3 & vec) const
	{
		vec = getInversetranslatedVector3(vec);
	}
};

class MQuaternion
{
public:

	float values[4];

public:

	MQuaternion(void)
	{
		loadIdentity();
	}

	MQuaternion(float val0, float val1, float val2, float val3)
	{
		values[0] = val0;
		values[1] = val1;
		values[2] = val2;
		values[3] = val3;
	}

	MQuaternion(float angle, const MVector3 & axis)
	{
		setFromAngleAxis(angle, axis);
	}

	MQuaternion(float xAngle, float yAngle, float zAngle)
	{
		setFromAngles(xAngle, yAngle, zAngle);
	}

	MQuaternion(const MQuaternion & q1, MQuaternion q2, float interpolation)
	{
		slerp(q1, q2, interpolation);
	}

public:

	MQuaternion operator * (const MQuaternion & quat) const;

	void operator *= (const MQuaternion & quat);

	MQuaternion operator - (void) const
	{
		return MQuaternion(-values[0], -values[1], -values[2], -values[3]);
	}

	inline bool operator == (const MQuaternion & quat) const
	{
		if(values[0] == quat.values[0] && 
		   values[1] == quat.values[1] &&
		   values[2] == quat.values[2] &&
		   values[3] == quat.values[3])
			return true;

		return false;
	}

	inline bool operator != (const MQuaternion & quat) const
	{	
		return !((*this) == quat);
	}

public:

	void normalize(void);
	void loadIdentity(void);
	void setFromAngles(float xAngle, float yAngle, float zAngle);
	void slerp(const MQuaternion & q1, const MQuaternion & q2, float interpolation);
	void invert(void);
	void setFromAngleAxis(float angle, const MVector3 & axis);
	void setFromVectors(const MVector3 & source, const MVector3 & destination);

	MVector3 getEulerAngles(void) const;

	float getAngle(void) const;
	MVector3 getAxis(void) const;
};

#endif
