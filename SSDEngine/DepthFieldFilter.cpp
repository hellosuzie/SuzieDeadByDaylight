#include "DepthFieldFilter.h"
#include "Device.h"
#include "Mesh.h"
#include "Material.h"
#include "ResourceMgr.h"

CDepthFieldFilter::CDepthFieldFilter()
	:m_DoFMiniBuffer(CBUFFER::MinimizeBuffer())
{
	m_DoFMiniBuffer.MiniType = MINIMIZE_BASE;

}


CDepthFieldFilter::~CDepthFieldFilter()
{
}
void CDepthFieldFilter::Init()
{
	m_DoFMaterial = CResourceMgr<CMaterial>::Find(L"DepthOfField");
	m_DepthTarget = CResourceMgr<RT>::Find(L"Depth");
	return CFilterEffect::Init();

}
void CDepthFieldFilter::Update(SPTR<MultiRenderTarget> _Target)
{
	AllMiniBufferUpdate(_Target, m_DoFMiniBuffer);

	CDevice::ViewPortUpdate();

	//CDevice::SetDS(L"PostDepth");
	SetOutputTarget();

	_Target->GetRenderTarget(0)->GetRTTexture()->Update(0);
	m_DepthTarget->GetRTTexture()->Update(1);
	CFilterEffect::MinimizeTarget->SetShaderResource(SHADER_PIXEL, 2);
	m_FilterMesh->Update();
	m_DoFMaterial->Update();
	m_FilterMesh->Render();
	_Target->GetRenderTarget(0)->GetRTTexture()->PopShaderResource(0);
	m_DepthTarget->GetRTTexture()->PopShaderResource(1);
	CFilterEffect::MinimizeTarget->ClearShaderResource(SHADER_PIXEL, 2);
	m_DoFMaterial->TexSamRollBack();

}