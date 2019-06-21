#pragma once
#include <Scene.h>
#include <NameBase.h>
#include <string>
#include <ReferenceBase.h>
#include <unordered_set>
#include <DXStruct.h>
#include <MagicCircle.h>

class SSDToolGlobal
{
public:
	static CScene*									ToolGlobalScene;
	static class CToolMainScene*					ToolMainScene;
	static CActor*									ToolSelectActor;
	static CRenderer*								ToolSelectRenderer;
	static CCollider*								ToolSelectCollider;
	static CLight*									ToolSelectLight;
	static class CBloomFilter*						ToolSelectBloom;
	static class CFogFilter*						ToolSelectFog;
	static class CGaussianBlur*						ToolSelectGaussian;
	static class CSkeletalFbxRenderer*				ToolSkelRender;

	static std::wstring								ToolSelectMesh;
	static std::wstring								ToolSelectMat;

	static class CFbx*								ToolSelectFbx;
	static class CBoneMesh*							ToolBoneMesh;
	static class CFBXMesh*							ToolFbxMesh;
	static int										ToolSelectMeshIndex;
	static int										ToolSelectSubset;
	static class CGizmo*							ToolMainGizmo;

	static class CNavigationObject*					ToolSelectNaviObj;
	static class CNavigationMesh*					ToolSelectNaviMesh;
	static class CNaviMeshArea*						ToolSelectNaviArea;


	static class EditDlg*							ToolEditDlg;
	static class ObjectDlg*							ToolObjectDlg;
	static class SceneListDlg*						ToolSceneList;
	static class EditView*							ToolEditView;
	static class LightDlg*							ToolLightDlg;
	static class ControlDlg*						ToolControlDlg;
	static class MaterialDlg*						ToolMatDlg;
	static class FilterDlg*							ToolFilterDlg;
	static class NaviDlg*							ToolNaviDlg;
	static class FogDlg*							ToolFogDlg;

	static bool										CollEdit;

	static std::unordered_set<CNavigationObject*>	setToolNaviObj;

public:
	static void Release();
	static void ResetDynamicDlg();

public:
	static const float InttoFloat(int _int);
	static const int FloattoInt(float _float);

	//static const CString AllowOnlyRealNum(CString _Text, bool minus);

	static void SetSelectActor(class CActor* _Actor);
	static const bool SetSelectNavigationCom(CActor* _Actor);

public:
	static void EraseNaviObj(CNavigationObject* _pNaviObj);

private:
	SSDToolGlobal() {}
	virtual ~SSDToolGlobal() = 0 {}
};

//enum FBX_TYPE
//{
//	FBXTYPE_BASE,
//	FBXTYPE_FBX,
//	FBXTYPE_SKEL,
//	FBXTYPE_MAX
//};
//
//enum COL_TYPE
//{
//	COLTYPE_OBB,
//	COLTYPE_SPHERE,
//	COLTYPE_MAX
//};
//
////prefab이름
////엑터 트렌스폼
////fbx렌더러 개수
////fbx렌더러 매쉬 이름, 머테리얼 이름, 매쉬 인덱스, 렌더러 트렌스폼
////스피어 콜리전 개수
////스피어 콜리전 트렌스폼, 콜리전 그룹 이름
////obb콜리전 개수
////obb콜리전 트렌스폼, 콜리전 그룹 이름
//
//class PrefabData : public CReferenceBase
//{
//public:
//	class FbxSaveData
//	{
//	public:
//		std::wstring							MeshName;
//		std::map<int, std::wstring>				MatInfoMap;
//
//		Vec3									RenderSize;
//		Vec3									RenderPos;
//		bool									AllCheck;
//	};
//
//	class ColSaveData
//	{
//	public:
//		std::wstring							ColGroupName;
//		COL_TYPE								ColType;
//
//		Vec3									ColSize;
//		Vec3									ColPos;
//
//	public:
//		ColSaveData(COL_TYPE _Type) : ColType(_Type) {}
//		ColSaveData() {}
//		~ColSaveData() {}
//	};
//
//	class NavigationData
//	{
//	public:
//		int										RenderLayer;
//		UINT									NaviSerialNumber;
//		std::wstring							NaviObjName;
//
//	public:
//		NavigationData()
//			: NaviObjName(L""), NaviSerialNumber(-1), RenderLayer(-1)
//		{}
//		~NavigationData() {}
//	};
//
//public:
//	Vec3										ActorSize;
//	Vec3										ActorPos;
//	Vec3										ActorRot;
//
//	std::vector<FbxSaveData>					vecFbxData;
//	std::vector<ColSaveData>					vecColData;
//	std::vector<NavigationData>					vecNaviData;
//
//public:
//	PrefabData() : ActorSize(Vec3::Zero3), ActorPos(Vec3::Zero3), ActorRot(Vec3::Zero3) {}
//	PrefabData(Vec3 _Size, Vec3 _Pos, Vec3 _Rot) : ActorSize(_Size), ActorPos(_Pos), ActorRot(_Rot) {}
//	~PrefabData() { }
//	PrefabData& operator= (const PrefabData& _Other)
//	{
//		ActorSize = _Other.ActorSize;
//		ActorPos = _Other.ActorPos;
//		ActorRot = _Other.ActorRot;
//		memcpy_s(&vecFbxData, sizeof(vecFbxData), &_Other.vecFbxData, sizeof(_Other.vecFbxData));
//		memcpy_s(&vecColData, sizeof(vecColData), &_Other.vecColData, sizeof(_Other.vecColData));
//		return (*this);
//	}
//};
//
//class FbxData : public CReferenceBase
//{
//public:
//	std::wstring								MeshName;
//	std::wstring								MatName;
//	FBX_TYPE									Type;
//	class CFbx*									FbxPtr;
//
//public:
//	FbxData(FBX_TYPE _Type = FBX_TYPE::FBXTYPE_FBX, std::wstring _MeshName = L"", std::wstring _MatName = L"")
//		: MeshName(_MeshName), MatName(_MatName), Type(_Type), FbxPtr(nullptr)
//	{}
//	~FbxData() {}
//};
//
//class FilterSaveData : public CReferenceBase
//{
//public:
//	class GaussianSaveData
//	{
//	public:
//		CBUFFER::GaussianBuffer						GaussianBuffer;
//		float										Sigma;
//
//	public:
//		GaussianSaveData() : Sigma(0.f)
//		{}
//		~GaussianSaveData() {}
//	};
//
//	class BloomSaveData
//	{
//	public:
//		MINIMIZE_BUFFER_TYPE						BufferType;
//		CBUFFER::MinimizeBuffer						MiniBuffer;
//		CBUFFER::BloomBuffer						BloomBuffer;
//
//		float										Sigma;
//		float										BloomBufferSize;
//		UINT										BloomResNum;
//
//	public:
//		BloomSaveData() {}
//		~BloomSaveData() {}
//	};
//
//public:
//	std::list<GaussianSaveData>						listGauData;
//	std::list<BloomSaveData>						listBloomData;
//
//public:
//	FilterSaveData() {}
//	~FilterSaveData() {}
//};
//
//class ActorSaveData : public PrefabData
//{
//public:
//	class LightSaveData
//	{
//	public:
//		RENDER_LAYER							Layer;
//		CBUFFER::LightBuffer					LightBuffer;
//
//		bool									VolumeCheck;
//		std::wstring							MeshName;
//		Vec3									LightSubSize;
//
//	public:
//		LightSaveData() : Layer(RENDER_LAYER_PLAYER), VolumeCheck(0), MeshName(L""), LightSubSize(Vec3::Zero3)
//		{}
//		~LightSaveData() {}
//		LightSaveData& operator= (const LightSaveData& _Other)
//		{
//			Layer = _Other.Layer;
//			LightBuffer = _Other.LightBuffer;
//			VolumeCheck = _Other.VolumeCheck;
//			MeshName = _Other.MeshName;
//			LightSubSize = _Other.LightSubSize;
//			return (*this);
//		}
//	};
//
//public:
//	int											UpdateLayer;
//	bool										IsLight;
//	LightSaveData								LightData;
//
//public:
//	ActorSaveData(Vec3 _Size, Vec3 _Pos, Vec3 _Rot) : PrefabData(_Size, _Pos, _Rot), IsLight(false)
//	{}
//	~ActorSaveData() {}
//};
//
//
//
//struct SceneFileHeader 
//{
//	int					FileVersion;
//	int					ActorCount;
//	// 추후 씬파일 구조가 변경되었을때
//	// 필요에따라 최대 56byte 만큼의 정보를 더 담을 수 있다. 
//	int					Temp[14];
//};
//
//enum SCENEFILEVISION 
//{
//	TERRAIN_NAVIGATION_SCENE_FILE = 1,
//	TERRAIN_MAGIC_FILE ,
//	MAX_SCENEFILEVISION,
//};
