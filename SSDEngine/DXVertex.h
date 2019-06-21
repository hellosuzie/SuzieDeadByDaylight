#pragma once
#include "DXHeader.h"

struct COLORVTX 
{
	Vec4				Pos;
	FColor				Color;
	Vec2				TexCoord;

	static UINT TypeSize() 
	{
		return (UINT)sizeof(COLORVTX);
	}
};

struct POSVTX
{
	Vec4				Pos;

	static UINT TypeSize()
	{
		return (UINT)sizeof(POSVTX);
	}
};

struct POSUVVTX
{
	Vec4				Pos;
	Vec2				TexCoord;

	static UINT TypeSize()
	{
		return (UINT)sizeof(POSUVVTX);
	}
};

struct COLORNORMALVTX
{
	Vec3				Pos;
	Vec3				Normal;
	FColor				Color;

	static UINT TypeSize()
	{
		return (UINT)sizeof(COLORNORMALVTX);
	}
};

struct BASICVTX 
{
	Vec3				Pos;
	Vec3				Normal;
	Vec2				TexCoord;

	static UINT TypeSize()
	{
		return (UINT)sizeof(BASICVTX);
	}
};

struct FBXSTATICVTX
{
	Vec3				Pos;
	Vec2				TexCoord;
	Vec4				Color;

	Vec3				Normal;
	Vec3				Tangent;
	Vec3				BiNormal;

	static UINT TypeSize()
	{
		return (UINT)sizeof(FBXSTATICVTX);
	}
};

struct FBXVTX : public  FBXSTATICVTX
{
	Vec4				Weights;
	Vec4				Indices;

	static UINT TypeSize()
	{
		return (UINT)sizeof(FBXVTX);
	}
};

// 지오 메트릭용 터레인 정점 구조체
struct GEOTERRAINVTX 
{
	Vec2				TerrainIdx;
	Vec4				TerrainHeight;			// LB LT RT RB 순으로의 테레인 높이
	Vec4				Color;					

	static UINT TypeSize()
	{
		return (UINT)sizeof(GEOTERRAINVTX);
	}
};

typedef struct _tagTERRAINVTX
{
	Vec4				HTerrainPos;
	Vec2				TexCoord;
	Vec3				Normal;
	Vec3				BiNormal;
	Vec3				Tangent;
	Vec3				VTerrainPos;

	static UINT TypeSize()
	{
		return (UINT)sizeof(_tagTERRAINVTX);
	}
}TERRAINVTX, TerrainVtx;


// 지오 메트릭용 터레인 정점 구조체
struct NAVITRIVTX
{
	// Postion : (x,y,z,_Idx)
	Vec4				TriVtx[3];
	Vec4				VtxColor;
	

	static UINT TypeSize()
	{
		return (UINT)sizeof(NAVITRIVTX);
	}

};

struct PARTICLEVTX
{
	Vec3				ParticlePos;
	Vec3				ParticleViewPos;
	float				ParticleTex;
	float				SizeX;
	float				SizeY;
	Vec2				ParticleTime;
	Vec4				Check;
	Vec3				AxisX;
	Vec3				AxisY;
	Vec4				AnimUV;

	static UINT TypeSize()
	{
		return (UINT)sizeof(PARTICLEVTX);
	}
};

struct IDX16 
{
	WORD _1, _2, _3;

public:
	static UINT MemSize()
	{
		return (UINT)sizeof(WORD);
	}
	static DXGI_FORMAT Format() 
	{
		return DXGI_FORMAT_R16_UINT;
	}

public:
	IDX16() :_1(0), _2(0), _3(0) {}
	IDX16(const IDX16& _Other)
		:_1(_Other._1),_2(_Other._2),_3(_Other._3)
	{}
	IDX16(const WORD& __1, const WORD& __2, const WORD& __3)
		:_1(__1), _2(__2) , _3(__3) {}
};

struct IDX32
{
public:
	union
	{
		struct
		{
			DWORD _1, _2, _3;
		};
		DWORD pIdx[3];
	};

public:
	static UINT MemSize()
	{
		return (UINT)sizeof(DWORD);
	}
	static DXGI_FORMAT Format()
	{
		return DXGI_FORMAT_R32_UINT;
	}
public:
	IDX32() :_1(0), _2(0), _3(0) {}
	IDX32(const IDX32& _Other)
		:_1(_Other._1), _2(_Other._2), _3(_Other._3)
	{}
	IDX32(const DWORD& __1, const DWORD& __2, const DWORD& __3)
		:_1(__1), _2(__2), _3(__3) {}
};

typedef class DXVTX 
{
public:
	static BASICVTX				BasicVtx[4];
	static BASICVTX				CubeVtx[6][4];
	static BASICVTX				FrustumVtx[6][4];

	static COLORVTX				ColorVtx[4];
	static COLORVTX				ColorCubeVtx[6][4];

	static POSUVVTX				TargetViewVtx[4];

	static IDX16				BasicIdx[2];
	static IDX32				BasicIdx32[2];

	static IDX16				CubeIdx[6][2];
	static IDX16				ColorCubeIdx[6][2];

}DXVtx;
