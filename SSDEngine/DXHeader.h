#pragma once

#ifndef _WINSOCK2API_
//#define _WINSOCK2API_
#include <WinSock2.h>
#endif 

#ifndef _MSWSOCK_
//#define _MSWSOCK_
#include <MSWSock.h>
#endif 


#include <Windows.h>

#include<d3d11.h>				// 다이렉트11 리소스및 자료형 헤더
#include<d3dcompiler.h>			// 쉐이더 컴파일용 헤더
#include<DirectXPackedVector.h> // 다이렉용 SIMD 방식을 사용하는 연산을 위한 벡터용 헤더

#include<DirectXTex.h>
#include<DirectXCollision.h>

#ifdef _DEBUG
	#ifdef X64
	#pragma comment(lib,"DirectXTex_Dx64")
	#pragma comment(lib,"DirectXFont_Dx64")
	#pragma comment(lib,"Recast_Dx64")
	#else 
	#pragma comment(lib,"DirectXTex_D")
	#pragma comment(lib,"DirectXFont_D")
	#pragma comment(lib,"Recast_D")
	#endif
#else
	#ifdef X64
	#pragma comment(lib,"DirectXTex_x64")
	#pragma comment(lib,"DirectXFont_x64")
	#pragma comment(lib,"Recast_x64")
	#else 
	#pragma comment(lib,"DirectXTex")
	#pragma comment(lib,"DirectXFont")
	#pragma comment(lib,"Recast")
	#endif
#endif



#pragma comment(lib,"d3d11")
#pragma comment(lib,"d3dcompiler")
#pragma comment(lib,"dxguid")




#define XMPI DirectX::XM_PI 
#define XMDPI DirectX::XM_DPI
#define XMHPI DirectX::XM_PIDIV2
#define XMQPI DirectX::XM_PIDIV4
#define D2R DirectX::XM_PI / 180.f
#define R2D 180.f / DirectX::XM_PI

struct _tagVEC3;
struct _tagVEC4;
typedef struct _tagVEC2
{
	union
	{
		DirectX::XMFLOAT2		XMF2;
		struct
		{
			float x;
			float y;
		};
		struct
		{
			float Start;
			float End;
		};
		float pf[2];

		struct
		{
			int ix;
			int iy;

			
		};
		int pi[2];
	};

public:
	operator DirectX::XMVECTOR() const 
	{
		return DirectX::XMLoadFloat2(&XMF2);
		
	}
	const float Cross(const _tagVEC2& _Other);
	const float Dot(const _tagVEC2& _Other);

public:
	_tagVEC2 operator=(const _tagVEC2& _Other);
	_tagVEC2 operator+(const _tagVEC2& _Other) const;
	_tagVEC2 operator+=(const _tagVEC2& _Other);
	_tagVEC2 operator-(const _tagVEC2& _Other) const;
	_tagVEC2 operator-=(const _tagVEC2& _Other);
	_tagVEC2 operator*(const _tagVEC2& _Other) const;
	_tagVEC2 operator*=(const _tagVEC2& _Other);

	_tagVEC2 operator*(const float& _Scala) const;
	_tagVEC2 operator*=(const float& _Scala);

	_tagVEC2 operator/(const float& _Scala) const;
	_tagVEC2 operator/=(const float& _Scala);


	_tagVEC2 operator*(const struct _tagMAT& _Mat) const;
	_tagVEC2 operator*=(const struct _tagMAT& _Mat);
public:
	const bool operator==(const _tagVEC2& _Other) const;
	const bool operator!=(const _tagVEC2& _Other) const;

public:
	const float Length() const;
	_tagVEC2 GetNormalizeVec() const;
	void Normalize();

public:
	_tagVEC2() : pf{ 0.f } {}
	_tagVEC2(const float& _X, const float& _Y)
		: x(_X), y(_Y)
	{}
	_tagVEC2(const int& _X, const int& _Y)
	{
		ix = _X;
		iy = _Y;
	}
	_tagVEC2(const _tagVEC2& _Other)
	{
		memcpy_s(pf, sizeof(pf), _Other.pf, sizeof(_Other.pf));
	}
	_tagVEC2(const DirectX::XMVECTOR& _XMVec) 
	{
		DirectX::XMStoreFloat2(&XMF2 ,_XMVec);
	}
	_tagVEC2(const POINT& _Point)
	{
		x = (float)_Point.x;
		y = (float)_Point.y;
	}
	~_tagVEC2() {}


} VEC2, Vec2 ,IVEC2 ,IVec2;

typedef struct _tagVEC3
{
	static _tagVEC3			Zero3;
	static _tagVEC3			One3;
	static _tagVEC3			Right3;
	static _tagVEC3			Left3;
	static _tagVEC3			Up3;
	static _tagVEC3			Down3;
	static _tagVEC3			Forward3;
	static _tagVEC3			Back3;



	union
	{
		DirectX::XMFLOAT3		XMF3;
		float pf[3];
		struct
		{
			float x;
			float y;
			float z;
		};
		int pi[3];
		struct
		{
			int ix;
			int iy;
			int iz;
		};
		_tagVEC2 vec2;
		struct
		{
			float r;
			float g;
			float b;
		};
	};

public:
	const _tagVEC3 operator-() const
	{
		_tagVEC3 ReturnVec;
		ReturnVec.x = -this->x;
		ReturnVec.y = -this->y;
		ReturnVec.z = -this->z;
		return ReturnVec;
	}

public:
	const bool operator==(const _tagVEC3& _Other) const
	{
		return (_Other.x == x && _Other.y == y && _Other.z == z);
	}

	const bool operator!=(const _tagVEC3& _Other) const
	{
		return (_Other.x != x || _Other.y != y || _Other.z != z);
	}


	_tagVEC3 operator=(const _tagVEC3& _Other);
	_tagVEC3 operator=(const _tagVEC2& _Other);
	_tagVEC3 operator=(const  DirectX::XMVECTOR& _XMVec);
	_tagVEC3 operator+(const _tagVEC3& _Other) const;
	_tagVEC3 operator+=(const _tagVEC3& _Other);
	_tagVEC3 operator-(const _tagVEC3& _Other) const;
	_tagVEC3 operator-=(const _tagVEC3& _Other);
	_tagVEC3 operator*(const _tagVEC3& _Other) const;
	_tagVEC3 operator*=(const _tagVEC3& _Other);

	_tagVEC3 operator*(const float& _Scala) const
	{
		return _tagVEC3(x*_Scala, y*_Scala, z*_Scala);
	}
	_tagVEC3 operator*=(const float& _Scala)
	{
		x *= _Scala;
		y *= _Scala;
		z *= _Scala;

		return *this;
	}

	_tagVEC3 operator/(const float& _Scala) const
	{
		return _tagVEC3(x / _Scala, y / _Scala, z / _Scala);
	}
	_tagVEC3 operator/=(const float& _Scala)
	{
		x /= _Scala;
		y /= _Scala;
		z /= _Scala;

		return *this;
	}

	_tagVEC3 operator*(const struct _tagMAT& _Mat) const;
	_tagVEC3 operator*=(const struct _tagMAT& _Mat);

public:
	operator DirectX::XMVECTOR() const
	{
		return DirectX::XMLoadFloat3(&XMF3);
	}


	operator DirectX::XMFLOAT3() const
	{
		return XMF3;
	}
	operator _tagVEC2() const 
	{
		return vec2;
	}
public:
	DirectX::XMFLOAT4 GetXMFloat4_Coord() const
	{
		DirectX::XMFLOAT4 Result;
		Result.x = x;
		Result.y = y;
		Result.z = z;
		Result.w = 1.f;
		return Result;
	}
	DirectX::XMFLOAT4 GetXMFloat4_Normal() const
	{
		DirectX::XMFLOAT4 Result;
		Result.x = x;
		Result.y = y;
		Result.z = z;
		Result.w = 0.f;
		return Result;
	}


	DirectX::XMVECTOR GetXMVector() const
	{
		return DirectX::XMLoadFloat3(&XMF3);
	}

public:
	const _tagVEC4 GetVec4_Normal() const;
	const _tagVEC4 GetVec4_Coord() const;
	const float Length() const;
	const float Length_Square() const;
	_tagVEC3 GetNormalizeVec() const ;

	const _tagVEC2 XY() const 
	{
		return _tagVEC2(x, y);
	}
	const _tagVEC2 XZ() const
	{
		return _tagVEC2(x, z);
	}
	const _tagVEC2 YZ() const
	{
		return _tagVEC2(y, z);
	}
	void Normalize();

public:
	_tagVEC3() : pf{ 0.f } {}
	_tagVEC3(const float& _X, const float& _Y, const float& _Z)
		: x(_X), y(_Y),z(_Z)
	{}
	_tagVEC3(const int& _iX, const int& _iY, const int& _iZ)
		:ix(_iX),iy(_iY),iz(_iZ)
	{
	}
	_tagVEC3(const _tagVEC3& _Other)
	{
		memcpy_s(pf, sizeof(pf), _Other.pf, sizeof(_Other.pf));
	}
	_tagVEC3(const _tagVEC2& _Vec2)
	{
		x = _Vec2.x;
		y = _Vec2.y;
		z = 0.f;
	}
	_tagVEC3(const _tagVEC2& _Vec2,const float& _Z)
	{
		x = _Vec2.x;
		y = _Vec2.y;
		z = _Z;
	}
	_tagVEC3(const DirectX::XMVECTOR& _XMVector);
	~_tagVEC3() {}

}VEC3, Vec3, IVec3;


typedef struct _tagVEC4
{

	typedef _tagVEC4 IVEC4;
	typedef _tagVEC4 Rect;
	typedef _tagVEC4 FCCOLOR;

	union
	{
		float pf[4];
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		DirectX::XMFLOAT4	XMF4;
		DirectX::XMFLOAT3	XMF3;

		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		struct
		{
			struct _tagVEC2		Pos;
			struct _tagVEC2		Size;
		};
		struct
		{
			struct _tagVEC2 Start;
			struct _tagVEC2 End;
		};
		struct 
		{
			struct _tagVEC2 StartUV;
			struct _tagVEC2 EndUV;
		};
		struct
		{
			struct _tagVEC2 LineStart;
			struct _tagVEC2 LineEnd;
		};
		int pi[4];
		struct
		{
			int ix;
			int iy;
			int iz;
			int iw;
		};
		struct
		{
			int ir;
			int ig;
			int ib;
			int ia;
		};
		_tagVEC2		vec2;
		_tagVEC3		vec3;
	};


public:
	static _tagVEC4 Zero;
	static _tagVEC4 One;
	static _tagVEC4 White;
	static _tagVEC4 Black;
	static _tagVEC4 Red;
	static _tagVEC4 Green;
	static _tagVEC4 Blue;
	static _tagVEC4 Yellow;
	static _tagVEC4 Cyan;
	static _tagVEC4 Magenta;
	static _tagVEC4 Pink;
	static _tagVEC4 PastelRed;
	static _tagVEC4 Mint;
	static _tagVEC4 LightYellow;
	static _tagVEC4 PastelGreen;
	static _tagVEC4 Lavender;
	static _tagVEC4 PastelMag;
	static _tagVEC4 PastelBlue;

public:
	_tagVEC4 operator-() const;
	_tagVEC4 operator=(const _tagVEC4& _Other) ;
	_tagVEC4 operator=(const _tagVEC3& _Vec3);
	_tagVEC4 operator=(const _tagVEC2& _Vec2);
	_tagVEC4 operator=(const DirectX::XMVECTOR& _XMVec);
	_tagVEC4 operator+(const _tagVEC4& _Other) const;
	_tagVEC4 operator+=(const _tagVEC4& _Other);
	_tagVEC4 operator-(const _tagVEC4& _Other) const;
	_tagVEC4 operator-=(const _tagVEC4& _Other);
	_tagVEC4 operator*(const _tagVEC4& _Other) const;
	_tagVEC4 operator*=(const _tagVEC4& _Other);
	_tagVEC4 operator*(const float& _Scala) const;
	_tagVEC4 operator*=(const float& _Scala);
	_tagVEC4 operator/(const float& _Scala) const;
	const bool operator==(const _tagVEC4& _Other) const;
	const bool operator!=(const _tagVEC4& _Other) const;

public:
	// 반지름
	const float Radius() const
	{
		return Size.x * 0.5f;
	}
	const float XSize() const
	{
		return Size.x;
	}
	const float YSize() const
	{
		return Size.y;
	}
	const float HXSize() const
	{
		return Size.x*0.5f;
	}
	const float HYSize() const
	{
		return Size.y*0.5f;
	}
	const float Left() const
	{
		return Pos.x - Size.x*0.5f;
	}
	const float Right() const
	{
		return Pos.x + Size.x*0.5f;
	}
	const float Top() const
	{
		return Pos.y + Size.y*0.5f;
	}
	const float Bottom() const
	{
		return Pos.y - Size.y*0.5f;
	}
	const Vec2 LeftTop() const
	{
		return Vec2(Pos.x - Size.x*0.5f, Pos.y + Size.y*0.5f);
	}
	const Vec2 LeftBottom() const
	{
		return Vec2(Pos.x - Size.x*0.5f, Pos.y - Size.y*0.5f);
	}
	const Vec2 RightTop() const
	{
		return Vec2(Pos.x + Size.x*0.5f, Pos.y + Size.y*0.5f);
	}
	const Vec2 RightBottom() const
	{
		return Vec2(Pos.x + Size.x*0.5f, Pos.y - Size.y*0.5f);
	}



public:
	_tagVEC4 operator*(const struct _tagMAT& _Mat) const;
	_tagVEC4 operator*=(const struct _tagMAT& _Mat);

public:
	operator DirectX::XMVECTOR() const 
	{
		return DirectX::XMLoadFloat4(&XMF4);
	}

	operator DirectX::XMFLOAT3() const
	{
		return XMF3;
	}


public:
	const struct _tagMAT GetQuatRotationMat();


public:
	IVEC4 GetIVec4() const;
	IVEC4 ConversionIVec4();

public:
	const float Length_Vec3() const;
	const _tagVEC3 GetNormalize_Vec3() const;
	void Normalize_Vec3();
public:
	const float Length() const;
	const _tagVEC4 GetNormalizeVec() const;
	void Normalize();


public:
	const POINT GetPoint() const;

public:
	const int GetARGB() const;
	const int GetRGBA() const;
	const int GetABGR() const;


public:
	_tagVEC4() : pf{ 0.f } {}
	_tagVEC4(const float& _X, const float& _Y, const float& _Z)
		: x(_X), y(_Y), z(_Z), w(1.f) {}
	_tagVEC4(const float& _X, const float& _Y, const float& _Z, const float& _W)
		: x(_X), y(_Y), z(_Z), w(_W) {}
	_tagVEC4(const int& _IX, const int& _IY, const int& _IZ)
		: ix(_IX), iy(_IY), iz(_IZ), iw(1) {}
	_tagVEC4(const int& _IX, const int& _IY, const int& _IZ, const int& _IW)
		: ix(_IX), iy(_IY), iz(_IZ), iw(_IW) {}
	_tagVEC4(const _tagVEC4& _Other)
		: x(_Other.x), y(_Other.y), z(_Other.z), w(_Other.w) {}
	_tagVEC4(const DirectX::XMVECTOR& _XMVec);
	_tagVEC4(const _tagVEC3& _Axis, const float& _Angle);
	_tagVEC4(const _tagVEC3& _Vec3);
	_tagVEC4(const _tagVEC2& _Vec2);

	~_tagVEC4() {}
} VEC4, Vec4, Rect, QUATERNION, Quat;

typedef _tagVEC4::IVEC4 IVEC4, IVec4;
typedef _tagVEC4::Rect Rect;
typedef _tagVEC4::FCCOLOR FCOLOR,FColor;


typedef enum AXIS_TYPE
{
	AXIS_X = 0,
	AXIS_Y,
	AXIS_Z,
	AXIS_U = 0,
	AXIS_V,  
	AXIS_N, 
	MAXAXIS,
}AXIS,Axis;

typedef struct _tagMAT
{
	union
	{
		DirectX::XMFLOAT4X4	 XMF4x4;
		struct _tagVEC4 Row[4];
		float M[4][4];
		float MArr[16];
		struct
		{
			float	_11;	float	_12;	float	_13;	float	_14;
			float	_21;	float	_22;	float	_23;	float	_24;
			float	_31;	float	_32;	float	_33;	float	_34;
			float	_41;	float	_42;	float	_43;	float	_44;
		};
	};

public:	
	// 월드 행렬을 넣어주면 크기,회전,이동 값을 분리해서 반환
	static void DivisionWorldMat(DirectX::XMVECTOR* _Scale, DirectX::XMVECTOR* _Rot, DirectX::XMVECTOR* _Pos, const _tagMAT& _World);
	static void DivisionWorldMat(_tagVEC3* _Scale, _tagVEC4* _Rot, _tagVEC3* _Pos, const _tagMAT& _World);


public:
	const bool operator==(const _tagMAT& _Other);
	void operator=(const _tagMAT& _Other);
	void operator=(const DirectX::XMFLOAT4X4& _Mat);
	const _tagMAT operator*(const _tagMAT& _Other) const;
	const _tagMAT operator*=(const _tagMAT& _Other);

	const Vec3 Mul_Normal(const Vec3& _Vec) const ;
	const Vec3 Mul_Coord(const Vec3& _Vec)const;
	const Vec4 MulVec4(const Vec4& _Vec) const;

public:
	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(&XMF4x4);
	}


public:
	void SetZeroMat();
	void SetIdentityMat();
	void SetScalingMat(const float& _X, const float& _Y,const float& _Z);
	void SetScalingMat(const _tagVEC4& _Scale);
	void SetScalingMat(const _tagVEC3& _Scale);
	void SetRotateXMat(const float& _Rad);
	void SetRotateYMat(const float& _Rad);
	void SetRotateZMat(const float& _Rad);
	void SetRotateXMat_ForDegree(const float& _Deg);
	void SetRotateYMat_ForDegree(const float& _Deg);
	void SetRotateZMat_ForDegree(const float& _Deg);
	void SetPosMat(const float& _X, const float& _Y, const float& _Z);
	void SetPosMat(const _tagVEC4& _Pos);
	void SetPosMat(const _tagVEC3& _Pos);
	void SetViewAtMat(const _tagVEC4& _EyePos, const _tagVEC4& _AtPos, const _tagVEC4& _UpDir);
	void SetViewAtMat(const DirectX::XMVECTOR& _EyePos, const DirectX::XMVECTOR& _AtPos, const DirectX::XMVECTOR& _UpDir);
	void SetViewToMat(const _tagVEC4& _Eye, const _tagVEC4& _AtDir, const _tagVEC4& _UpDir);
	void SetViewToMat(const DirectX::XMVECTOR&& _Eye, const DirectX::XMVECTOR&& _AtDir, const DirectX::XMVECTOR&& _UpDir);
	void SetOrthoProjMat(const float& _Width, const float& _Height, const float& _Near, const float& _Far);
	void SetFovPersProjMat(const float& _fov, const float& _Aspect, const float& _Near, const float& _Far);
	void SetFovPersProjMat_Angle(const float& _Width, const float& _Height, const float& _Near, const float& _Far);
	void SetPersProjMat(const float& _Width, const float& _Height, const float& _Near, const float& _Far);
	

public:
	const _tagMAT GetAxisMat() const
	{
		Mat ReturnMat;
		
		Vec3 Axis;
		for (size_t i = 0; i < 3; i++)
		{
			ReturnMat.Row[i] = Row[i].vec3.GetNormalizeVec();
		}

		return ReturnMat;
	}
	const _tagVEC4 GetScaleFactor()  const
	{
		_tagVEC4 Scale;

		Scale.x = Row[0].Length_Vec3();
		Scale.y = Row[1].Length_Vec3();
		Scale.z = Row[2].Length_Vec3();
		return Scale;
	}
	const _tagVEC4 GetAxisQuaternion() const
	{
		Mat AxisMat = GetAxisMat();
		return Vec4(DirectX::XMQuaternionRotationMatrix(AxisMat));
	}

public:
	 _tagMAT GetTransposeMat() const
	{
		_tagMAT Result{ DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&XMF4x4)) };
		return  Result;
	}
	void TransposeMat() 
	{
		float temp;
		for (size_t x = 0; x < 4; ++x)
		{
			for (size_t y = x + 1; y < 4; ++y)
			{
				temp = M[x][y];
				M[x][y] = M[y][x];
				M[y][x] = temp;
			}
		}
	}

	void InverseMat()
	{
		*this = DirectX::XMMatrixInverse(nullptr, *this);
		
	}
	_tagMAT GetInverseMat() const
	{
		Mat InvMat = *this;
		InvMat.InverseMat();
		return InvMat;

	}

public:
	const _tagVEC4 GetColmVec(const int& _Colm) const;
	//const _tagVEC4 GetBasisVec4(const AXIS_TYPE& Axis) const;
	//const _tagVEC3 GetBasisVec(const AXIS_TYPE& Axis) const;
	

public:
	_tagMAT() : M{ 0.f } { SetIdentityMat(); }
	_tagMAT(const _tagMAT& _Other)
	{
		memcpy_s(M, sizeof(M), _Other.M, sizeof(_Other.M));
	}

	_tagMAT(const _tagVEC3& _Row0, const _tagVEC3& _Row1, const _tagVEC3& _Row2)
	{
		SetIdentityMat();
		Row[0] = _Row0;
		Row[1] = _Row1;
		Row[2] = _Row2;
	}
	_tagMAT(const _tagVEC4& _Row0, const _tagVEC4& _Row1, const _tagVEC4& _Row2)
	{
		SetIdentityMat();
		Row[0] = _Row0;
		Row[1] = _Row1;
		Row[2] = _Row2;
	}
	_tagMAT(const DirectX::XMMATRIX& _XMMat)
	{
		DirectX::XMStoreFloat4x4(&XMF4x4, _XMMat);
	}
	~_tagMAT() {}

}MAT, MAT4, Mat, Mat4;





#define INFIVEC3 Vec3(D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX,D3D11_FLOAT32_MAX)


