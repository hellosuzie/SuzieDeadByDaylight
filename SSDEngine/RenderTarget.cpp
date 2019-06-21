#include "RenderTarget.h"
#include "Device.h"
#include "SSDThread.h"


RenderTarget::RenderTarget()
	:m_RTTex(nullptr), m_ClearColor(Vec4::Zero)
{
}


RenderTarget::~RenderTarget()
{
}


void RenderTarget::Update(const UINT& _TargetSlot)
{
	//DXCONTEXT->OMSetRenderTargets(1, ,);
}

const bool RenderTarget::Create(const CREATE_DESC& _Desc)
{
	m_RTTex = CTexture::CreateTexture(_Desc.Width, _Desc.Height, _Desc.Format, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	if (nullptr == m_RTTex) 
	{
		assert(nullptr);
		return false;
	}
	return true;
}

void RenderTarget::Clear()
{
	if (nullptr == m_RTTex && nullptr == m_RTTex->m_pTexRTView)
	{
		return;
	}

	CCriSectionObject<CDevice> Lock;
	CDevice::GetContext()->ClearRenderTargetView(m_RTTex->m_pTexRTView, m_ClearColor.pf);
}


void RenderTarget::SetTargetClearColor(const Vec4& _Color)
{
	m_ClearColor = _Color;
}