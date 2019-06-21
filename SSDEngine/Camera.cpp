#include "Camera.h"
#include "TransForm.h"
#include "SSDCore.h"
#include "Device.h"
#include "Actor.h"
#include "TransForm.h"
#include "KeyMgr.h"
#include "Scene.h"
#include "TimerMgr.h"
#include "DXMath.h"
#include "CameraScript.h"
#include "SSDCore.h"
#include "MultiRenderTarget.h"
#include "Mesh.h"
#include "Material.h"
#include "SamplerState.h"
#include <algorithm>
#include "FilterEffect.h"
#include "Renderer.h"
#include "Frustum.h"

// 한 엑터 안에 카메라 컴포넌트는 2개가 들어올 수 없다.

const bool CCamera::IsMultiComponentCheck(CActor* _pActor) 
{
	if (nullptr == _pActor)
		return false;

	// 엑터 안에 카메라가 있으므로 트랜스폼 컴포넌트가 들어갈수 없다.
	if (nullptr != _pActor->FindCom<CCamera>())
		return false;

	return true;
}



CCamera::CCamera()
	:m_eProjMode(ORTHO_CAMERA), m_fNear(1.f), m_fFar(50000.f),m_fFov(0.f)
	, m_eScreenViewMode(SCREENVIEW_WINDOW), m_CameraGroup(CAMERA_GROUP(0)), m_bWheelZoom(false)
	, m_CurZoomMag(1.f), m_ScreenViewSize(Vec2()), m_CameraViewTarget(nullptr), m_CameraMesh(nullptr), m_CameraMaterial(nullptr)
	, m_eViewAtType(VIEW_AT_Z), m_BlurViewMat(Mat()), m_fBlurTime(0.f), m_fAccBlurTime(0.f), m_bBluring(false)
	, m_fBlurPow(2.f), m_BlurViewPos(Vec3())
{
	m_vecViewRenderLayer.clear();
	m_listEyePivot.clear();
}
CCamera::CCamera(CActor* _Actor)
	: CBaseComponent(_Actor), m_eProjMode(ORTHO_CAMERA), m_fNear(1.f), m_fFar(50000.f), m_fFov(0.f)
	, m_eScreenViewMode(SCREENVIEW_FREE), m_CameraGroup(CAMERA_GROUP(0)), m_bWheelZoom(false), m_ScreenViewSize(Vec2())
	, m_CurZoomMag(1.f), m_CameraViewTarget(nullptr), m_CameraMesh(nullptr), m_CameraMaterial(nullptr)
	, m_eViewAtType(VIEW_AT_Z), m_BlurViewMat(Mat()), m_fBlurTime(0.f), m_fAccBlurTime(0.f), m_bBluring(false)
	, m_fBlurPow(2.f), m_BlurViewPos(Vec3())
{
	m_vecViewRenderLayer.clear();
	m_listEyePivot.clear();
}

CCamera::~CCamera()
{
	m_vecViewRenderLayer.clear();
	m_listCameraScript.clear();
	m_listEyePivot.clear();
}

bool CCamera::CameraFrustumCheck(SPTR<CRenderer> _Ren)
{
	//return m_CamFrustum->FrustumPointCheck(_Ren->GetTrans()->GetWorldPos());
	return m_CamFrustum->FrustumSphereCheck(_Ren->GetTrans()->GetWorldPos(), 500.f);
}

bool CCamera::CameraLengthCheck(SPTR<class CRenderer> _Ren)
{
	Vec3 Length = _Ren->GetTrans()->GetWorldPos() - TRANS->GetWorldPos();
	float Len = Length.Length();
	if (3000.f <= Len)
	{
		return true;
	}
	return false;
}

SPTR<CFrustum>	CCamera::GetCameraFrustum()
{
	return m_CamFrustum;
}
void CCamera::SetCamViewTarget()
{
	if (nullptr != m_CameraViewTarget)
		m_CameraViewTarget->SetMultiRenderTarget();
}

void CCamera::ClearCamTarget()
{
	if (nullptr != m_CameraViewTarget)
		m_CameraViewTarget->Clear();
}

void CCamera::CamViewTargetRender()
{
	DXCONTEXT->RSSetViewports(1, &m_CamViewPort);
	//슬롯 자동으로 바꿀 수 있게 업데이트 해야함
	m_CameraViewTarget->GetRenderTarget()->GetRTTexture()->Update(0, SHADER_PIXEL);
	m_CameraMesh->Update();
	m_CameraMaterial->Update();
	m_CameraMesh->Render(0);
	
	m_CameraViewTarget->GetRenderTarget()->GetRTTexture()->PopShaderResource(0, SHADER_PIXEL);
	m_CameraMaterial->TexSamRollBack();
}

void CCamera::SetCamViewPort(Vec2 _StartPos, IVec2 _Size)
{
	m_CamViewPort.TopLeftX = (float)_StartPos.ix;
	m_CamViewPort.TopLeftY = (float)_StartPos.iy;

	m_CamViewPort.Width = (float)_Size.ix;
	m_CamViewPort.Height = (float)_Size.iy;

	m_CamViewPort.MinDepth = 0.f;
	m_CamViewPort.MaxDepth = 1.f;
}

void CCamera::PushViewLayer(RENDER_LAYER _Layer)
{
	for (auto Value : m_vecViewRenderLayer)
	{
		if (Value == _Layer)
			return;
	}

	m_vecViewRenderLayer.push_back(_Layer);
}
void CCamera::SetCamData(const CAMDATA& _Data)
{
	m_fFov = _Data.Fov;
	m_fNear = _Data.Near;
	m_fFar = _Data.Far;

	m_matView.SetIdentityMat();
	m_matProj.SetIdentityMat();
}

const bool CCamera::Init(const CAMDATA& _InitData)
{
	m_CameraGroup = _InitData.CamGroup;

	m_fFov = _InitData.Fov;
	m_fNear = _InitData.Near;
	m_fFar = _InitData.Far;

	m_matView.SetIdentityMat();
	m_matProj.SetIdentityMat();

	m_pBelongScene->GetRenderMgr()->PushCamera(this);

	m_CameraMesh = CResourceMgr<CMesh>::Find(L"TargetViewMesh");
	m_CameraMaterial = CResourceMgr<CMaterial>::Find(L"TargetMaterial");

	RT::CREATE_DESC RTDesc;

	RTDesc.Width = (UINT)Core::GetWindowWidth();
	RTDesc.Height = (UINT)Core::GetWindowHeight();
	RTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	m_CameraViewTarget = new MultiRenderTarget();
	RenderTarget* TempRenderTarget = new RenderTarget();
	TempRenderTarget->Create(RTDesc);
	m_CameraViewTarget->CreateDepth();
	m_CameraViewTarget->PushRenderTarget(TempRenderTarget);

	SetCamViewPort(IVec2(0, 0), IVec2(Core::GetWindowWidth(), Core::GetWindowHeight()));
	
	//Frustum
	m_CamFrustum = new CFrustum(this);
	m_CamFrustum->Init();

	return true;
}

void CCamera::PreUpdate(const float& _DeltaTime)
{
	m_listEyePivot.clear();

	for (SPTR<CCameraScript> CamScript : m_listCameraScript) 
	{
		if (nullptr != CamScript && true == CamScript->IsActive())
			CamScript->PreUpdate(_DeltaTime);
	}

	for (size_t i = 0; i < m_mapUIBase.size(); ++i)
	{
		m_UImapiter = m_mapUIBase.find((UI_LAYER)i);
		if (m_UImapiter == m_mapUIBase.end()
			|| true == m_UImapiter->second.empty())
			continue;

		for (SPTR<CUIBase> UIBaseIter : m_UImapiter->second)
		{
			if (nullptr != UIBaseIter && true == UIBaseIter->IsActive())
				UIBaseIter->PreUpdate(_DeltaTime);
		}
	}
}

void CCamera::Update(const float& _DeltaTime)
{
	for (SPTR<CCameraScript> CamScript : m_listCameraScript) 
	{
		if (nullptr != CamScript && true == CamScript->IsActive())
			CamScript->Update(_DeltaTime);
	}

	if (m_eProjMode == PERSPECTIVE_CAMERA)
		return;

	if (true == m_bWheelZoom &&  true == CKeyMgr::GetWheelEvent())
	{
		// 앞으로 밀면 휠제타 값이 양수가 나온다
		if (0 < CKeyMgr::GetWheelSign())
			m_CurZoomMag -= 0.01f;
		// 뒤로 땡기면 휠 제타 값이 음수가 나온다
		else
			m_CurZoomMag += 0.01f;
	}

	for (size_t i = 0; i < m_mapUIBase.size(); ++i)
	{
		m_UImapiter = m_mapUIBase.find((UI_LAYER)i);
		if (m_UImapiter == m_mapUIBase.end()
			|| true == m_UImapiter->second.empty())
			continue;

		for (SPTR<CUIBase> UIBaseIter : m_UImapiter->second)
		{
			if (nullptr != UIBaseIter && true == UIBaseIter->IsActive())
				UIBaseIter->Update(_DeltaTime);
		}
	}
}

void CCamera::PostUpdate(const float& _DeltaTime)
{
	for (SPTR<CCameraScript> CamScript : m_listCameraScript) 
	{
		if(nullptr != CamScript && true == CamScript->IsActive())
			CamScript->PostUpdate(_DeltaTime);
	}


	Vec3 Eye = TRANS->GetWorldPos();

	for (Vec3& Pivot :m_listEyePivot)
		Eye += Pivot;
	
	Vec3 AtDir;
	Vec3 UpDir;

	switch (m_eViewAtType)
	{
	case VIEW_AT_Z:
	{
		AtDir = TRANS->GetWorldAxis(AXIS_Z);
		UpDir = TRANS->GetWorldAxis(AXIS_Y);
	}
		break;
	case VIEW_AT_INVZ:
	{
		AtDir = -TRANS->GetWorldAxis(AXIS_Z);
		UpDir = TRANS->GetWorldAxis(AXIS_Y);
	}
	break;
	case VIEW_AT_Y: 
	{
		AtDir = TRANS->GetWorldAxis(AXIS_Y);
		UpDir = TRANS->GetWorldAxis(AXIS_Z);
	}
		break;
	default:
	{
		TASSERT(true);
		return;
	}
		break;
	}

	m_matView.SetViewToMat(Eye, AtDir, UpDir);

	if (true == m_bBluring)
	{
		BluringCamera(_DeltaTime);
	}
	
	switch (m_eProjMode)
	{
	case CCamera::ORTHO_CAMERA: 
	{
		switch (m_eScreenViewMode)
		{
		case CCamera::SCREENVIEW_WINDOW:
			m_matProj.SetOrthoProjMat((float)Core::GetClientWidth()*m_CurZoomMag, (float)Core::GetClientHeight()*m_CurZoomMag, m_fNear, m_fFar);
			break;
		case CCamera::SCREENVIEW_FREE:
			m_matProj.SetOrthoProjMat(m_ScreenViewSize.x*m_CurZoomMag, m_ScreenViewSize.y*m_CurZoomMag, m_fNear, m_fFar);
			break;
		default:
			m_matProj.SetIdentityMat();
			break;
		}
	}
		break;
	case CCamera::PERSPECTIVE_CAMERA:
	{
		switch (m_eScreenViewMode)
		{
		case CCamera::SCREENVIEW_WINDOW:
			m_matProj.SetFovPersProjMat_Angle(m_fFov, Core::GetWindowAspect(), m_fNear, m_fFar);
			m_FrustumProj.SetFovPersProjMat_Angle(m_fFov * 2.f, Core::GetWindowAspect(), 0.001f, m_fFar);
			break;
		case CCamera::SCREENVIEW_FREE:
			m_matProj.SetFovPersProjMat_Angle(m_fFov, m_ScreenViewSize.x / m_ScreenViewSize.y, m_fNear, m_fFar);
			m_FrustumProj.SetFovPersProjMat_Angle(m_fFov * 2.f, Core::GetWindowAspect(), 0.001f, m_fFar);
			break;
		default:
			m_matProj.SetIdentityMat();
			break;
		}
	}
		break;
	default:
		m_matProj.SetIdentityMat();
		break;
	}

	for (SPTR<CCameraScript> CamScript : m_listCameraScript)
	{
		if (nullptr != CamScript && true == CamScript->IsActive())
			CamScript->LastCamScriptUpdate(_DeltaTime);
	}



	m_CamFrustum->Update();



	for (size_t i = 0; i < m_mapUIBase.size(); ++i)
	{
		m_UImapiter = m_mapUIBase.find((UI_LAYER)i);
		if (m_UImapiter == m_mapUIBase.end()
			|| true == m_UImapiter->second.empty())
			continue;

		for (SPTR<CUIBase> UIBaseIter : m_UImapiter->second)
		{
			if (nullptr != UIBaseIter && true == UIBaseIter->IsActive())
				UIBaseIter->PostUpdate(_DeltaTime);
		}
	}
}

void CCamera::LastUpdate(const float& _DeltaTime) 
{
	for (SPTR<CCameraScript> CamScript : m_listCameraScript)
		CamScript->LastUpdate(_DeltaTime);

	for (size_t i = 0; i < m_mapUIBase.size(); ++i)
	{
		m_UImapiter = m_mapUIBase.find((UI_LAYER)i);
		if (m_UImapiter == m_mapUIBase.end()
			|| true == m_UImapiter->second.empty())
			continue;

		for (SPTR<CUIBase> UIBaseIter : m_UImapiter->second)
		{
			if (nullptr != UIBaseIter && true == UIBaseIter->IsActive())
				UIBaseIter->LastUpdate(_DeltaTime);
		}
	}
}

void CCamera::Release()
{
	//if (true != m_listUIBase.empty())
	//{
	//	m_listUIBase.erase(std::remove_if(m_listUIBase.begin(), m_listUIBase.end(), [](SPTR<CUIBase> _Script)->bool
	//	{
	//		if (true == _Script->IsDeath())
	//			_Script->Release();

	//		return _Script->IsDeath();
	//	}), m_listUIBase.end());
	//}

	if (m_listCameraScript.empty() )
		return;

	m_listCameraScript.erase(std::remove_if(m_listCameraScript.begin(), m_listCameraScript.end(), [](SPTR<CCameraScript> _Script)->bool 
	{
		if (true == _Script->IsDeath())
			_Script->Release();

		return _Script->IsDeath();
	}), m_listCameraScript.end());
}

void CCamera::SetPos(const Vec3& _Pos)
{
	TRANS->SetPos(_Pos);
}

void CCamera::SetPos(const Vec2& _Pos)
{
	TRANS->SetPos(_Pos);
}

const Vec2 CCamera::GetOrthoViewSize()
{
	if (m_eScreenViewMode == SCREENVIEW_WINDOW)
		return Vec2((float)Core::GetWindowWidth() * m_CurZoomMag, (float)Core::GetWindowHeight() * m_CurZoomMag);
	else if (m_eScreenViewMode == SCREENVIEW_FREE)
		return Vec2(m_ScreenViewSize.x * m_CurZoomMag, m_ScreenViewSize.y * m_CurZoomMag);

	return Vec2(0.f, 0.f);
}

const bool CCamera::IsCameraMove()
{
	return this->IsTransMove();
	//return TRANS->ChangeTransform();
}

const Vec3 CCamera::GetPos()
{
	return TRANS->GetWorldPos();
}
const Vec3 CCamera::GetRot()
{
	return TRANS->GetRotate();
}

const Vec3 CCamera::GetMousePos()
{
	Vec3 MousePos;
	MousePos = Core::GetScreenMousePos();

	switch (m_eScreenViewMode)
	{
	case CCamera::SCREENVIEW_WINDOW:
	{
		MousePos.x = (MousePos.x - Core::GetClientWidth() * 0.5f) * m_CurZoomMag;
		MousePos.y = (-(MousePos.y - Core::GetClientHeight() * 0.5f)) * m_CurZoomMag;
		MousePos.vec2 = MousePos.vec2 + TRANS->GetWorldPos().vec2 ;
	}
	return MousePos ;
	case CCamera::SCREENVIEW_FREE:
	{
		MousePos.x = ( MousePos.x - m_ScreenViewSize.x * 0.5f) * m_CurZoomMag;
		MousePos.y = (-(MousePos.y - m_ScreenViewSize.y * m_CurZoomMag * 0.5f)) * m_CurZoomMag;
		MousePos.vec2 = MousePos.vec2 + TRANS->GetWorldPos().vec2 ;
	}
	return MousePos;
	}

	return Vec3();
}

const float CCamera::GetViewMinDepth()
{
	return TRANS->GetWorldPos().z + m_fNear;
}

const SPTR<RenderTarget> CCamera::GetCamTarget(const unsigned int& _TargetIdx)
{
	if (nullptr == m_CameraViewTarget)
		return nullptr;

	return m_CameraViewTarget->GetRenderTarget(_TargetIdx);
}

void CCamera::SetCameraBluring(const float& _BlurTime, SPTR<CCamera> _BlurCamera, const float& _Pow /*= 2.f*/)
{
	if (this == _BlurCamera) 
	{
		return;
	}
	m_bBluring = true;
	m_fAccBlurTime = 0.f;
	m_fBlurTime = _BlurTime;
	m_BlurViewMat = _BlurCamera->GetViewMatrix();
	m_BlurViewPos = _BlurCamera->TRANS->GetWorldPos();
	m_fBlurPow = _Pow;
}
void CCamera::BluringCamera(const float& _DeltaTime)
{
	if (m_fAccBlurTime >= m_fBlurTime) 
	{
		m_bBluring = false;
		return;
	}

	float T = DXMath::Clamp(m_fAccBlurTime / m_fBlurTime, 0.f, 1.f);
	Vec3 CurU, CurV, CurN, CurT;
	Vec3 BlurU, BlurV, BlurN, BlurT;
	
	CurU = m_matView.GetColmVec(0);
	CurV = m_matView.GetColmVec(1);
	CurN = m_matView.GetColmVec(2);
	CurT = m_matView.Row[3];
	//CurT = TRANS->GetWorldPos();

	BlurU = m_BlurViewMat.GetColmVec(0);
	BlurV = m_BlurViewMat.GetColmVec(1);
	BlurN = m_BlurViewMat.GetColmVec(2);
	BlurT = m_BlurViewMat.Row[3];
	//BlurT = m_BlurViewPos;

	BlurU = DXMath::Vec3Slerp(BlurU, CurU, pow(T, m_fBlurPow));
	BlurV = DXMath::Vec3Slerp(BlurV, CurV, pow(T, m_fBlurPow));
	BlurN = DXMath::Vec3Slerp(BlurN, CurN, pow(T, m_fBlurPow));
	BlurT = DXMath::Vec3Lerp(BlurT, CurT, pow(T, m_fBlurPow));

	m_matView.SetIdentityMat();
	m_matView.Row[0] = Vec3(BlurU.x, BlurV.x, BlurN.x);
	m_matView.Row[1] = Vec3(BlurU.y, BlurV.y, BlurN.y);
	m_matView.Row[2] = Vec3(BlurU.z, BlurV.z, BlurN.z);
	//m_matView.Row[3] = Vec3(DXMath::Vector3Dot(-BlurT, BlurU), -DXMath::Vector3Dot(-BlurT, BlurV), -DXMath::Vector3Dot(-BlurT, BlurN));
	m_matView.Row[3].vec3 = BlurT;

	m_fAccBlurTime += _DeltaTime;
}
const bool CCamera::PushCameraScript(CBaseComponent* _CamScript)
{
	if (nullptr == _CamScript)
		return false;

	((CCameraScript*)_CamScript)->SetCamera(this);
	m_listCameraScript.push_back((CCameraScript*)_CamScript);
	((CCameraScript*)_CamScript)->MountingCameraEvent();
	return true;
}

void CCamera::SetCameraPivot(const Vec3& _Pivot)
{
	m_listEyePivot.push_back(_Pivot);
}

void CCamera::SetCameraZoom(const float& _ZoomMag)
{
	m_CurZoomMag = _ZoomMag;
}

void CCamera::ClearPivotList()
{
	m_listEyePivot.clear();
}

void CCamera::FilterRender()
{
	if (0 == m_FilterList.size())
	{
		return;
	}

	m_FilterStartIter = m_FilterList.begin();
	m_FilterEndIter = m_FilterList.end();

	m_LastTarget = m_CameraViewTarget;
	bool Check = false;

	for (; m_FilterStartIter != m_FilterEndIter; ++m_FilterStartIter)
	{
		if (false == (*m_FilterStartIter)->m_FilterOn)
		{
			continue;
		}
		Check = true;
		(*m_FilterStartIter)->Update(m_LastTarget);
		m_LastTarget = (*m_FilterStartIter)->m_OutFilterTarget;
	}

	if (false == Check)
	{
		return;
	}

	SetCamViewTarget();
	CDevice::GetContext()->RSSetViewports(1, &m_CamViewPort);
	m_LastTarget->GetRenderTarget()->GetRTTexture()->Update(0);
	m_CameraMaterial->Update();
	m_CameraMesh->Update();
	m_CameraMesh->Render();
	m_LastTarget->GetRenderTarget()->GetRTTexture()->PopShaderResource(0);
	m_CameraMaterial->TexSamRollBack();
	CDevice::ResetShaderResource();
}

void CCamera::ResetCamFilter()
{
	if (0 <= m_FilterList.size())
	{
		m_FilterList.clear();
	}
}

void CCamera::UIRender()
{
	ClearCamTarget();
	SetCamViewTarget();

	for (size_t i = 0; i < m_mapUIBase.size(); ++i)
	{                
		m_UImapiter = m_mapUIBase.find((UI_LAYER)i);
		if (m_UImapiter == m_mapUIBase.end()
			|| true == m_UImapiter->second.empty())
			continue;

		for (SPTR<CUIBase> UIBaseIter : m_UImapiter->second)
		{
			if (nullptr != UIBaseIter && true == UIBaseIter->IsActive())
				UIBaseIter->UIRender();
		}
	}
}

void CCamera::SetUIPosition()
{
	//저장된 UI포지션 가져와서 - 변환해주고 포스 바꿔주기
	for (size_t i = 0; i < m_mapUIBase.size(); ++i)
	{
		m_UImapiter = m_mapUIBase.find((UI_LAYER)i);
		if (m_UImapiter == m_mapUIBase.end()
			|| true == m_UImapiter->second.empty())
			continue;

		for (SPTR<CUIBase> UIBaseIter : m_UImapiter->second)
		{
			if (nullptr != UIBaseIter && true == UIBaseIter->IsActive())
				UIBaseIter->UIRender();
		}
	}

}

bool CCamera::PushUICom(CUIBase* _UICom, UI_LAYER _Layer)
{
	auto FindIter = m_mapUIBase.find(_Layer);

	if (m_mapUIBase.end() == FindIter)
	{
		auto Check = m_mapUIBase.insert(std::map<UI_LAYER, std::list<SPTR<CUIBase>>>::value_type(_Layer, std::list<SPTR<CUIBase>>()));

		if (true == Check.second)
			Check.first->second.push_back(_UICom);
		else
			return false;
	}
	else
		FindIter->second.push_back(_UICom);

	return true;
}