#include "FogFilter.h"
#include "Device.h"
#include "Mesh.h"
#include "Material.h"
#include "ResourceMgr.h"
#include "Camera.h"
#include "TransForm.h"

CFogFilter::CFogFilter() : m_FogBuffer(CBUFFER::FogBuffer()), m_FogMat(nullptr), m_FilterCam(nullptr)
{
}


CFogFilter::~CFogFilter()
{
}

void CFogFilter::Init(CCamera* _FilterCam)
{
	m_FogMat = CResourceMgr<CMaterial>::Find(L"Fog");
	m_FilterCam = _FilterCam;

 	m_FogBuffer = CBUFFER::FogBuffer();


	return CFilterEffect::Init();
}

void CFogFilter::Update(SPTR<MultiRenderTarget> _Target)
{
	m_BaseBuffer.CamWorldPos = m_FilterCam->GetTrans()->GetWorldPos();

	if (nullptr != CDevice::RenBaseCB)
		CDevice::RenBaseCB->Update(m_BaseBuffer);

	CDevice::SetDS(L"PostDepth");
	SetOutputTarget();

	m_FogMat->SetConstantBufferData(L"FogBuffer", m_FogBuffer);
	_Target->GetRenderTarget(0)->GetRTTexture()->Update(0);
	m_FogMat->Update();
	m_FilterMesh->Update();
	m_FilterMesh->Render();
	_Target->GetRenderTarget(0)->GetRTTexture()->PopShaderResource(0);
	m_FogMat->TexSamRollBack();
}