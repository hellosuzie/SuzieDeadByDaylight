#pragma once
#include "DXHeader.h"
#include "DXEnum.h"
#include <list>


class FbxDrawData
{
public:
	UINT Mesh;
	UINT Idx;
	UINT Mat;

public:
	FbxDrawData() : Mesh(0), Idx(0), Mat(0) {}
	FbxDrawData(const UINT _Mesh, const UINT _Idx, const UINT _Mat)
		: Mesh(_Mesh), Idx(_Idx), Mat(_Mat) {}
};


enum BASE2DTYPE
{
	BASE2D_POINT,
	BASE2D_RECT,
	BASE2D_CIRCLE,
};

#define MAXGAUSSIANMAP 256
#define MAXGAUSSIANRANGE 16



class CBUFFER
{
public:
	typedef struct _tagRENBASEBUFFER
	{
		Mat					World;
		Mat					InvWorld;
		Mat					NormalWorld;
		Mat					View;
		Mat					Proj;
		Mat					WV;
		Mat					NormalWV;
		Mat					VP;
		Mat					WVP;
		Vec4				ColorFactor;
		Vec4				CamWorldPos;

		_tagRENBASEBUFFER()
			:World(Mat()), View(Mat()), Proj(Mat()), WV(Mat()), VP(Mat()), WVP(Mat()), ColorFactor(Vec4()), CamWorldPos(Vec4())
		{}
		~_tagRENBASEBUFFER() {}
	}RenBaseBuffer;


	typedef struct _tagSPRITE2DBUFFER
	{
		Vec4	SpriteUV;

		_tagSPRITE2DBUFFER() :SpriteUV(Vec4()) {}
		_tagSPRITE2DBUFFER(const Vec4& _SpriteUV) : SpriteUV(_SpriteUV) {}
		~_tagSPRITE2DBUFFER() {}
	}Sprite2DBuffer;


	typedef struct _tagBASE2DBUFFER
	{
		Mat				WVP;
		Vec3			Color;
		int				Type;
		Vec4			BorderData;

		_tagBASE2DBUFFER() : WVP(Mat()), Color(1.f, 1.f, 1.f), Type(BASE2D_POINT), BorderData(Vec4()) {}
		_tagBASE2DBUFFER(const Vec3& _Color, const BASE2DTYPE& _Type, const Vec4& _BorderData)
			:Color(_Color), Type(_Type), BorderData(_BorderData), WVP(Mat())
		{}
		~_tagBASE2DBUFFER() {}
	} Base2DBuffer
		;


	typedef struct _tagGRIDBUFFER
	{
		int				Step;
		float			MinRange;
		float			MaxRange;
		float			Border;
		float			MinStep;
		float			MaxStep;
		float			CalMaxRange;
		float			Y;

		_tagGRIDBUFFER()
			: Step(0), MinRange(0.f), MaxRange(0.f), Border(0.f), MinStep(0.f), MaxStep(0.f), CalMaxRange(0.f), Y(0.f) {}
		~_tagGRIDBUFFER() {}
	}GridBuffer;

	typedef struct _tagDEBUGBUFFER
	{
		unsigned int	FiOption;
		float			BoarderSize;
		Vec2			FiFactor;
		Mat				FrustumMat;

		_tagDEBUGBUFFER() :FiOption(0), BoarderSize(1.f), FiFactor(Vec3()), FrustumMat(Mat()){}
		_tagDEBUGBUFFER(const unsigned int& _FiOp, const float& _BoarderSize) 
			: FiOption(0), BoarderSize(_BoarderSize), FiFactor(Vec2()) , FrustumMat(Mat()) {}
		~_tagDEBUGBUFFER() {}
	}DebugBuffer;

	typedef struct _tagDEBUGTARGETBUFFER
	{
		unsigned int	DivisionLevel[2];
		unsigned int	TargetPosition[2];


		_tagDEBUGTARGETBUFFER()
		{
			ZeroMemory(DivisionLevel, sizeof(DivisionLevel));
			ZeroMemory(TargetPosition, sizeof(TargetPosition));
		}
		~_tagDEBUGTARGETBUFFER() {}
	}DebugTargetBuffer;

	typedef struct _tagLIGHTBUFFER
	{
		Vec4			LightColor;
		Vec4			LightPos;
		Vec4			LightDir;
		Vec4			LightInvDir;
		Vec4			LightViewPos;
		Vec4			LightViewDir;
		Vec4			LightViewInvDir;
		// x : 초기 빛의 세기 , yzw : 감쇄 매개변수
		// 빛의 세기 I(d) = I(0) / (a0 + a1 * d + a2 * d * d) 
		Vec4			LightAtenuation;
		float			LightRange;
		float			LightPower;
		int				LightType;
		float			SpecularPower;
		float			SpotPower;
		float			AmbientFactor;
		Vec2			Temp;
		Mat				ViewMat;
		Mat				ProjMat;
		Mat				VP;

		_tagLIGHTBUFFER()
			:LightColor(Vec4::White), LightPos(Vec4(0.f, 0.f, 0.f, 1.f)), LightDir(Vec4())
			, LightInvDir(Vec4()), LightRange(0.f), LightPower(1.f), SpotPower(8.f)
			, LightType(MAX_LIGHT_TPYE), SpecularPower(32.f), AmbientFactor(0.05f)
			, LightAtenuation(Vec4::One), ViewMat(Mat()), ProjMat(Mat()), VP(Mat())
			, LightViewPos(Vec4()), LightViewDir(Vec4()), LightViewInvDir(Vec4())
		{}
		~_tagLIGHTBUFFER() {}
		_tagLIGHTBUFFER& operator= (const _tagLIGHTBUFFER& _Other)
		{
			LightColor = _Other.LightColor;
			LightPos = _Other.LightPos;
			LightDir = _Other.LightDir;
			LightInvDir = _Other.LightInvDir;
			LightViewPos = _Other.LightViewPos;
			LightViewDir = _Other.LightViewDir;
			LightViewInvDir = _Other.LightViewInvDir;
			LightAtenuation = _Other.LightAtenuation;
			LightRange = _Other.LightRange;
			LightPower = _Other.LightPower;
			LightType = _Other.LightType;
			SpecularPower = _Other.SpecularPower;
			SpotPower = _Other.SpotPower;
			AmbientFactor = _Other.AmbientFactor;
			Temp = _Other.Temp;
			ViewMat = _Other.ViewMat;
			ProjMat = _Other.ProjMat;
			VP = _Other.VP;
			return *this;
		}

	}LightBuffer;

	typedef struct _tagGAUSSIANBUFFER
	{
		Vec2			WinSize;
		int				Range;
		float			TotalGaussianMap;

		float			GaussianMap[MAXGAUSSIANMAP];

		_tagGAUSSIANBUFFER() : WinSize(Vec2()), Range(5), TotalGaussianMap(0.f)
		{
			ZeroMemory(GaussianMap, sizeof(GaussianMap));
		}
		~_tagGAUSSIANBUFFER() {}
		_tagGAUSSIANBUFFER& operator= (const _tagGAUSSIANBUFFER& _Other)
		{
			WinSize = _Other.WinSize;
			Range = _Other.Range;
			TotalGaussianMap = _Other.TotalGaussianMap;
			memcpy_s(GaussianMap, sizeof(GaussianMap), _Other.GaussianMap, sizeof(_Other.GaussianMap));
			return *this;
		}

	}GaussianBuffer;

	typedef struct _tagMINIMIZEBUFFER
	{
		// 축소 변환 작업에 대한 타입(일반 축소,휘도 추출 축소)
		unsigned int        MiniType;
		//빛의 총량이 얼만큼 이상이어야 출력하는지에 대한 값
		float				Amount;
		//빛의 양을 계산할 때 rgb를 몇의 비율로 해서 계산할것인지에 대한 값
		Vec3				Luminance;
		//추출한 빛을 몇배로 밝게 출력할것인지에 대한 값
		float               Multiple;
		float               None1;
		float               None2;

		_tagMINIMIZEBUFFER() : MiniType(MINIMIZE_BASE),Amount(0.f), Luminance(Vec3(0.33333333334f, 0.33333333334f, 0.33333333334f)), Multiple(1.f)
			, None1(0.f), None2(0.f)
		{
		}
		~_tagMINIMIZEBUFFER() {}
		_tagMINIMIZEBUFFER& operator= (const _tagMINIMIZEBUFFER& _Other)
		{
			MiniType = _Other.MiniType;
			Amount = _Other.Amount;
			Luminance = _Other.Luminance;
			Multiple = _Other.Multiple;
			return *this;
		}

	}MinimizeBuffer;

	typedef struct _tagBLOOMBUFFER
	{
		float			TargetSize;
		float			None1;
		float			None2;
		float			None3;

		Vec2			WinSize;
		int				Range;
		float			TotalGaussianMap;

		float			GaussianMap[MAXGAUSSIANMAP];

		_tagBLOOMBUFFER(float _TargetSize) : TargetSize(_TargetSize), None1(0.f), None2(0.f), None3(0.f), WinSize(Vec2()), Range(5), TotalGaussianMap(0.f)
		{
			ZeroMemory(GaussianMap, sizeof(GaussianMap));
		}
		_tagBLOOMBUFFER() : TargetSize(MINIMIZE_BUFFER_TYPE::MINI_64), None1(0.f), None2(0.f), None3(0.f), WinSize(Vec2()), Range(5), TotalGaussianMap(0.f)
		{
			ZeroMemory(GaussianMap, sizeof(GaussianMap));
		}
		~_tagBLOOMBUFFER() {}
		_tagBLOOMBUFFER& operator= (const _tagBLOOMBUFFER& _Other)
		{
			TargetSize = _Other.TargetSize;
			WinSize = _Other.WinSize;
			Range = _Other.Range;
			TotalGaussianMap = _Other.TotalGaussianMap;
			memcpy_s(GaussianMap, sizeof(GaussianMap), _Other.GaussianMap, sizeof(_Other.GaussianMap));
			return *this;
		}

	}BloomBuffer;

	typedef struct _tagTERRAINBUFFER
	{
		int				FloorCount;
		float			SizeX;
		float			SizeZ;

		float			None1;

		_tagTERRAINBUFFER() : FloorCount(0), SizeX(0.f), SizeZ(0.f), None1(0.f)
		{
		}
		~_tagTERRAINBUFFER() {}
	}TerrainBuffer;

	typedef struct _tagGIZMOBUFFER
	{
		Mat GizmoMat;

		_tagGIZMOBUFFER(): GizmoMat(Mat()){}
		~_tagGIZMOBUFFER() {}
	}GizmoBuffer;

	typedef struct _tagSKINNINGBUFFER
	{
		UINT			BoneCount;
		UINT			KeyFrameCount;
		UINT			CurFrame;
		UINT			NextFrame;
		float			LerpPara;
		Vec3			Padding;

		_tagSKINNINGBUFFER() : BoneCount(0), KeyFrameCount(0) , CurFrame(0) , NextFrame(0), LerpPara(0.f) {}
		~_tagSKINNINGBUFFER() {}
	}SkinningBuffer;


	typedef struct _tagNAVIAREABUFFER 
	{
		Mat				AreaWVP;

	public:
		_tagNAVIAREABUFFER()
			:AreaWVP(Mat())
		{
		}
		~_tagNAVIAREABUFFER() 
		{
		}

	}NaviAreaBuffer;

	typedef struct _tagFOGBUFFER
	{
		Vec3			FogColor;
		float			FogStartDepth;
		Vec3			FogHighlightColor;
		float			FogGlobalDensity;
		Vec3			FogSunDir;
		float			FogHeightFallOff;

	public:
		_tagFOGBUFFER() 
			: FogColor(Vec3::Zero3), FogStartDepth(800.f), FogHighlightColor(Vec3::Zero3), FogGlobalDensity(100), FogSunDir(Vec3(0.f, -1.f, 0.f)), FogHeightFallOff(1)		{
		}
		~_tagFOGBUFFER()
		{
		}

	}FogBuffer;

	typedef struct _tagBASEFBXBUFFER
	{
		bool			EmivCheck;
		bool			Test1;
		bool			Test2;
		bool			Test3;

		Vec3			Test4;

	public:
		_tagBASEFBXBUFFER() 
			: EmivCheck(false), Test1(false), Test2(false), Test3(false), Test4(Vec3())
		{
		}
		_tagBASEFBXBUFFER(bool _EmivCheck)
			: EmivCheck(_EmivCheck), Test1(false), Test2(false), Test3(false), Test4(Vec3())
		{
		}
		~_tagBASEFBXBUFFER()
		{
		}

	}BaseFbxBuffer;

	typedef struct _tagMATERIALCOLOR
	{
		Vec4			Diffuse;
		Vec4			Specular;
		Vec4			Ambient;
		Vec4			Emissive;

	public:
		_tagMATERIALCOLOR()
			: Diffuse(Vec4()), Specular(Vec4()), Ambient(Vec4()), Emissive(Vec4())
		{
		}
		_tagMATERIALCOLOR(Vec4 _Emv)
			: Diffuse(Vec4()), Specular(Vec4()), Ambient(Vec4()), Emissive(_Emv)
		{
		}
		~_tagMATERIALCOLOR()
		{
		}

	}MaterialColor;

	typedef struct _tagPARTICLEBUFFER
	{
		Vec4			AxisX;
		Vec4			AxisY;
		Vec4			AccTime;
		Vec4			ParticleColor;

	public:
		_tagPARTICLEBUFFER() : AxisX(Vec4()), AxisY(Vec4()), AccTime(Vec4()), ParticleColor(Vec4::One)
		{
		}
		~_tagPARTICLEBUFFER()
		{
		}

	}ParticleBuffer;

	typedef struct _tagFORWARDLIGHTBUFFER
	{
		_tagLIGHTBUFFER				ArrLightData[128];
		int							LightCount;
		int							Temp1;
		int							Temp2;
		int							Temp3;

	public:
		_tagFORWARDLIGHTBUFFER() : LightCount(0), Temp1(0), Temp2(0), Temp3(0)
		{
			ZeroMemory(ArrLightData, sizeof(ArrLightData));
		}
		~_tagFORWARDLIGHTBUFFER()
		{
		}

	}ForwardLightBuffer;

	typedef struct _tagDISSOLVEBUFFER
	{
		unsigned int				Active;
		float						Progess;
		float						Temp1;
		float						Temp2;


	public:
		_tagDISSOLVEBUFFER() : Active(0), Progess(0.f), Temp1(0.f), Temp2(0.f)
		{
			
		}
		~_tagDISSOLVEBUFFER()
		{
		}

	}DissolveBuffer;

	typedef struct _tagDECALBUFFER
	{
		Mat							WorldViewInv;

	public:
		_tagDECALBUFFER() : WorldViewInv()
		{

		}
		~_tagDECALBUFFER()
		{
		}

	}DecalBuffer;

	typedef struct _tagUIBUFFER
	{
		Vec4						UIColorFactor;

	public:
		_tagUIBUFFER() : UIColorFactor(Vec4::One)
		{

		}
		~_tagUIBUFFER()
		{
		}

	}UIBuffer;

	/*struct TilePixelBuffer 
	{
		Vec4		ColorFactor;
		IVec4		Options;


		TilePixelBuffer() : ColorFactor(Vec4()),Options(IVec4()){}
		~TilePixelBuffer() {}
	};


	struct ShadowTraceBuffer
	{
		Vec4	PlayerUV;
		Vec4	TileUV;
		Vec4	PlayerUVRatio;
		Vec4	RotateAngle;
		IVec4   Options;

		ShadowTraceBuffer() :TileUV(Vec4()), PlayerUVRatio(Vec4()) , PlayerUV(Vec4()), RotateAngle(Vec4()), Options(IVec4()){}
		ShadowTraceBuffer(const Vec4& _PlayerUV, const Vec4& _TileUV, const Vec4& _PlayerUVRatio,const Vec4& _RotAngle)
			: PlayerUV(_PlayerUV),TileUV(_TileUV), PlayerUVRatio(_PlayerUVRatio), RotateAngle(_RotAngle)
			, Options(IVec4())
		{}
		~ShadowTraceBuffer() {}
	};

	struct GlobalInstanceDuffer
	{
		Mat				VPMat;
		Vec4			TileData;

		GlobalInstanceDuffer() :VPMat(Mat()),TileData(Vec4()){}
		GlobalInstanceDuffer(const Mat& _VP,const Vec4& _Data) :VPMat(_VP), TileData(_Data) {}
		~GlobalInstanceDuffer() {}
	};

	struct CParticleBuffer
	{
		Mat				VPMat;
		Vec4			ParticleGlobalValue;

		CParticleBuffer() : VPMat(Mat()),ParticleGlobalValue(Vec4()) { }
		CParticleBuffer(const Mat& _VP, const Vec4& _GlobalValue)  : VPMat(_VP),ParticleGlobalValue(_GlobalValue) {}
		~CParticleBuffer() {}
	};

	struct TrailBuffer
	{
		Mat				VPMat;
		Vec4			SpriteUV;
		Vec4			TrailData;
		Vec4			PerFrameData;

		TrailBuffer()
			: VPMat(Mat()), SpriteUV(Vec4()), TrailData(Vec4()), PerFrameData(Vec4())
		{
		}
		TrailBuffer(const Mat& _VP, const Vec4& _SpriteUV)
			: VPMat(_VP), SpriteUV(_SpriteUV), TrailData(Vec4()), PerFrameData(Vec4())
		{}
		~TrailBuffer() {}
	};

	struct AfterImageBuffer
	{
		Mat				VPMat;
		Vec4			SpriteUV;
		Vec4			AfterImageData;

		AfterImageBuffer()
			: VPMat(Mat()), SpriteUV(Vec4()), AfterImageData(Vec4())
		{
		}
		AfterImageBuffer(const Mat& _VP, const Vec4& _SpriteUV)
			: VPMat(_VP), SpriteUV(_SpriteUV), AfterImageData(Vec4())
		{}
		~AfterImageBuffer() {}
	};


	struct LineBuffer
	{
		Mat				ViewProj;
		Vec4			SpriteUV;

		LineBuffer() :ViewProj(Mat()), SpriteUV(Vec4()) {}
		~LineBuffer() {}
	};*/




private:
	CBUFFER() {}
	virtual ~CBUFFER() = 0 {}
};




typedef struct _tagLINKVTX
{
	bool			bNaviVtx;
	UINT			VtxID;
	Vec3			VtxPos;
}LinkVtx;


struct TerrainData
{
	struct NaviLinkData
	{
		UINT					NaviMeshID;
		UINT					NaviTriIdx;
		IVec2					TerrainTriIdx;
		LinkVtx					LinkVtxData[3];
	};

	struct TerrainDataHeader
	{
		unsigned int							TerrainSizeX;
		unsigned int							TerrainSizeZ;
		float									TerrainMaxHeight;
		float									TerrainCellSize;
		unsigned int							LinkTriCount;
	};

	TerrainDataHeader				Header;
	std::vector<struct	GEOTERRAINVTX>		TerrainDataVec;
	std::list<NaviLinkData>			NaviLinkTriList;

};