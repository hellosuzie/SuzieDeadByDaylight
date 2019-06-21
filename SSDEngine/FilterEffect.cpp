#include "FilterEffect.h"
#include "SSDCore.h"
#include "Mesh.h"
#include "ResourceMgr.h"
#include "Material.h"
#include "MultiRenderTarget.h"
#include "RenderTarget.h"


SPTR<CMaterial>						CFilterEffect::Mini64Mat = nullptr;
SPTR<CMaterial>						CFilterEffect::Mini128Mat = nullptr;
SPTR<CMaterial>						CFilterEffect::Mini256Mat = nullptr;
SPTR<CMaterial>						CFilterEffect::Mini512Mat = nullptr;
SPTR<MultiRenderTarget>				CFilterEffect::MinimizeTarget = nullptr;

void CFilterEffect::MiniBufferInit()
{
	Mini64Mat = CResourceMgr<CMaterial>::Find(L"Minimize64");
	Mini128Mat = CResourceMgr<CMaterial>::Find(L"Minimize128");
	Mini256Mat = CResourceMgr<CMaterial>::Find(L"Minimize256");
	Mini512Mat = CResourceMgr<CMaterial>::Find(L"Minimize512");
	MinimizeTarget = CResourceMgr<MRT>::Find(L"Minimize"); 
}

//작게 만들 랜더타겟, 미니버퍼에 넣을 상수버퍼값, 
void CFilterEffect::MiniBufferUpdate(MINIMIZE_BUFFER_TYPE _Type, SPTR<MultiRenderTarget> _Target, CBUFFER::MinimizeBuffer&  _Buffer, const UINT _TargetSlotNum)
{
	CBUFFER::MinimizeBuffer Temp;
	Temp.MiniType = _Buffer.MiniType;
	Temp.Amount = _Buffer.Amount;
	Temp.Luminance = _Buffer.Luminance; 
	Temp.Multiple = _Buffer.Multiple;

	switch (_Type)
	{
	case MINI_64:
	{
		Mini64Mat->SetConstantBufferData(L"MinimizeBuffer", Temp);

		CDevice::SetDS(L"PostDepth");
		SPTR<MRT> MiniTarget = CResourceMgr<MRT>::Find(L"Minimize64");
		MiniTarget->Clear();
		MiniTarget->SetMultiRenderTarget(true);

		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->Update(0);
		Mini64Mat->Update();
		m_FilterMesh->Update();
		m_FilterMesh->Render();
		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->PopShaderResource(0);
		Mini64Mat->TexSamRollBack();
	}
		break;
	case MINI_128:
	{
		Mini128Mat->SetConstantBufferData(L"MinimizeBuffer", Temp);

		CDevice::SetDS(L"PostDepth");
		SPTR<MRT> MiniTarget = CResourceMgr<MRT>::Find(L"Minimize128");
		MiniTarget->Clear();
		MiniTarget->SetMultiRenderTarget(true);

		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->Update(0);
		Mini128Mat->Update();
		m_FilterMesh->Update();
		m_FilterMesh->Render();
		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->PopShaderResource(0);
		Mini128Mat->TexSamRollBack();
	}
		break;
	case MINI_256:
	{
		Mini256Mat->SetConstantBufferData(L"MinimizeBuffer", Temp);

		CDevice::SetDS(L"PostDepth");
		SPTR<MRT> MiniTarget = CResourceMgr<MRT>::Find(L"Minimize256");
		MiniTarget->Clear();
		MiniTarget->SetMultiRenderTarget(true);

		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->Update(0);
		Mini256Mat->Update();
		m_FilterMesh->Update();
		m_FilterMesh->Render();
		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->PopShaderResource(0);
		Mini256Mat->TexSamRollBack();
	}
		break;
	case MINI_512:
	{
		Mini512Mat->SetConstantBufferData(L"MinimizeBuffer", Temp);

		CDevice::SetDS(L"PostDepth");
		SPTR<MRT> MiniTarget = CResourceMgr<MRT>::Find(L"Minimize512");
		MiniTarget->Clear();
		MiniTarget->SetMultiRenderTarget(true);

		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->Update(0);
		Mini512Mat->Update();
		m_FilterMesh->Update();
		m_FilterMesh->Render();
		_Target->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->PopShaderResource(0);
		Mini512Mat->TexSamRollBack();
	}
		break;
	default:
		break;
	}
}

void CFilterEffect::AllMiniBufferUpdate(SPTR<MultiRenderTarget> _InputTarget, CBUFFER::MinimizeBuffer& _Buffer, const UINT _TargetSlotNum/* = 0*/)
{
	MinimizeTarget->Clear();
	Mini512Mat->SetConstantBufferData(L"MinimizeBuffer", _Buffer);
	Mini256Mat->SetConstantBufferData(L"MinimizeBuffer", _Buffer);
	Mini128Mat->SetConstantBufferData(L"MinimizeBuffer", _Buffer);
	Mini64Mat->SetConstantBufferData(L"MinimizeBuffer", _Buffer);

	CDevice::SetDS(L"PostDepth");

	// 1. 원본 -> 512x512 축소
	MinimizeTarget->SetSingleRenderTarget(0, true);
	_InputTarget->GetRenderTarget(_TargetSlotNum)->GetRTTexture()->Update(0);
	Mini512Mat->Update();
	m_FilterMesh->Update();
	m_FilterMesh->Render();
	Mini512Mat->TexSamRollBack();


	// 2. 512 x 512 -> 256 x 256 축소
	MinimizeTarget->SetSingleRenderTarget(1, true);
	MinimizeTarget->GetRenderTarget(0)->GetRTTexture()->Update(0);
	Mini256Mat->Update();
	m_FilterMesh->Update();
	m_FilterMesh->Render();
	Mini256Mat->TexSamRollBack();

	// 3. 256 x 256 -> 128 x 128 축소
	MinimizeTarget->SetSingleRenderTarget(2, true);
	MinimizeTarget->GetRenderTarget(1)->GetRTTexture()->Update(0);
	Mini128Mat->Update();
	m_FilterMesh->Update();
	m_FilterMesh->Render();
	Mini128Mat->TexSamRollBack();

	// 4. 128 x 128 -> 64 x 64 축소
	MinimizeTarget->SetSingleRenderTarget(3, true);
	MinimizeTarget->GetRenderTarget(2)->GetRTTexture()->Update(0);
	Mini64Mat->Update();
	m_FilterMesh->Update();
	m_FilterMesh->Render();
	MinimizeTarget->GetRenderTarget(2)->GetRTTexture()->PopShaderResource(0);
	Mini64Mat->TexSamRollBack();

}

CFilterEffect::CFilterEffect() 
	: m_FilterOn(true), m_FilterMesh(nullptr), m_ShaderResFilterTarget(nullptr), m_OutFilterTarget(nullptr), m_ParentComponent(nullptr)
{

}

CFilterEffect::~CFilterEffect()
{
}

void CFilterEffect::Init()
{
	RT::CREATE_DESC RTDesc;
	RTDesc.Width = Core::GetWindowWidth();
	RTDesc.Height = Core::GetWindowHeight();
	RTDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	SPTR<RenderTarget> FilterOutTarget = new RenderTarget();
	FilterOutTarget->Create(RTDesc);

	m_OutFilterTarget = new MultiRenderTarget();
	m_OutFilterTarget->PushRenderTarget(FilterOutTarget);

	m_FilterMesh = CResourceMgr<CMesh>::Find(L"TargetViewMesh");
}

void CFilterEffect::Update(SPTR<MultiRenderTarget> _Target)
{

}

void CFilterEffect::SetOutputTarget()
{
	if (nullptr == m_OutFilterTarget)
	{
		TASSERT(true);
	}
	m_OutFilterTarget->Clear();
	m_OutFilterTarget->SetMultiRenderTarget();
}
