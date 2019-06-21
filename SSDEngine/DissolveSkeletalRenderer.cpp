#include "DissolveSkeletalRenderer.h"
#include "Fbx.h"
#include "Texture.h"
#include "Camera.h"
#include "Material.h"
#include "TimerMgr.h"
#include "DXMath.h"

CDissolveSkeletalRenderer::CDissolveSkeletalRenderer()
	:m_DissolveBuffer(CBUFFER::DissolveBuffer()), m_fAccTime(0.f)
	, m_DissolveTime(Vec2()), m_pDissolveUpdate(nullptr)
{
	m_DissolveBuffer.Active = 0;
}


CDissolveSkeletalRenderer::~CDissolveSkeletalRenderer()
{
}

void CDissolveSkeletalRenderer::Update(const float& _DeltaTime)
{
	CSkeletalFbxRenderer::Update(_DeltaTime);

	if (nullptr != m_pDissolveUpdate) 
	{
		(this->*m_pDissolveUpdate)(_DeltaTime);
		m_fAccTime += _DeltaTime;
	}
}

void CDissolveSkeletalRenderer::SetFbx(const wchar_t* _FbxKey)
{
	CFbxRenderer::SetFbx(_FbxKey);
	m_BoneTexture = m_RenderingFbx->m_BoneMatTex->GetCopyCreateTexture();
	m_CSBoneTexture = m_RenderingFbx->m_ComputeBoneMatTex->GetCopyCreateTexture();


	CreateDefaultAnimation();
	ChangeBoneAnimation(DEFANIM);

	m_vecCurAnimMat.resize(m_RenderingFbx->m_AllBoneVec.size());
	m_vecCurSkinningmMat.resize(m_RenderingFbx->m_AllBoneVec.size());
	m_vecAnimSocket.clear();
}

void CDissolveSkeletalRenderer::RenderUpdate(SPTR<CCamera> _Camera)
{
	CSkeletalFbxRenderer::RenderUpdate(_Camera);
}

void CDissolveSkeletalRenderer::DissolveUpdate(const float& _DeltaTime) 
{
	if (m_fAccTime >= m_DissolveTime.End) 
	{
		m_DissolveBuffer.Active = DISSOVEOFF;
		m_DissolveBuffer.Progess = 0.f;
		m_pDissolveUpdate = nullptr;
		CallDissolveEvent();
		Off();
		return;
	}

	float Progress = DXMath::Clamp((m_fAccTime - m_DissolveTime.Start) / (m_DissolveTime.End - m_DissolveTime.Start), 0.f, 1.f);

	if (Progress > 0.f && Progress < 1.f)
	{
		m_DissolveBuffer.Progess = Progress;
		DessolveBufferUpdate();
	}
}
void CDissolveSkeletalRenderer::ReverseDissolveUpdate(const float& _DeltaTime) 
{
	if (m_fAccTime >= m_DissolveTime.End)
	{
		m_DissolveBuffer.Active = DISSOVEOFF;
		m_DissolveBuffer.Progess = 0.f;
		m_pDissolveUpdate = nullptr;
		CallReverseDissolveEvent();
		return;
	}

	float Progress = DXMath::Clamp((m_fAccTime - m_DissolveTime.Start) / (m_DissolveTime.End - m_DissolveTime.Start), 0.f, 1.f);

	if (Progress > 0.f && Progress < 1.f)
	{
		m_DissolveBuffer.Progess = 1.f - Progress;
		DessolveBufferUpdate();
	}
}
void CDissolveSkeletalRenderer::DessolveBufferUpdate()
{
	UINT MaxCount = (UINT)m_vecMesh.size();

	for (UINT i = 0; i < MaxCount; ++i)
	{
		reinterpret_cast<CFBXMesh*>(m_vecMesh[i].PTR)->GetMatStart(0)->SetConstantBufferData(L"DissolveBuffer", m_DissolveBuffer);
	}
}
void CDissolveSkeletalRenderer::SetDissolve(const float& _Time) 
{
	m_DissolveBuffer.Active = DISSOVEACTIVE;
	m_DissolveBuffer.Progess = 0.f;
	m_DissolveTime.Start = 0.f;
	m_DissolveTime.End = _Time;

	m_fAccTime = 0.f;
	m_pDissolveUpdate = &CDissolveSkeletalRenderer::DissolveUpdate;
	DessolveBufferUpdate();
	On();	
}
void CDissolveSkeletalRenderer::SetDissolve(const float& _WaitTime, const float& _Time) 
{
	m_DissolveBuffer.Active = DISSOVEACTIVE;
	m_DissolveBuffer.Progess = 0.f;
	m_DissolveTime.Start = _WaitTime;
	m_DissolveTime.End = _WaitTime + _Time;

	m_fAccTime = 0.f;
	m_pDissolveUpdate = &CDissolveSkeletalRenderer::DissolveUpdate;
	DessolveBufferUpdate();
	On();
}
void CDissolveSkeletalRenderer::SetReverseDissolve(const float& _Time) 
{
	m_DissolveBuffer.Active = DISSOVEACTIVE;
	m_DissolveBuffer.Progess = 1.f;
	m_DissolveTime.Start = 0.f;
	m_DissolveTime.End = _Time;

	m_fAccTime = 0.f;
	m_pDissolveUpdate = &CDissolveSkeletalRenderer::ReverseDissolveUpdate;
	DessolveBufferUpdate();
	On();
}
void CDissolveSkeletalRenderer::SetReverseDissolve(const float& _WaitTime, const float& _Time) 
{
	m_DissolveBuffer.Active = DISSOVEACTIVE;
	m_DissolveBuffer.Progess = 1.f;
	m_DissolveTime.Start = _WaitTime;
	m_DissolveTime.End = _WaitTime + _Time;

	m_fAccTime = 0.f;
	m_pDissolveUpdate = &CDissolveSkeletalRenderer::ReverseDissolveUpdate;
	DessolveBufferUpdate();
	On();
}

void CDissolveSkeletalRenderer::CallDissolveEvent() 
{
	if (true == m_listDissolveCallBack.empty())
		return;

	std::list<std::function<void(void)>>::iterator Start = m_listDissolveCallBack.begin();
	std::list<std::function<void(void)>>::iterator End = m_listDissolveCallBack.end();
	
	for (;Start != End; ++Start)
	{
		(*Start)();
	}
}
void CDissolveSkeletalRenderer::CallReverseDissolveEvent() 
{
	if (true == m_listReDissolveCallBack.empty())
		return;

	std::list<std::function<void(void)>>::iterator Start = m_listReDissolveCallBack.begin();
	std::list<std::function<void(void)>>::iterator End = m_listReDissolveCallBack.end();

	for (; Start != End; ++Start)
	{
		(*Start)();
	}
}

void CDissolveSkeletalRenderer::PushFbxMesh(const int& _MeshIdx) 
{
	SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(_MeshIdx, L"DissolveSkeletal");

	if (nullptr == Mesh)
	{
#ifdef _DEBUG
		assert(nullptr);
		return;
#endif
	}
	else
	{
		Mesh->GetMatStart(0)->SetConstantBufferData(L"DissolveBuffer", m_DissolveBuffer);
		Mesh->GetMatStart(0)->PushTexture(NOISESLOT, L"Noise");
		Mesh->GetMatStart(0)->PushTexture(BURNSLOT, L"Burn");
		m_vecMesh.push_back(Mesh);
	}
}

void CDissolveSkeletalRenderer::SetAllFbxMesh()
{
	for (int Idx = 0; Idx < (int)m_RenderingFbx->m_AllFBXMeshVec.size(); ++Idx)
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(Idx, L"DissolveSkeletal");

		if (nullptr == Mesh)
		{
#ifdef _DEBUG
			assert(nullptr);
			return;
#endif
		}
		else
		{
			Mesh->GetMatStart(0)->SetConstantBufferData(L"DissolveBuffer", m_DissolveBuffer);
			Mesh->GetMatStart(0)->PushTexture(NOISESLOT, L"Noise");
			Mesh->GetMatStart(0)->PushTexture(BURNSLOT, L"Burn");
			m_vecMesh.push_back(Mesh);
		}
	}
}