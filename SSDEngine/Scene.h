	#pragma once
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include "DXEnum.h"
#include "Resource.h"
#include "NameBase.h"
#include "ResourceMgr.h"
#include "Camera.h"
#include "Actor.h"
#include "Collider.h"
#include "Renderer.h"
#include "Script.h"
#include "Light.h"
#include "DXStruct.h"

#define UICLEARCOLOR Vec4(1.f,0.f,1.f,0.f)

class CSceneScript : public CRefBase, public CTypeBase
{
	friend class CScene;
protected:
	bool											m_bLoading;
	CScene *										m_pPairScene;

public:
	virtual const bool Loading() = 0;
	virtual const bool AsyncLoading(void* _Arg) { return true; }

public:
	void SetCameraPos(const Vec3& _Pos, const size_t& _CamIndex = 0);

protected:
	CActor * CreateActor(const ACTOR_UPDATE_LAYER& _Layer = UPDATE_LAYER_NORMAL, const  std::wstring& _Name = L"UnnamedActor");
	CActor * CreateActor(const std::wstring& _Name, const ACTOR_UPDATE_LAYER& _Layer = UPDATE_LAYER_NORMAL);
	CActor * CreateActorNoneTransform(const ACTOR_UPDATE_LAYER& _Layer = UPDATE_LAYER_NORMAL, const  std::wstring& _Name = L"UnnamedActor");
	CActor * CreateActorNoneTransform(const std::wstring& _Name, const ACTOR_UPDATE_LAYER& _Layer = UPDATE_LAYER_NORMAL);

protected:
	void LinkCollsionChannel(const SCENECOLLISION& _Type  ,const std::wstring& _Src, const std::wstring& _Dest);
	void DisConnectChannel(const SCENECOLLISION& _Type,const std::wstring& _Src, const std::wstring& _Dest);


public:
	virtual void PreUpdate(const float& _DeltaTime) {}
	virtual void PostUpdate(const float& _DeltaTime) {}
	virtual void LastUpdate(const float& _DeltaTime) {}

	// 씬이 전환되기 전 딱 한번 호출하는 함수
	virtual void WakeUp() {};
	virtual void ShutDown() {};
	virtual void SceneRender() {};
	virtual void DebugRender() {};

public:
	SPTR<CCamera> GetMainCamera();
	SPTR<CCamera> GetUICamera();
	SPTR<CCamera> GetSceneCamera(const size_t& _CamIndex);
	CActor*	GetSceneCameraActor(const size_t& _CamIndex);

protected:
	// 자신과 연결된 씬의 모든 액터,컴포넌트들을 제거
	void ClearScene();
	// ClearScene() + 씬 초기화(기본 카메라 생성)
	void InitializeScene();

protected:
	CSceneScript(CScene* _Pair) : m_pPairScene(_Pair) , m_bLoading(false){}
	virtual ~CSceneScript() = 0 {}
};

class CCollisionChannel : public CReferenceBase
{
private:
	friend class CSceneCollisionMgr;
private:
	std::wstring										m_ChannelKey;
	std::list<SPTR<CCollider>>							m_listCollider;

	std::list<CCollisionChannel*>						m_listCollisionChannel;


private:
	void Collision();
	void DebugRender();
	void LastUpdate();
	void Release();

private:
	void Clear();

private:
	void OtherCollision(CCollisionChannel* _OtherChannel);
	void ThisCollision();

private:
	CCollisionChannel(const std::wstring& _Src);
	~CCollisionChannel();
};


class CSceneCollisionMgr
{
	friend class CScene;
	friend CCollider;

public:
	CScene*																			m_pPairScene;
	std::unordered_map<std::wstring, SPTR<CCollisionChannel>>						m_mapColliderChannel;

public:
	const bool PushCollider(CCollider* _Col);
	void LinkCollsionChannel(const std::wstring& _Src, const std::wstring& _Dest);
	void DisConnectChannel(const std::wstring& _Src, const std::wstring& _Dest);

private:
	void Collision();
	void DebugRender();
	void LastUpdate();
	void Release();

private:
	void Clear();


	CSceneCollisionMgr(CScene* _Scene)
		:m_pPairScene(_Scene)
	{}
	~CSceneCollisionMgr()
	{
		m_mapColliderChannel.clear();
	}
};

struct InstValue
{
	class CFbxRenderer*					InstRenderer;
	int									InstMeshIndex;
};

//struct RenList 
//{
//	std::list<SPTR<CRenderer>>						m_RenderingList;
//	std::list<SPTR<CRenderer>>						m_RenderingNoneList;
//
//public:
//
//};

class CSceneRenderMgr  
{
private:
	static SPTR<class MultiRenderTarget>								pDefferredMRT;
	static SPTR<class MultiRenderTarget>								pDecalMRT;
	static SPTR<MultiRenderTarget>										pDeferredLightMRT;
	static SPTR<class CMesh>											pDefferredMesh;
	static SPTR<class CMaterial>										pDefferredMaterial;
	static SPTR<CMaterial>												pCamMergeMaterial;
	static SPTR<CMaterial>												pUIMergeMaterial;

public:
	static bool															IsDeferred;
	static CBUFFER::ForwardLightBuffer									FLightBuffer;

private:
	static void Init();

private:
	friend class CScene;
	friend class Engine;
	friend class SSDCore;
	friend CCamera;
	friend CRenderer;
	friend CLight;

	CScene*																m_pPairScene;
	
	
	// 카메라의 레이어(오더)에 따라서 한 오브젝트가 랜더링이 여러번 될 수 있다.
	// 현재 저장씬에 존재하는 모든 카메라
	std::multimap<CAMERA_GROUP, SPTR<CCamera>>							m_mapCamera;
	std::multimap<CAMERA_GROUP, SPTR<CCamera>>::iterator				m_CamStartIter;
	std::multimap<CAMERA_GROUP, SPTR<CCamera>>::iterator				m_CamEndIter;


	std::list<SPTR<CCamera>>											m_CandidateMainCamera;
	std::list<SPTR<CCamera>>::iterator									m_MainCamIter;
	std::list<SPTR<CCamera>>::iterator									m_MainCamEnd;
	SPTR<CCamera>														m_UICamera;

	std::map<RENDER_LAYER, std::list<SPTR<CRenderer>>>					m_mapRendererList;
	std::list<SPTR<CRenderer>>											m_DebugList;
	std::map<RENDER_LAYER, std::list<SPTR<CRenderer>>>::iterator		m_FindRenListIter;

	std::list<SPTR<CLight>>												m_listLight;

	//인스턴싱
	std::map<CFbx*, std::vector<InstValue>>								m_mapFbxRenderGroup;
	std::list<SPTR<CRenderer>>											m_listSingle;
	std::list<SPTR<CRenderer>>											m_listDecal;

public:
	const bool PushCamera(CCamera* _Camera);
	void RegisteredMainCamera(SPTR<CCamera> _pCamera);
	void PopBackCandidateMainCam();

	SPTR<CCamera> ChangeMainCamera();
	SPTR<CCamera> ChangeMainCamera(SPTR<CCamera> _pCamera);
	void ClearMainCameraList();
	const bool PushRenderer(CRenderer* _Ren);
	const bool PushLight(CLight* _Light);
	
public:
	SPTR<CCamera> GetMainViewCamera();

private:
	void Render();
	void Release();

private:
	void CameraRelease();
	void RendererRelease();

	void RenderCameraLoop(SPTR<CCamera> _CamIter);

private:
	void Clear();

private:
	void DefferrdRender(SPTR<CCamera> _Camera, std::list<SPTR<CRenderer>>& _List);
	void ForwardRender(SPTR<CCamera> _Camera, std::list<SPTR<CRenderer>>& _List);
	void DefferrdLightRender(SPTR<CCamera> _Camera);
	void MergeCameraTarget(SPTR<CCamera> _Camera);
	void MergeUITarget(SPTR<CCamera> _CamTarget, SPTR<CCamera> _UITarget);

	void ForwardLightRender();

	CSceneRenderMgr(CScene* _Scene)
		:m_pPairScene(_Scene), m_UICamera(nullptr)
	{

	}
	~CSceneRenderMgr()
	{
	}
};


class CScene final : public CResource
{
	friend class Engine;
	friend CResourceMgr<CScene>;
	friend CSceneScript;
	friend class CGameWindow;
	friend class ASceneCamera;
	friend CCollider;
	friend CScript;
	friend SSDCore;

private:
	static CScene*										pCurScene;
	static CScene*										pNextScene;
	static bool											bFixFrame;
	static float										fFixDeltaTime;
	static float										fAccSceneDeltaTime;
public:
	static void SceneRelease();

public:
	template<typename Script >
	static CScene* CreateScene(const std::wstring& _SceneName , const bool& _Loading = false)
	{
		CScene* NewScene = CResourceMgr<CScene>::Create(_SceneName, { _SceneName });

		if (nullptr == NewScene)
		{
			TASSERT(true);
			return nullptr;
		}
		
		NewScene->Init();
		Script* InitScript = new Script(NewScene);
		// 만약 템플릿을 잘못 넣어줘도 SPTR형에서 자식 형이 아닐경우 컴파일러에서 
		// 에러를 출력해준다.
		
		NewScene->SetSceneScript(InitScript);

		if (true == _Loading)
			NewScene->Loading();

		return NewScene;
	}


	template<typename Script >
	static CScene* CreateNoResourceScene(const std::wstring& _SceneName, const bool& _Loading = false)
	{
		CScene* NewScene = CResourceMgr<CScene>::Create(_SceneName, { _SceneName });

		if (nullptr == NewScene)
		{
			TASSERT(true);
			return nullptr;
		}

		NewScene->Init();
		Script* InitScript = new Script(NewScene);

		// 만약 템플릿을 잘못 넣어줘도 SPTR형에서 자식 형이 아닐경우 컴파일러에서 
		// 에러를 출력해준다.

		NewScene->SetSceneScript(InitScript);

		if (true == _Loading)
			NewScene->Loading();

		return NewScene;
	}

	static SPTR<CScene> FindScene(const std::wstring& _SceneName);
	static const bool ChangeScene(const std::wstring& _SceneName);
	static const bool ChangeScene(const std::wstring& _SceneName,const float _FixFps);

private:
	static void SceneProgress(const float& _DeltaTime);


private:
	static CActor* CreateActorNoneTransform(CScene* _pScene, const ACTOR_UPDATE_LAYER& _Layer, const std::wstring& _Name);
public:
	static CActor* CreateActor(CScene* _pScene,const ACTOR_UPDATE_LAYER& _Layer,const std::wstring& _Name);


private:
	bool													m_bLoadingComplete;
	std::list<SPTR<CSceneScript>>							m_listSceneScript;
	CSceneCollisionMgr										m_PairCollision2DMgr;
	CSceneCollisionMgr										m_PairCollision3DMgr;
	CSceneRenderMgr											m_PairRenderMgr;
	
	std::map<int, std::list<SPTR<CActor>>>					m_mapActorList;
	std::list<SPTR<CScript>>								m_listActorScript;
	std::vector<SPTR<CCamera>>								m_vecSceneCamera;
	

public:
	const bool IsLoading() { return m_bLoadingComplete; }

private:
	bool SetSceneScript(CSceneScript* _Script)
	{
		if (nullptr == _Script)
			return false;

		_Script->InitTypeInfo(_Script);
		m_listSceneScript.push_back(_Script);
		return true;
	}

public:
	bool AddSceneScript(CSceneScript* _Script, const bool& _bLoading = true) 
	{
		if (nullptr == _Script)
			return false;

		_Script->InitTypeInfo(_Script);

		if (true == _bLoading) 
		{
			_Script->m_bLoading = true;
			_Script->Loading();
		}
		else
			m_bLoadingComplete = false;

		m_listSceneScript.push_back(_Script);
		return true;
	}

	template<typename Script>
	SPTR<Script> FindSceneScript()
	{
		std::list<SPTR<CSceneScript>>::iterator Start = m_listSceneScript.begin();
		std::list<SPTR<CSceneScript>>::iterator End = m_listSceneScript.end();

		for (;Start != End;++Start)
		{
			if (true == (*Start)->EqualType<Script>())
				return *Start;
		}

		return nullptr;
	}

public:
	void Init();
	const bool Loading();
	void WakeUp();
	void ShutDown();

public:
	void Progress(const float& _DeltaTime);
	void Update(const float& _DeltaTime);
	void Collision();
	void Collision3D();
	void DebugRender();
	void Render();
	void LastUpdate(const float& _DeltaTime);
	void Release();
	void ReduceRelease();

private:
	const bool PushCollider(const SCENECOLLISION& _Type ,CCollider* _Collider);
	const bool PushScript(CScript* _Script);
	const bool PushActor(CActor* _Actor,const ACTOR_UPDATE_LAYER& _Layer);
	const bool PushRenderer(CRenderer* _Renderer);
	const bool PushCamera(CCamera* _Camera);

public:
	void RegisteredMainCamera(SPTR<CCamera> _pCamera);
	void PopBackMainCamera();

	SPTR<CCamera> ChangeMainCamera();
	SPTR<CCamera> ChangeMainCamera(SPTR<CCamera> _pCamera);
protected:
	void ClearScene();


public:
	SPTR<CCamera> GetSceneCamera(const size_t& _CamIndex) 
	{
		if (m_vecSceneCamera.size() <= _CamIndex)
			return nullptr;

		return m_vecSceneCamera[_CamIndex];
	}
	CActor* GetSceneCameraActor(const size_t& _CamIndex)
	{
		if (m_vecSceneCamera.size() <= _CamIndex)
			return nullptr;

		return m_vecSceneCamera[_CamIndex]->GetCamActor();
	}
	SPTR<CCamera> GetSceneCamera() 
	{
		if (true == m_vecSceneCamera.empty())
			return nullptr;

		return m_vecSceneCamera[0];
	}
	SPTR<CCamera> GetUICamera() 
	{
		if (m_vecSceneCamera.size() <= 1)
			return nullptr;

		return m_vecSceneCamera[1];
	}

	SPTR<CCamera> GetMainViewCamera() 
	{
		return m_PairRenderMgr.GetMainViewCamera();
	}

public:
	CSceneRenderMgr * GetRenderMgr() { return &m_PairRenderMgr; }
	CSceneCollisionMgr* GetCollision2DMgr() { return &m_PairCollision2DMgr; }
	CSceneCollisionMgr* GetCollision3DMgr() { return &m_PairCollision3DMgr; }

private:
	CScene();
	~CScene();

public:
	NONE_LOAD_RESOURCE(CScene)
	CREATE_DESC_BODY(std::wstring Name;)

private:
	const bool Create(const CREATE_DESC& _Desc);

public:
	std::map<int, std::list<SPTR<CActor>>>& GetAllActor()
	{
		return m_mapActorList;
	}
};

