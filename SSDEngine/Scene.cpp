
#ifndef _WINSOCK2API_
#include <WinSock2.h>
#endif 

#include "Scene.h"
#include "ResourceMgr.h"
#include "Device.h"
#include "GlobalDebug.h"
#include "TransForm.h"
#include "MultiRenderTarget.h"
#include "DepthStencilState.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"
#include "Profiler.h"
#include "KeyMgr.h"
#include "TerrainRenderer.h"
#include "FbxRenderer.h"
#include "InstancingBuffer.h"
#include "SSDThread.h"
#include "DecalRenderer.h"
CScene*											CScene::pCurScene = nullptr;
CScene*											CScene::pNextScene = nullptr;
bool											CScene::bFixFrame = false;
float											CScene::fFixDeltaTime = 0.f;
float											CScene::fAccSceneDeltaTime = 0.f;
//////////////////////////////////////		Scene Script Load Part		///////////////////////////////////////////
void CSceneScript::SetCameraPos(const Vec3& _Pos, const size_t& _CamIndex /*= 0*/)
{
	if (m_pPairScene->m_vecSceneCamera.size() <= _CamIndex)
		return;

	m_pPairScene->m_vecSceneCamera[_CamIndex]->SetPos(_Pos);
}
CActor * CSceneScript::CreateActor(const ACTOR_UPDATE_LAYER& _Layer/* = UPDATE_LAYER_NORMAL*/, const  std::wstring& _Name /*= L"UnnamedActor"*/)
{
	return CScene::CreateActor(m_pPairScene, _Layer, _Name);
}
CActor * CSceneScript::CreateActor(const std::wstring& _Name, const ACTOR_UPDATE_LAYER& _Layer/* = UPDATE_LAYER_NORMAL*/)
{
	return CScene::CreateActor(m_pPairScene, _Layer, _Name);
}
CActor * CSceneScript::CreateActorNoneTransform(const ACTOR_UPDATE_LAYER& _Layer /*= UPDATE_LAYER_NORMAL*/, const  std::wstring& _Name/* = L"UnnamedActor"*/)
{
	return CScene::CreateActorNoneTransform(m_pPairScene, _Layer, _Name);
}
CActor * CSceneScript::CreateActorNoneTransform(const std::wstring& _Name, const ACTOR_UPDATE_LAYER& _Layer /*= UPDATE_LAYER_NORMAL*/)
{
	return CScene::CreateActorNoneTransform(m_pPairScene, _Layer, _Name);
}

void CSceneScript::LinkCollsionChannel(const SCENECOLLISION& _Type, const std::wstring& _Src, const std::wstring& _Dest)
{
	switch (_Type)
	{
	case COLLISION_2D:
		return m_pPairScene->GetCollision2DMgr()->LinkCollsionChannel(_Src, _Dest);
	case COLLISION_3D:
		return m_pPairScene->GetCollision3DMgr()->LinkCollsionChannel(_Src, _Dest);
	default:
		return;
	}
}
void CSceneScript::DisConnectChannel(const SCENECOLLISION& _Type, const std::wstring& _Src, const std::wstring& _Dest)
{
	switch (_Type)
	{
	case COLLISION_2D:
		return m_pPairScene->GetCollision2DMgr()->DisConnectChannel(_Src, _Dest);
	case COLLISION_3D:
		return m_pPairScene->GetCollision3DMgr()->DisConnectChannel(_Src, _Dest);
	default:
		return;
	}
}
SPTR<CCamera> CSceneScript::GetMainCamera()
{
	return m_pPairScene->GetSceneCamera();
}
SPTR<CCamera> CSceneScript::GetUICamera()
{
	return m_pPairScene->GetUICamera();
}
SPTR<CCamera>	CSceneScript::GetSceneCamera(const size_t& _CamIndex)
{
	return m_pPairScene->GetSceneCamera(_CamIndex);
}
CActor*	CSceneScript::GetSceneCameraActor(const size_t& _CamIndex)
{
	return m_pPairScene->GetSceneCameraActor(_CamIndex);
}


void CSceneScript::ClearScene()
{
	if (nullptr != m_pPairScene)
	{
		m_pPairScene->ClearScene();
	}
}

void CSceneScript::InitializeScene()
{
	if (nullptr != m_pPairScene)
	{
		m_pPairScene->ClearScene();
		m_pPairScene->Init();
	}
}
///////////////////////////////////////			Collision Channel				////////////////////////////////////////////////////////
CCollisionChannel::CCollisionChannel(const std::wstring& _Src)
	:m_ChannelKey(_Src)
{
	m_listCollider.clear();
	m_listCollisionChannel.clear();
}
CCollisionChannel::~CCollisionChannel()
{
	m_listCollider.clear();
	m_listCollisionChannel.clear();
}


void CCollisionChannel::Collision()
{
	if (true == m_listCollisionChannel.empty())
		return;

	for (CCollisionChannel* ColChannel : m_listCollisionChannel)
	{
		if (ColChannel != this)
			OtherCollision(ColChannel);
		else
			ThisCollision();
	}
}
void CCollisionChannel::DebugRender()
{
	if (true == m_listCollider.empty())
		return;

	for (SPTR<CCollider> Col : m_listCollider) 
	{
		if(true == Col->IsActive())
			Col->DebugRender();
	}
}
void CCollisionChannel::LastUpdate()
{
	if (true == m_listCollider.empty())
		return;

	for (SPTR<CCollider> Col : m_listCollider)
	{
		if(nullptr != Col)
			Col->DumpDeathCollider();
	}
}
void CCollisionChannel::Release()
{
	if (true == m_listCollider.empty())
		return;

	
	m_listCollider.erase(std::remove_if(m_listCollider.begin(), m_listCollider.end(), [](SPTR<CCollider> Collider)->bool 
	{
		return Collider->IsDeath();
	}), m_listCollider.end());
}
void CCollisionChannel::OtherCollision(CCollisionChannel* _OtherChannel)
{
	if (nullptr == _OtherChannel)
		return;

	for (SPTR<CCollider> MyCollider : m_listCollider)
	{
		if (false == MyCollider->IsActive()) 
		{
			MyCollider->CollisionEndEvent();
			continue;
		}

		for (SPTR<CCollider> OtherCollider : _OtherChannel->m_listCollider) 
		{
			if (true == MyCollider->IsIgnoreCollider(OtherCollider))
			{
				continue;
			}

			if (false == OtherCollider->IsActive()) 
			{
				OtherCollider->CollisionEndEvent();
				MyCollider->EraseSet(OtherCollider);
				continue;
			}

			MyCollider->Collision(OtherCollider);

			if (false == MyCollider->IsActive()) 
			{
				MyCollider->CollisionEndEvent();
				OtherCollider->CollisionEndEvent();
				MyCollider->ReleaseSet();	
			}

			if (false == OtherCollider->IsActive()) 
			{
				MyCollider->CollisionEndEvent();
				OtherCollider->CollisionEndEvent();
				MyCollider->EraseSet(OtherCollider);
			}
		}
		
	}
}
void CCollisionChannel::ThisCollision()
{
	std::list<SPTR<CCollider>>::iterator StartIter = m_listCollider.begin();
	std::list<SPTR<CCollider>>::iterator EndIter = m_listCollider.end();

	for (; StartIter != EndIter; ++StartIter )
	{
		if (false == (*StartIter)->IsActive())
			continue;

		std::list<SPTR<CCollider>>::iterator ColIter = StartIter;
		++ColIter;

		for (; ColIter != EndIter; ++ColIter) 
		{
			if (true == (*StartIter)->IsIgnoreCollider(*ColIter)) 
			{
				continue;
			}

			if (false == (*ColIter)->IsActive())
			{
				(*StartIter)->CollisionEndEvent();
				(*StartIter)->EraseSet((*ColIter));
				continue;
			}

			(*StartIter)->Collision(*ColIter);

			if (false == (*StartIter)->IsActive())
			{
				(*StartIter)->CollisionEndEvent();
				(*ColIter)->CollisionEndEvent();
				(*StartIter)->ReleaseSet();
			}

			if (false == (*ColIter)->IsActive())
			{
				(*StartIter)->CollisionEndEvent();
				(*ColIter)->CollisionEndEvent();
				(*StartIter)->EraseSet((*ColIter));
			}
		}
	}
}

void CCollisionChannel::Clear()
{
	std::list<SPTR<CCollider>>::iterator Start = m_listCollider.begin();
	std::list<SPTR<CCollider>>::iterator End = m_listCollider.end();
	
	for (;Start != End;++Start)
	{
		(*Start)->ClearSet();
	}

	m_listCollider.clear();
}

///////////////////////////////////////			SceneCollisionMgr				/////////////////////////////////////////////////////
const bool CSceneCollisionMgr::PushCollider(CCollider* _Col)
{
	if (nullptr == _Col)
		return false;

	std::wstring Channel = _Col->GetChannel();

	auto Find = m_mapColliderChannel.find(Channel);

	// 채널이 없다면 채널부터 생성한 뒤 충돌체를 넣는다
	if (Find == m_mapColliderChannel.end())
	{
		CCollisionChannel* NewChannel = new CCollisionChannel(Channel);

		auto InsertIter = m_mapColliderChannel.insert(std::unordered_map<std::wstring, SPTR<CCollisionChannel>>::value_type(Channel, NewChannel));

		if (true == InsertIter.second)
			InsertIter.first->second->m_listCollider.push_back(_Col);
		else
			return false;
	}
	else
		Find->second->m_listCollider.push_back(_Col);

	return true;
}


void CSceneCollisionMgr::LinkCollsionChannel(const std::wstring& _Src, const std::wstring& _Dest)
{
	SPTR<CCollisionChannel> Src = UMAPFINDSPTR<SPTR<CCollisionChannel>>(m_mapColliderChannel, _Src);
	SPTR<CCollisionChannel> Dest = UMAPFINDSPTR<SPTR<CCollisionChannel>>(m_mapColliderChannel, _Dest);
	// Src Channel이 없는경우 새로 생성
	if (Src == nullptr)
	{
		CCollisionChannel* NewChannel = new CCollisionChannel(_Src);
		m_mapColliderChannel.insert(std::unordered_map<std::wstring, SPTR<CCollisionChannel>>::value_type(_Src, NewChannel));
		Src = NewChannel;
	}

	if (Dest == nullptr)
	{
		CCollisionChannel* NewChannel = new CCollisionChannel(_Dest);
		m_mapColliderChannel.insert(std::unordered_map<std::wstring, SPTR<CCollisionChannel>>::value_type(_Dest, NewChannel));
		Dest = NewChannel;
	}

	for (CCollisionChannel* Channel : Src->m_listCollisionChannel)
	{
		// 만일 추가하려는 리스트에 _Dest가 들어있는경우 연결을 하지 않는다
		if (Channel == Dest)
			return;
	}

	for (CCollisionChannel* Channel : Dest->m_listCollisionChannel)
	{
		// 만일 상대에 채널에 내가 있는경우 연결을 하지 않는다.
		if (Channel == Src)
			return;
	}

	Src->m_listCollisionChannel.push_back(Dest);
}

void CSceneCollisionMgr::DisConnectChannel(const std::wstring& _Src, const std::wstring& _Dest)
{
	SPTR<CCollisionChannel> Src = UMAPFINDSPTR<SPTR<CCollisionChannel>>(m_mapColliderChannel, _Src);


	if (nullptr == Src)
		return;

	std::list<CCollisionChannel*>::iterator Start = Src->m_listCollisionChannel.begin();
	std::list<CCollisionChannel*>::iterator End = Src->m_listCollisionChannel.end();
	
	if (true == Src->m_listCollisionChannel.empty())
		return;


	for (;Start!= End; ++Start)
	{
		if ((*Start)->m_ChannelKey == _Dest) 
		{
			Src->m_listCollisionChannel.erase(Start);
			return;
		}
	}
}
void CSceneCollisionMgr::Collision()
{
	if (true == m_mapColliderChannel.empty())
		return;

	SPTR<CCollisionChannel> CurChannel;

	for (auto& ChannelIter : m_mapColliderChannel)
	{
		CurChannel = ChannelIter.second;
		CurChannel->Collision();
	}
}
void CSceneCollisionMgr::DebugRender()
{
	if (true == m_mapColliderChannel.empty())
		return;

	for (auto& ChannelIter : m_mapColliderChannel)
	{
		SPTR<CCollisionChannel> Channel = ChannelIter.second;
		if (nullptr != Channel)
			Channel->DebugRender();
	}
}
void CSceneCollisionMgr::LastUpdate()
{
	if (true == m_mapColliderChannel.empty())
		return;

	for (std::map<std::wstring,SPTR<CCollisionChannel>>::value_type& ChannelIter : m_mapColliderChannel)
	{
		SPTR<CCollisionChannel> Channel = ChannelIter.second;
		if (nullptr != Channel)
			Channel->LastUpdate();
	}
}
void CSceneCollisionMgr::Release()
{
	if (true == m_mapColliderChannel.empty())
		return;

	for (auto& ChannelIter : m_mapColliderChannel) 
	{
		SPTR<CCollisionChannel> Channel = ChannelIter.second;
		
		if(nullptr != Channel)
			Channel->Release();
	}
	
}

void CSceneCollisionMgr::Clear()
{
	std::unordered_map<std::wstring, SPTR<CCollisionChannel>>::iterator Start = m_mapColliderChannel.begin();
	std::unordered_map<std::wstring, SPTR<CCollisionChannel>>::iterator End = m_mapColliderChannel.end();

	for (;Start!=End ;++Start)
	{
		Start->second->Clear();
	}

}

////////////////////////////////////		Scene Render Mgr			//////////////////////////////////////////////////////
SPTR<MultiRenderTarget>					CSceneRenderMgr::pDefferredMRT = nullptr;
SPTR<MultiRenderTarget>					CSceneRenderMgr::pDecalMRT = nullptr;
SPTR<MultiRenderTarget>					CSceneRenderMgr::pDeferredLightMRT = nullptr;
SPTR<CMesh>								CSceneRenderMgr::pDefferredMesh = nullptr;
SPTR<CMaterial>							CSceneRenderMgr::pDefferredMaterial = nullptr;
SPTR<CMaterial>							CSceneRenderMgr::pCamMergeMaterial = nullptr;
SPTR<CMaterial>							CSceneRenderMgr::pUIMergeMaterial = nullptr;
bool									CSceneRenderMgr::IsDeferred = false;
CBUFFER::ForwardLightBuffer				CSceneRenderMgr::FLightBuffer;


void CSceneRenderMgr::Init()
{
	pDefferredMRT = CResourceMgr<MultiRenderTarget>::Find(L"Deferred");
	pDecalMRT = CResourceMgr<MultiRenderTarget>::Find(L"Decal");
	pDeferredLightMRT = CResourceMgr<MultiRenderTarget>::Find(L"Lighting");
	pDefferredMesh = CResourceMgr<CMesh>::Find(L"2DMesh");
	pDefferredMaterial = CResourceMgr<CMaterial>::Find(L"DeferredLight");
	pCamMergeMaterial = CResourceMgr<CMaterial>::Find(L"MergeCamera");
	pUIMergeMaterial = CResourceMgr<CMaterial>::Find(L"UIMerge");
}
const bool CSceneRenderMgr::PushCamera(CCamera* _Camera)
{
	if (nullptr == _Camera)
		return false;


	CAMERA_GROUP Group = _Camera->m_CameraGroup;

	std::multimap<CAMERA_GROUP, SPTR<CCamera>>::iterator FindStartIter = m_mapCamera.find(Group);
	std::multimap<CAMERA_GROUP, SPTR<CCamera>>::iterator FindEndIter;

	// 그룹이 중복되는 카메라가 없는경우
	if (FindStartIter == m_mapCamera.end())
	{
		m_mapCamera.insert(std::multimap<CAMERA_GROUP, SPTR<CCamera>>::value_type(Group, _Camera));
		return true;
	}

	FindStartIter = m_mapCamera.lower_bound(Group);
	FindEndIter = m_mapCamera.upper_bound(Group);

	// 넣은 카메라를 또 넣으려 할때 튕겨낸다.
	for (; FindStartIter != FindEndIter; ++FindStartIter)
	{
		if (_Camera == FindStartIter->second)
			return false;
	}

	m_mapCamera.insert(std::multimap<CAMERA_GROUP, SPTR<CCamera>>::value_type(Group, _Camera));
	return true;
}

void CSceneRenderMgr::RegisteredMainCamera(SPTR<CCamera> _pCamera)
{
	if (nullptr == _pCamera)
		return;


	for (SPTR<CCamera> Cam : m_CandidateMainCamera)
	{
		if (_pCamera == Cam)
			return;
	}
	
	m_CandidateMainCamera.push_back(_pCamera);
	// 만일 후보 메인카메라가 하나밖에 없는경우 그 카메라를 메인카메라로 지정한다.
	if (1 == m_CandidateMainCamera.size()) 
	{
		m_MainCamIter = m_CandidateMainCamera.begin();
		(*m_MainCamIter)->ActorOn();
	}

	m_MainCamEnd = m_CandidateMainCamera.end();
}
void CSceneRenderMgr::PopBackCandidateMainCam()
{
	std::list<SPTR<CCamera>>::reverse_iterator LastCam = m_CandidateMainCamera.rbegin();

	 m_CandidateMainCamera.pop_back();
	 m_MainCamEnd = m_CandidateMainCamera.end();

	 if (*LastCam == *m_MainCamIter) 
	 {
		 m_MainCamIter = m_CandidateMainCamera.begin();
	 }
}
SPTR<CCamera> CSceneRenderMgr::ChangeMainCamera()
{
	std::list<SPTR<CCamera>>::iterator Prev = m_MainCamIter;
	//(*Prev)->ActorOff();
	//(*Prev)->Off();
	++m_MainCamIter;

	if (m_MainCamIter == m_MainCamEnd) 
	{
		m_MainCamIter = m_CandidateMainCamera.begin();
	}

	//(*m_MainCamIter)->ActorOn();
	//(*m_MainCamIter)->On();
	CKeyMgr::InitMosuePos();

	return *m_MainCamIter;
}

SPTR<CCamera> CSceneRenderMgr::ChangeMainCamera(SPTR<CCamera> _pCamera)
{
	//std::list<SPTR<CCamera>>::iterator Prev = m_MainCamIter;

	std::list<SPTR<CCamera>>::iterator Start = m_CandidateMainCamera.begin();
	std::list<SPTR<CCamera>>::iterator End = m_CandidateMainCamera.end();

	for (;Start != End; ++Start)
	{
		if (_pCamera == *Start) 
		{
			std::list<SPTR<CCamera>>::iterator Prev = m_MainCamIter;
			m_MainCamIter = Start;
			CKeyMgr::InitMosuePos();
			break;
		}
	}

	return *m_MainCamIter;
}
void CSceneRenderMgr::ClearMainCameraList()
{
	m_CandidateMainCamera.clear();
}
const bool CSceneRenderMgr::PushRenderer(CRenderer* _Ren)
{
	if (nullptr == _Ren)
		return false;

	RENDER_LAYER Layer = _Ren->GetRenderLayer();

	if (Layer == RENDER_LAYER_DEBUG) 
	{
		m_DebugList.push_back(_Ren);
		return true;
	}

	auto FindIter = m_mapRendererList.find(Layer);
	
	// 해당 랜더 레이어에 소속된 렌더러 리스트가 없는경우
	if (m_mapRendererList.end() == FindIter)
	{
		auto Check = m_mapRendererList.insert(std::map< RENDER_LAYER, std::list<SPTR<CRenderer>>>::value_type(_Ren->GetRenderLayer(), std::list<SPTR<CRenderer>>()));

		if (true == Check.second) 
			Check.first->second.push_back(_Ren);
		else
			return false;
	}
	else 
		FindIter->second.push_back(_Ren);

	return true;
	
}

const bool CSceneRenderMgr::PushLight(CLight* _Light)
{
	if (nullptr == _Light)
		return false;

	m_listLight.push_back(_Light);
	return true;
}

SPTR<CCamera> CSceneRenderMgr::GetMainViewCamera()
{
	return *m_MainCamIter;
}
void CSceneRenderMgr::Render() 
{
	CDevice::ViewPortUpdate();

	// 기존 방식 : 랜더 매니져의 등록된 카메라 만큼 랜더링 수행
	/*
	m_CamStartIter = m_mapCamera.begin();
	m_CamEndIter = m_mapCamera.end();

	for (; m_CamStartIter!= m_CamEndIter; ++m_CamStartIter)
	{
		RenderCameraLoop(m_CamStartIter->second);
	}
	*/

	// 변경된 방식 : 랜더링 매니져에서 메인 카메라로 지정된 카메라만 랜더링 수행
	RenderCameraLoop(*m_MainCamIter);
	m_UICamera->UIRender();

	CDevice::Clear();
	CDevice::SetBackBufferView();
	CDevice::ViewPortUpdate();

	//캠 그리기
	//(*m_MainCamIter)->CamViewTargetRender();
	//UI 그리기
	//m_UICamera->CamViewTargetRender();

	CDevice::Clear();
	CDevice::SetBackBufferView();
	CDevice::ViewPortUpdate();

	MergeUITarget((*m_MainCamIter), m_UICamera);
}

void CSceneRenderMgr::Release()
{
	//m_mapCamera.clear();

	//for (auto& RenListIter : m_mapRendererList)
	//{
	//	if (true == RenListIter.second.empty())
	//		continue;

	//	RenListIter.second.clear();
	//}

	//m_mapRendererList.clear();
	CameraRelease();
	RendererRelease();
}


void CSceneRenderMgr::CameraRelease() 
{
	m_CamStartIter = m_mapCamera.begin();
	m_CamEndIter = m_mapCamera.end();

	for ( ;m_CamStartIter != m_CamEndIter; )
	{
		if (true == m_CamStartIter->second->IsDeath())
			m_CamStartIter = m_mapCamera.erase(m_CamStartIter);
		else
			++m_CamStartIter;
	}
}

void CSceneRenderMgr::RendererRelease() 
{
	std::list<SPTR<CRenderer>>::iterator RenStart;
	std::list<SPTR<CRenderer>>::iterator RenEnd;
	for (auto& RenListIter : m_mapRendererList)
	{
		if (true == RenListIter.second.empty())
			continue;

		RenStart = RenListIter.second.begin();
		RenEnd = RenListIter.second.end();

		for (; RenStart != RenEnd;)
		{
			if (true == (*RenStart)->IsDeath())
				RenStart = RenListIter.second.erase(RenStart);
			else
				++RenStart;
		}
	}

	std::list<SPTR<CRenderer>>::iterator DebugStart = m_DebugList.begin();
	std::list<SPTR<CRenderer>>::iterator DebugEnd = m_DebugList.end();

	for ( ; DebugStart != DebugEnd ; )
	{
		if (true == (*DebugStart)->IsDeath()) 
		{
			DebugStart = m_DebugList.erase(DebugStart);
		}
		else 
		{
			++DebugStart;
		}
	}
}

void CSceneRenderMgr::RenderCameraLoop(SPTR<CCamera> _Camera)
{
	CDevice::SetBaseDS();

	RENDER_LAYER Layer;

	if (false == _Camera->IsActive())
		return;


	_Camera->ClearCamTarget();
	pDefferredMRT->Clear();
	pDeferredLightMRT->Clear();

	CDevice::SetBaseDS();
	CDevice::ResetRenderTarget();
	CDevice::ResetShaderResource();

	ID3D11DepthStencilView* CamDV = _Camera->m_CameraViewTarget->GetDepthView();
	pDefferredMRT->SetMultiRenderTarget(CamDV);

	for (size_t i = 0; i < _Camera->m_vecViewRenderLayer.size(); i++)
	{
		Layer = _Camera->m_vecViewRenderLayer[i];
		m_FindRenListIter = m_mapRendererList.find(Layer);

		if (m_FindRenListIter == m_mapRendererList.end()
			|| true == m_FindRenListIter->second.empty())
			continue;

		CSceneRenderMgr::IsDeferred = true;
		DefferrdRender(_Camera, m_FindRenListIter->second);
		CSceneRenderMgr::IsDeferred = true;
	}

	CSceneRenderMgr::IsDeferred = false;
	DefferrdLightRender(_Camera);
	MergeCameraTarget(_Camera);

	CDevice::SetBaseDS();

	for (size_t i = 0; i < _Camera->m_vecViewRenderLayer.size(); i++)
	{
		Layer = _Camera->m_vecViewRenderLayer[i];
		m_FindRenListIter = m_mapRendererList.find(Layer);

		if (m_FindRenListIter == m_mapRendererList.end()
			|| true == m_FindRenListIter->second.empty())
			continue;

		//ForwardLightRender();
		ForwardRender(_Camera, m_FindRenListIter->second);
	}

	_Camera->FilterRender();

	if (false == GlobalDebug::bDebugMode) 
	{
		return;
	}

	CDevice::SetBaseDS();
	_Camera->SetCamViewTarget();
	ForwardRender(_Camera, m_DebugList);
}

void CSceneRenderMgr::DefferrdRender(SPTR<CCamera> _Camera, std::list<SPTR<CRenderer>>& _List) 
{
	m_listSingle.clear();
	m_listDecal.clear();
	m_mapFbxRenderGroup.clear();

	CDevice::ResetRenderTarget();
	CDevice::ResetShaderResource();
	CDevice::ResetShaderResourceTexture();

	ID3D11DepthStencilView* CamDV = _Camera->m_CameraViewTarget->GetDepthView();
	pDefferredMRT->SetMultiRenderTarget(CamDV);

	for (SPTR<CRenderer> Ren : _List)
	{
		if (false == Ren->IsActive())
			continue;

		//프러스텀 컬링
		//if (false == _Camera->CameraFrustumCheck(Ren))
		//{
		//	continue;
		//}

		//뎁스 컬링
		//if (true == _Camera->CameraLengthCheck(Ren))
		//{
		//	continue;
		//}

		//fbxRenderer이면 따로 맵에 담아서 랜더링한다
		if (true == Ren->EqualType<CFbxRenderer>())
		{
			CFbxRenderer* TempRen = (CFbxRenderer*)Ren.PTR;
			SPTR<CFbx> TempFbx = TempRen->GetRenderingFbx();
			if (nullptr == TempFbx)
			{
				continue;
			}
			int MeshIndex = (int)TempFbx->GetSingleFbxMesh(0)->GetMeshIdx();
			std::map<CFbx*, std::vector<InstValue>>::iterator Finditer = m_mapFbxRenderGroup.find(TempFbx);
			if (Finditer == m_mapFbxRenderGroup.end())
			{
				std::vector<InstValue> pVec;
				pVec.push_back(InstValue{ TempRen, MeshIndex });
				m_mapFbxRenderGroup.insert(std::map<CFbx*, std::vector<InstValue>>::value_type(TempFbx.GetPtr(), pVec));
			}
			else
			{
				Finditer->second.push_back(InstValue{ TempRen, MeshIndex });
			}
		}
		if (true == Ren->EqualType<CDecalRenderer>())
		{
			m_listDecal.push_back(Ren);
		}
		else
		{
			Ren->RenderUpdate(_Camera);
			CDevice::RenBaseCB->SupplyShader();
			Ren->Render(_Camera);
			Ren->RenderRelease();
		}
	}

	//fbx랜더러 맵에서 vector사이즈가 1인 싱글 랜더러들은 따로 빼서 랜더링해준다
	std::map<CFbx*, std::vector<InstValue>>::iterator mapSIter = m_mapFbxRenderGroup.begin();
	std::map<CFbx*, std::vector<InstValue>>::iterator mapEIter = m_mapFbxRenderGroup.end();
	for (; mapSIter != mapEIter; ++mapSIter)
	{
		if (1 == mapSIter->second.size())
		{
			mapSIter->second[0].InstRenderer->RenderUpdate(_Camera);
			CDevice::RenBaseCB->SupplyShader();
			mapSIter->second[0].InstRenderer->Render(_Camera);
			mapSIter->second[0].InstRenderer->RenderRelease();
		}
		else
		{
 			for (size_t i = 0; i < mapSIter->second.size(); ++i)
			{
				////인스턴싱 랜더
				//mapSIter->second[i].InstRenderer->RenderUpdate(_Camera);

				tInstancingData tIsntData = {};
				tIsntData.matWorld = mapSIter->second[i].InstRenderer->GetSubTransCRWorld();
				tIsntData.matWV = tIsntData.matWorld * _Camera->m_matView;
				tIsntData.matWVP = tIsntData.matWV * _Camera->m_matProj;

				CInstancingBuffer::Inst()->AddInstancingData(tIsntData);
			}
			CInstancingBuffer::Inst()->SetData();
			CDevice::RenBaseCB->SupplyShader();
			mapSIter->second[0].InstRenderer->InstancingRender(_Camera);
			CInstancingBuffer::Inst()->Clear();
		}
	}

	CDevice::ResetRenderTarget();
	CDevice::ResetShaderResource();
	CDevice::ResetShaderResourceTexture();
	pDecalMRT->SetMultiRenderTarget(CamDV);

	for (SPTR<CRenderer> Ren : m_listDecal)
	{
		if (false == Ren->IsActive())
			continue;

		Ren->RenderUpdate(_Camera);
		CDevice::RenBaseCB->SupplyShader();
		Ren->Render(_Camera);
		Ren->RenderRelease();
	}

}

void CSceneRenderMgr::ForwardRender(SPTR<CCamera> _Camera, std::list<SPTR<CRenderer>>& _List)
{
	for (SPTR<CRenderer> Ren : _List)
	{
		if (false == Ren->IsActive()) 
		{
			continue;
		}

		Ren->RenderUpdate(_Camera);
		CDevice::RenBaseCB->SupplyShader();
		Ren->Render(_Camera);
		Ren->RenderRelease();
	}
}

void CSceneRenderMgr::DefferrdLightRender(SPTR<CCamera> _Camera)
{
	CDevice::SetDisableDS();
	CDevice::ResetRenderTarget();
	CDevice::ResetShaderResource();

	ID3D11DepthStencilView* CamDV = _Camera->m_CameraViewTarget->GetDepthView();
	pDeferredLightMRT->SetMultiRenderTarget(CamDV);

	for (SPTR<CLight> Light : m_listLight )
	{
		if (false == Light->IsActive())
			continue;

		Light->Lighting(_Camera);
	}
}

void CSceneRenderMgr::MergeCameraTarget(SPTR<CCamera> _Camera)
{
	CDevice::SetBaseRS();
	CDevice::ResetRenderTarget();
	CDevice::ResetShaderResource();
	_Camera->SetCamViewTarget();

	pDefferredMesh->Update();
	pCamMergeMaterial->Update();
	pDefferredMesh->Render();
	pCamMergeMaterial->TexSamRollBack();
}

void CSceneRenderMgr::MergeUITarget(SPTR<CCamera> _CamTarget, SPTR<CCamera> _UITarget)
{
	_CamTarget->m_CameraViewTarget->GetRenderTarget()->GetRTTexture()->Update(0, SHADER_PIXEL);
	_UITarget->m_CameraViewTarget->GetRenderTarget()->GetRTTexture()->Update(1, SHADER_PIXEL);

	pDefferredMesh->Update();
	pUIMergeMaterial->Update();
	pDefferredMesh->Render();
	_CamTarget->m_CameraViewTarget->GetRenderTarget()->GetRTTexture()->PopShaderResource(0, SHADER_PIXEL);
	_UITarget->m_CameraViewTarget->GetRenderTarget()->GetRTTexture()->PopShaderResource(1, SHADER_PIXEL);
	pUIMergeMaterial->TexSamRollBack();
}

void CSceneRenderMgr::ForwardLightRender()
{
	int Index = 0;
	for (SPTR<CLight> Light : m_listLight)
	{
		if (false == Light->IsActive())
			continue;
		CSceneRenderMgr::FLightBuffer.ArrLightData[Index] = Light->m_LightBufferData;
		++Index;
		CSceneRenderMgr::FLightBuffer.LightCount = Index;	
	}
}

void CSceneRenderMgr::Clear()
{
	m_mapCamera.clear();

	for (std::map<RENDER_LAYER, std::list<SPTR<CRenderer>>>::value_type& RenList : m_mapRendererList)
	{                                                                                                                                                      
		if (true == RenList.second.empty())
			continue;

		RenList.second.clear();
	}
	
	if (false == m_DebugList.empty())
		m_DebugList.clear();
	

	if (false == m_listLight.empty())
		m_listLight.clear();


	m_CandidateMainCamera.clear();
	m_MainCamIter = m_CandidateMainCamera.end();
	m_MainCamEnd = m_CandidateMainCamera.end();
}
////////////////////////////////////		Static Scene Function		/////////////////////////////////////
void CScene::SceneRelease()
{
	pCurScene = nullptr;
	pNextScene = nullptr;
	CSceneRenderMgr::pDefferredMRT = nullptr;
	CSceneRenderMgr::pDefferredMesh = nullptr;
	CSceneRenderMgr::pDefferredMaterial = nullptr;
}

SPTR<CScene> CScene::FindScene(const std::wstring& _SceneName)
{
	return CResourceMgr<CScene>::Find(_SceneName);
}
const bool CScene::ChangeScene(const std::wstring& _SceneName) 
{
	SPTR<CScene> ChangeScene = CResourceMgr<CScene>::Find(_SceneName);

	if (nullptr == ChangeScene)
	{
		TASSERT(true);
		return false;
	}

	pNextScene = ChangeScene;

	bFixFrame = false;
	fFixDeltaTime = 0.f;
	fAccSceneDeltaTime = 0.f;
	return true;
}
const bool CScene::ChangeScene(const std::wstring& _SceneName, const float _FixFps)
{
	SPTR<CScene> ChangeScene = CResourceMgr<CScene>::Find(_SceneName);

	if (nullptr == ChangeScene)
	{
		TASSERT(true);
		return false;
	}

	pNextScene = ChangeScene;

	bFixFrame = true;
	fFixDeltaTime = 1.f / _FixFps;
	fAccSceneDeltaTime = 0.f;
	return true;
}
void CScene::SceneProgress(const float& _DeltaTime)
{
	if (nullptr != pNextScene)
	{
		if (nullptr != pCurScene)
			pCurScene->ShutDown();

		pNextScene->WakeUp();
		CScene::pCurScene = CScene::pNextScene;
		CScene::pNextScene = nullptr;
		return;
	}
	
	if (nullptr == pCurScene)
	{
		return;
	}

	pCurScene->Progress(_DeltaTime);

	//if (true == bFixFrame) 
	//{
	//	if (fAccSceneDeltaTime < fFixDeltaTime) 
	//	{
	//		fAccSceneDeltaTime += _DeltaTime;
	//		return;
	//	}

	//	pCurScene->Progress(fFixDeltaTime);

	//	while (fAccSceneDeltaTime >= fFixDeltaTime) 
	//	{
	//		fAccSceneDeltaTime -= fFixDeltaTime;
	//	}
	//}
	//else 
	//{
	//	pCurScene->Progress(_DeltaTime);
	//}


}

CActor*  CScene::CreateActorNoneTransform(CScene* _pScene, const ACTOR_UPDATE_LAYER& _Layer, const std::wstring& _Name)
{
	if (nullptr == _pScene)
		return nullptr;

	CActor* NewActor = Engine::CreateGameObject<CActor>(_pScene);

	NewActor->SetName(_Name);
	NewActor->SetParentScene(_pScene);

	if (false == _pScene->PushActor(NewActor, _Layer))
	{
		TASSERT(true);
		return nullptr;
	}

	NewActor->Init();
	return NewActor;
}

CActor* CScene::CreateActor(CScene* _pScene, const ACTOR_UPDATE_LAYER& _Layer, const std::wstring& _Name)
{
	if (nullptr == _pScene)
		return nullptr;

	CActor* NewActor = Engine::CreateGameObject<CActor>(_pScene);

	NewActor->SetName(_Name);
	NewActor->SetParentScene(_pScene);
	
	if (false == _pScene->PushActor(NewActor, _Layer))
	{
		TASSERT(true);
		return nullptr;
	}

	NewActor->Init();
	NewActor->AddCom<CTransForm>();
	return NewActor;
}


////////////////////////////////////////		Member Scene Func		///////////////////////////////////
CScene::CScene()
	: m_bLoadingComplete(false), m_PairCollision2DMgr(CSceneCollisionMgr(this))
	, m_PairRenderMgr(CSceneRenderMgr(this)), m_PairCollision3DMgr(CSceneCollisionMgr(this))
{
	
}

CScene::~CScene()
{
	m_listSceneScript.clear();
	m_mapActorList.clear();
}

void CScene::Init()
{
	// 마우스와 기즈모 충돌체를 기본으로 링크한다.
	m_PairCollision3DMgr.LinkCollsionChannel(L"Mouse", L"Gizmo");

	// 씬의 메인카메라 생성( 인덱스 0)
	CActor* MainCamActor = CreateActor(this, UPDATE_LAYER_LAST,L"MainCameraActor");
	CCamera* pCamera = MainCamActor->AddCom<CCamera>(CAMDATA{ CAMERA_LAYER_OBJECT, 45.f ,1.f, 50000.f });
	pCamera->SetPerspectiveMode();
	pCamera->SetWheelZoom(true);
	pCamera->SettingViewLayer(RENDER_LAYER_MAP, RENDER_LAYER_OBJECT, RENDER_LAYER_BACKEFFECT, RENDER_LAYER_PLAYER, RENDER_LAYER_FRONTEFFECT ,RENDER_LAYER_FIRST, RENDER_LAYER_ALPHA);
	m_vecSceneCamera.push_back(pCamera);
	m_PairRenderMgr.RegisteredMainCamera(pCamera);

	//씬 UI 카메라 생성( 인덱스 1 )
	CActor* UICamActor = CreateActor(this, UPDATE_LAYER_LAST, L"UICameraActor");
	CCamera* pUICamera = UICamActor->AddCom<CCamera>(CAMDATA{ CAMERA_LAYER_UI , 45.f ,1.f, 10000.f });
	pUICamera->SettingViewLayer(RENDER_LAYER_BACKUI, RENDER_LAYER_UI, RENDER_LAYER_FRONTUI);
	m_vecSceneCamera.push_back(pUICamera);
	pUICamera->SetOrthogonalMode();
	m_PairRenderMgr.m_UICamera = pUICamera;
	pUICamera->GetCamTarget(0)->SetTargetClearColor(UICLEARCOLOR);
	//pUICamera->SetWheelZoom(false);

	//// 씬 UI카메라 인덱스 1
}

const bool CScene::Loading()
{
	if (true == m_bLoadingComplete )
		return false;

	for ( SPTR<CSceneScript> Script : m_listSceneScript)
	{
		if (nullptr != Script && false == Script->m_bLoading)
		{
			Script->Loading();
			Script->m_bLoading = true;
		}
	}

	m_bLoadingComplete = true;
	return true;
}
void CScene::Progress(const float& _DeltaTime)
{
	CProfiler::TotalTimerStart();

	CProfiler::UpdateTimerStart();
	pCurScene->Update(_DeltaTime);
	CProfiler::UpdateTimerEnd();

	CProfiler::CollisionTimerStart();
	pCurScene->Collision();
	CProfiler::CollisionTimerEnd();

	CProfiler::RenderTimerStart();
	pCurScene->Render();
	CProfiler::RenderTimerEnd();

	CProfiler::PresentTimerStart();
	CDevice::Present();
	CProfiler::PresentTimerEnd();

	CProfiler::LastUpdateTimerStart();
	pCurScene->LastUpdate(_DeltaTime);
	CProfiler::LastUpdateTimerEnd();

	LARGE_INTEGER TempTotal = CProfiler::TotalTimerEnd();

	CProfiler::UpdateProfilingRatio(TempTotal);
	CProfiler::CollisionProfilingRatio(TempTotal);
	CProfiler::RenderProfilingRatio(TempTotal);
	CProfiler::PresentProfilingRatio(TempTotal);
	CProfiler::LastUpdateProfilingRatio(TempTotal);

	pCurScene->Release();
}
void CScene::Update(const float& _DeltaTime)
{
	for (SPTR<CSceneScript> Script : m_listSceneScript)
	{
		if (nullptr != Script)
			Script->PreUpdate(_DeltaTime);
	}

	// Actor 업데이트
	for (auto& ActorList : m_mapActorList) 
	{
		for(SPTR<CActor> Actor : ActorList.second)
		{
			if(Actor->IsActive())
				Actor->PreUpdate(_DeltaTime);
		}
	}
	
	for (auto& ActorList : m_mapActorList)
	{
		for (SPTR<CActor> Actor : ActorList.second)
		{
			if (Actor->IsActive())
				Actor->Update(_DeltaTime);
		}
	}


	for (auto& ActorList : m_mapActorList)
	{
		for (SPTR<CActor> Actor : ActorList.second)
		{
			if (Actor->IsActive())
				Actor->PostUpdate(_DeltaTime);
		}
	}


	for (SPTR<CSceneScript> Script : m_listSceneScript)
	{
		if (nullptr != Script)
			Script->PostUpdate(_DeltaTime);
	}
}

void CScene::Collision()
{
	m_PairCollision2DMgr.Collision();
	m_PairCollision3DMgr.Collision();
}
void CScene::Collision3D()
{
	m_PairCollision3DMgr.Collision();
}
// 스크립트 전용 디버그 랜더 호출
void CScene::DebugRender()
{
	for (SPTR<CScript> Script : m_listActorScript)
	{
		if(true == Script->IsActive())
			Script->DebugRender();
	}
}

void CScene::Render()
{
	CCriSectionObject<CDevice> Lock;
	m_PairRenderMgr.Render();

	for (SPTR<CSceneScript> SceneScript : m_listSceneScript)
		SceneScript->SceneRender();

	if (GlobalDebug::bDebugMode) 
	{
		DebugRender();
		m_PairCollision2DMgr.DebugRender();
		m_PairCollision3DMgr.DebugRender();

		CProfiler::DebugRender();

		for (SPTR<CSceneScript> SceneScript : m_listSceneScript)
			SceneScript->DebugRender();
	}
}

void CScene::LastUpdate(const float& _DeltaTime)
{
	for (auto& ActorList : m_mapActorList) 
	{
		for (SPTR<CActor> Actor : ActorList.second)
		{
			if(nullptr != Actor)
				Actor->LastUpdate(_DeltaTime);
		}
	}

	for (SPTR<CSceneScript> Script : m_listSceneScript)
	{
		if (nullptr != Script)
			Script->LastUpdate(_DeltaTime);
	}

	m_PairCollision2DMgr.LastUpdate();
	m_PairCollision3DMgr.LastUpdate();
}

void CScene::Release() 
{
	for (auto& ActorList : m_mapActorList) 
	{
		std::list<SPTR<CActor>>::iterator StartActor = ActorList.second.begin();
		std::list<SPTR<CActor>>::iterator EndActor = ActorList.second.end();
			
		for (; StartActor != EndActor; )
		{
			if (true == (*StartActor)->IsDeath()
				|| nullptr !=  (*StartActor)->m_pParentActor ) // 액터의 부모가 있는 경우 씬 리스트에서는 제외한다.
			{
				StartActor = ActorList.second.erase(StartActor);
			}
			else
			{
				// 자기 안의 컴포넌트와 차일드 엑터를 릴리즈하는 작업을 수행
				(*StartActor)->PushWaitChild();
				(*StartActor)->Release();
				++StartActor;
			}
		}
	}
	// 랜더 & 컬라이더 매니져에서도 릴리즈를 실행해야한다.
	m_PairRenderMgr.Release();
	m_PairCollision2DMgr.Release();
	m_PairCollision3DMgr.Release();
}

void CScene::ReduceRelease()
{
	for (auto& ActorList : m_mapActorList)
	{
		std::list<SPTR<CActor>>::iterator StartActor = ActorList.second.begin();
		std::list<SPTR<CActor>>::iterator EndActor = ActorList.second.end();

		for (; StartActor != EndActor; )
		{
			if (true == (*StartActor)->IsDeath()
				|| nullptr != (*StartActor)->m_pParentActor) // 액터의 부모가 있는 경우 씬 리스트에서는 제외한다.
			{
				StartActor = ActorList.second.erase(StartActor);
			}
			else
			{
				// 자기 안의 컴포넌트와 차일드 엑터를 릴리즈하는 작업을 수행
				(*StartActor)->PushWaitChild();
				(*StartActor)->Release();
				++StartActor;
			}
		}
	}

	m_PairCollision3DMgr.Release();

}
void CScene::WakeUp()
{

	// 런타임중 씬스크립트가 추가된 경우
	// 스크립트 리스트에서 로딩이 되지 않은 스크립트를 로딩시킨다.
	if(false == m_bLoadingComplete)
		Loading();
	
	for (SPTR<CSceneScript> Script : m_listSceneScript)
	{
		if(nullptr != Script)
			Script->WakeUp();
	}

}

void CScene::ShutDown()
{
	for (SPTR<CSceneScript> Script : m_listSceneScript)
	{
		if (nullptr != Script)
			Script->ShutDown();
	}
}

const bool CScene::PushActor(CActor* _Actor, const ACTOR_UPDATE_LAYER& _Layer)
{
	if (nullptr == _Actor)
		return false;

	_Actor->SetUpdateLayer(_Layer);

	std::map<int, std::list<SPTR<CActor>>>::iterator MapIter = m_mapActorList.find(_Layer);

	// 새로운 레이어 번호가 들어온 경우
	if (m_mapActorList.end() == MapIter)
	{
		std::list<SPTR<CActor>> NewList;
		auto InsertIter =  m_mapActorList.insert(std::map<int, std::list<SPTR<CActor>>>::value_type(_Layer, NewList));
	
		if(true == InsertIter.second)
			InsertIter.first->second.push_back(_Actor);
	}
	// 기존 레이어 번호가 들어온 경우
	else
		MapIter->second.push_back(_Actor);

	return true;
}

const bool CScene::PushCollider(const SCENECOLLISION& _Type,CCollider* _Collider)
{
	if (nullptr == _Collider)
		return false;

	switch (_Type)
	{
	case COLLISION_2D:
		return m_PairCollision2DMgr.PushCollider(_Collider);
	case COLLISION_3D:
		return m_PairCollision3DMgr.PushCollider(_Collider);
	default:
		break;
	}
	return false;
}

const bool CScene::PushScript(CScript* _Script)
{
	if (nullptr == _Script)
		return false;

	m_listActorScript.push_back(_Script);
	return true;
}

const bool CScene::PushRenderer(CRenderer* _Renderer) 
{
	if (nullptr == _Renderer)
		return false;

	return m_PairRenderMgr.PushRenderer(_Renderer);
}

const bool CScene::PushCamera(CCamera* _Camera) 
{
	if (nullptr == _Camera)
		return false;

	return m_PairRenderMgr.PushCamera(_Camera);
}

void CScene::RegisteredMainCamera(SPTR<CCamera> _pCamera)
{
	m_PairRenderMgr.RegisteredMainCamera(_pCamera);
}
void CScene::PopBackMainCamera()
{
	m_PairRenderMgr.PopBackCandidateMainCam();
}

SPTR<CCamera> CScene::ChangeMainCamera()
{
	return m_PairRenderMgr.ChangeMainCamera();
}
SPTR<CCamera> CScene::ChangeMainCamera(SPTR<CCamera> _pCamera)
{
	return m_PairRenderMgr.ChangeMainCamera(_pCamera);
}
void CScene::ClearScene()
{
	m_vecSceneCamera.clear();

	m_PairCollision2DMgr.Clear();
	m_PairCollision3DMgr.Clear();
	m_PairRenderMgr.Clear();

	std::list<SPTR<CActor>>::iterator Start;
	std::list<SPTR<CActor>>::iterator End;

	for (std::map<int, std::list<SPTR<CActor>>>::value_type& ActorList : m_mapActorList)
	{
		if (true == ActorList.second.empty())
		{
			continue;
		}

		ActorList.second.clear();
	}

	m_listActorScript.clear();
}
const bool CScene::Create(const CREATE_DESC& _Desc)
{
	SetName(_Desc.Name);

	return true;
}

