#pragma once
#include <map>
#include <SSDCore.h>
#include <DXStruct.h>
#include "MagicCircle.h"
#include "MiddleStruct.h"

#define LOGICPORT 54321

#define STATEX -530.f
#define STATEY -350.f

enum TREETYPE
{
	BUSH1,
	BUSH2,
	BUSH3,
	BUSH4,
	BUSH5,
	BUSH6,
	TREE1,
	TREE2,
	TREETYPEMAX,
};


enum FBX_TYPE
{
	FBXTYPE_BASE,
	FBXTYPE_FBX,
	FBXTYPE_SKEL,
	FBXTYPE_MAX
};

enum COL_TYPE
{
	COLTYPE_OBB,
	COLTYPE_SPHERE,
	COLTYPE_MAX
};


enum SCENEFILEVISION
{
	TERRAIN_NAVIGATION_SCENE_FILE = 1,
	TERRAIN_MAGIC_FILE,
	TERRAIN_MAGIC_DEFERRED_FILE,
	//TERRAIN_MAGIC_LAYER_FILE,
	MAX_SCENEFILEVISION,
	LAST_SCENE_VERSION = MAX_SCENEFILEVISION - 1,
};

class PrefabData : public CReferenceBase
{
public:
	class FbxSaveData
	{
	public:
		std::wstring							MeshName;
		std::map<int, std::wstring>				MatInfoMap;

		Vec3									RenderSize;
		Vec3									RenderPos;
		bool									AllCheck;
		bool									IsDeferred;
		RENDER_LAYER							RenderLayer;

	public:
		FbxSaveData() : IsDeferred(true), RenderLayer(RENDER_LAYER::RENDER_LAYER_PLAYER)
		{}
		~FbxSaveData()
		{}
	};

	class ColSaveData
	{
	public:
		std::wstring							ColGroupName;
		COL_TYPE								ColType;

		Vec3									ColSize;
		Vec3									ColPos;

	public:
		ColSaveData(COL_TYPE _Type) : ColType(_Type) {}
		ColSaveData() {}
		~ColSaveData() {}
	};

	class NavigationData
	{
	public:
		int										RenderLayer;
		UINT									NaviSerialNumber;
		std::wstring							NaviObjName;

	public:
		NavigationData()
			: NaviObjName(L""), NaviSerialNumber(-1), RenderLayer(-1)
		{}
		~NavigationData() {}
	};

public:
	Vec3										ActorSize;
	Vec3										ActorPos;
	Vec3										ActorRot;

	std::vector<FbxSaveData>					vecFbxData;
	std::vector<ColSaveData>					vecColData;
	std::vector<NavigationData>					vecNaviData;

public:
	PrefabData() : ActorSize(Vec3::Zero3), ActorPos(Vec3::Zero3), ActorRot(Vec3::Zero3) {}
	PrefabData(Vec3 _Size, Vec3 _Pos, Vec3 _Rot) : ActorSize(_Size), ActorPos(_Pos), ActorRot(_Rot) {}
	~PrefabData() { }
	PrefabData& operator= (const PrefabData& _Other)
	{
		ActorSize = _Other.ActorSize;
		ActorPos = _Other.ActorPos;
		ActorRot = _Other.ActorRot;
		memcpy_s(&vecFbxData, sizeof(vecFbxData), &_Other.vecFbxData, sizeof(_Other.vecFbxData));
		memcpy_s(&vecColData, sizeof(vecColData), &_Other.vecColData, sizeof(_Other.vecColData));
		return (*this);
	}
};

class FbxData : public CReferenceBase
{
public:
	std::wstring								MeshName;
	std::wstring								MatName;
	FBX_TYPE									Type;
	class CFbx*									FbxPtr;

public:
	FbxData(FBX_TYPE _Type = FBX_TYPE::FBXTYPE_FBX, std::wstring _MeshName = L"", std::wstring _MatName = L"")
		: MeshName(_MeshName), MatName(_MatName), Type(_Type), FbxPtr(nullptr)
	{}
	~FbxData() {}
};

class FilterSaveData : public CReferenceBase
{
public:
	class GaussianSaveData
	{
	public:
		CBUFFER::GaussianBuffer						GaussianBuffer;
		float										Sigma;

	public:
		GaussianSaveData() : Sigma(0.f)
		{}
		~GaussianSaveData() {}
	};

	class BloomSaveData
	{
	public:
		MINIMIZE_BUFFER_TYPE						BufferType;
		CBUFFER::MinimizeBuffer						MiniBuffer;
		CBUFFER::BloomBuffer						BloomBuffer;

		float										Sigma;
		float										BloomBufferSize;
		UINT										BloomResNum;

	public:
		BloomSaveData() {}
		~BloomSaveData() {}
	};

public:
	std::list<GaussianSaveData>						listGauData;
	std::list<BloomSaveData>						listBloomData;

public:
	FilterSaveData() {}
	~FilterSaveData() {}
};

class ActorSaveData : public PrefabData
{
public:
	class LightSaveData
	{
	public:
		RENDER_LAYER							Layer;
		CBUFFER::LightBuffer					LightBuffer;

		bool									VolumeCheck;
		std::wstring							MeshName;
		Vec3									LightSubSize;

	public:
		LightSaveData() : Layer(RENDER_LAYER_PLAYER), VolumeCheck(0), MeshName(L""), LightSubSize(Vec3::Zero3)
		{}
		~LightSaveData() {}
		LightSaveData& operator= (const LightSaveData& _Other)
		{
			Layer = _Other.Layer;
			LightBuffer = _Other.LightBuffer;
			VolumeCheck = _Other.VolumeCheck;
			MeshName = _Other.MeshName;
			LightSubSize = _Other.LightSubSize;
			return (*this);
		}
	};

public:
	int											UpdateLayer;
	bool										IsLight;
	LightSaveData								LightData;

	bool										IsMagicCircle;
	MAGICCIRCLE_COLOR							MagicCircleColor;
	Vec3										MagicCircleScale;
	Vec3										MagicCirclePivot;

public:
	ActorSaveData(Vec3 _Size, Vec3 _Pos, Vec3 _Rot) : PrefabData(_Size, _Pos, _Rot), IsLight(false), IsMagicCircle(false)
	{}
	~ActorSaveData() {}
};

struct SceneFileHeader
{
	int					FileVersion;
	int					ActorCount;
	// 추후 씬파일 구조가 변경되었을때
	// 필요에따라 최대 56byte 만큼의 정보를 더 담을 수 있다. 
	int					Temp[14];
};

typedef class DeadByDaylightGlobal
{
public:
	static class CDXFont*										pPrettyFont;
	static class CTerrain*										pMainTerrain;
	static class CParticleRenderer*								pMainParticle;
	//static class CDBDRule*									pGameRule;

	static std::vector<SPTR<ActorSaveData>>						m_vecActorData;
	static std::vector<SPTR<ActorSaveData>>						m_vecActorLoadData;
	static std::vector<SPTR<FilterSaveData>>					m_vecFilterData;

	static std::list<SPTR<class CFbxRenderer>>					m_FbxList;
	static std::list<SPTR<class COBBCollider>>					m_ObbList;
	static std::list<SPTR<class CSphereCollider>>				m_SphereList;
	static std::list<SPTR<class CNavigationObject>>				m_NaviObjList;

	static std::map<int, std::list<SPTR<CActor>>>				m_mapAllActor;
	static std::map<int, std::list<SPTR<CActor>>>				m_mapAllLoadActor;

	static TerrainData											m_TerrainLoadData;
	static RoomList												m_RecvRoomList;
	static LobbyEnterData										m_RecvRoomData;
	static WorldData											m_RecvWorldData;

	static bool													bGameWin;

public:
	static void Init();
	static void Release();

private:
	static void PathSetting();
	static void KeySetting();

public:
	static void SaveScene(const wchar_t* _Path, CScene* _WriteScene);
	static void WriteScene(const wchar_t* _Path, CScene* _WriteScene);
	
	static void ReadScene(const wchar_t* _Path,CScene* _ReadScene,const bool& _bCircle = false);
	static void ReadScene(const wchar_t* _Path);
	static void LoadScene(CScene* _LoadScene, const SCENEFILEVISION& _FileVersion,const bool& _bCircle);

private:
	static void ReadScene_TerrainNavi(class CReadStream& _ReadData,const SceneFileHeader& _Header);
	static void ReadScene_TerrainMagic(CReadStream& _ReadData, const SceneFileHeader& _Header);
	static void ReadScene_TerrainMagicDeferrd(CReadStream& _ReadData, const SceneFileHeader& _Header);

public:
	static Vec2 NumbertoUIRaio(Vec2 _Pos);


public:
	static void RecordRoomListData(const RoomList& _RecvData);
	static void RecordRecvRoomData(const LobbyEnterData& _RecvData);

public:
	DeadByDaylightGlobal() {}
	virtual ~DeadByDaylightGlobal() = 0;
}GlobalDBD, DBD;


// 씬 로드에 필요한 구조체들
