#include "MultiRenderTarget.h"
#include "Device.h"
#include "DXMath.h"
#include "ResourceMgr.h"
#include "SSDCore.h"


MultiRenderTarget::MultiRenderTarget()
{
}


MultiRenderTarget::~MultiRenderTarget()
{
}


const bool MultiRenderTarget::Create(const CREATE_DESC& _Desc)
{
	UINT Count = DXMath::Clamp(_Desc.RTCount, (UINT)0, (UINT)MAXRENDERTARGET);

	for (UINT i = 0; i <  Count; i++)
	{
		if (nullptr == _Desc.RTKey[i])
			continue;

		SPTR<RT> RenTarget = CResourceMgr<RT>::Find(_Desc.RTKey[i]);

		if (nullptr == RenTarget)
			assert(nullptr);
		else 
			m_vecRenderTarget.push_back(RenTarget);
	}

	for (size_t i = 0; i < m_vecRenderTarget.size(); i++)
	{
		if (nullptr == m_vecRenderTarget[i]
			|| nullptr == m_vecRenderTarget[i]->m_RTTex
			|| nullptr == m_vecRenderTarget[i]->m_RTTex->m_pTexRTView)
		{
			assert(nullptr);
			continue;
		}

		m_vecRTView.push_back(m_vecRenderTarget[i]->m_RTTex->m_pTexRTView);

		if (nullptr == m_vecRenderTarget[i]
			|| nullptr == m_vecRenderTarget[i]->m_RTTex->m_pTexSRView)
		{
			assert(nullptr);
			continue;
		}

		m_vecSRView.push_back(m_vecRenderTarget[i]->m_RTTex->m_pTexSRView);
	}
	return true;
}

const bool MultiRenderTarget::CreateDepth(UINT _Width, UINT _Height)
{
	if (0 == _Width)
	{
		_Width = SSDCore::GetWindowWidth();
	}
	if (0 == _Height)
	{
		_Height = SSDCore::GetWindowHeight();
	}

	m_DepthTex = CTexture::CreateTexture(_Width, _Height, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);
	if (nullptr == m_DepthTex)
	{
		return false;
	}
	return true;
}

void MultiRenderTarget::SetShaderResource(const UINT& _ShaderFlag, const UINT& StartSlot/* = 0*/)
{
	if (0 != (_ShaderFlag & SHADER_VERTEX))
		DXCONTEXT->VSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);

	if (0 != (_ShaderFlag & SHADER_HULL))
		DXCONTEXT->HSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);

	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		DXCONTEXT->DSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);

	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		DXCONTEXT->GSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);

	if (0 != (_ShaderFlag & SHADER_PIXEL))
		DXCONTEXT->PSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
}
void MultiRenderTarget::SetShaderResource(const SHADER_TYPE& _Shader, const UINT& StartSlot /*= 0*/)
{
	switch (_Shader)
	{
	case SHADER_ALL: 
	{
		DXCONTEXT->VSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		DXCONTEXT->HSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		DXCONTEXT->DSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		DXCONTEXT->GSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		DXCONTEXT->PSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
	}
		break;
	case SHADER_VERTEX:
		DXCONTEXT->VSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		break;
	case SHADER_HULL:
		DXCONTEXT->HSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		break;
	case SHADER_DOMAIN:
		DXCONTEXT->DSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		break;
	case SHADER_GEOMETRY:
		DXCONTEXT->GSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		break;
	case SHADER_PIXEL:
		DXCONTEXT->PSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &m_vecSRView[0]);
		break;
	default:
		break;
	}

}
void MultiRenderTarget::ClearShaderResource(const UINT& _ShaderFlag, const UINT& StartSlot /*= 0*/)
{
	std::vector<ID3D11ShaderResourceView*> Empty;
	Empty.assign(m_vecSRView.size(), nullptr);

	if (0 != (_ShaderFlag & SHADER_VERTEX))
		DXCONTEXT->VSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);

	if (0 != (_ShaderFlag & SHADER_HULL))
		DXCONTEXT->HSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);

	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		DXCONTEXT->DSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);

	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		DXCONTEXT->GSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);

	if (0 != (_ShaderFlag & SHADER_PIXEL))
		DXCONTEXT->PSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
}

void MultiRenderTarget::ClearShaderResource(const SHADER_TYPE& _Shader, const UINT& StartSlot /*= 0*/)
{
	std::vector<ID3D11ShaderResourceView*> Empty;
	Empty.assign(m_vecSRView.size(), nullptr);

	switch (_Shader)
	{
	case SHADER_ALL:
	{
		DXCONTEXT->VSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		DXCONTEXT->HSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		DXCONTEXT->DSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		DXCONTEXT->GSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		DXCONTEXT->PSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
	}
	break;
	case SHADER_VERTEX:
		DXCONTEXT->VSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		break;
	case SHADER_HULL:
		DXCONTEXT->HSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		break;
	case SHADER_DOMAIN:
		DXCONTEXT->DSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		break;
	case SHADER_GEOMETRY:
		DXCONTEXT->GSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		break;
	case SHADER_PIXEL:
		DXCONTEXT->PSSetShaderResources(StartSlot, (UINT)m_vecRTView.size(), &Empty[0]);
		break;
	default:
		break;
	}
}
SPTR<RenderTarget> MultiRenderTarget::GetRenderTarget(UINT _Index)
{
	if (_Index >= m_vecRenderTarget.size())
	{
		return nullptr;
	}
	return m_vecRenderTarget[_Index];
}
SPTR<CTexture> MultiRenderTarget::GetDepthTex()
{
	return m_DepthTex;
}

ID3D11DepthStencilView* MultiRenderTarget::GetDepthView()
{
	if (nullptr == m_DepthTex)
		return nullptr;

	return m_DepthTex->GetDepthStancilView();
}
void MultiRenderTarget::Clear()
{
	for (size_t i = 0; i < m_vecRenderTarget.size(); i++)
	{
		m_vecRenderTarget[i]->Clear();
	}

	if(nullptr != m_DepthTex)
		DXCONTEXT->ClearDepthStencilView(m_DepthTex->m_pTexDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
void MultiRenderTarget::SetSingleRenderTarget(const int& _Idx, bool _IsDepthNull /*= false*/)
{
	ID3D11DepthStencilView* pDepthView = nullptr;


	if (_Idx >= m_vecRenderTarget.size()) 
	{
		TASSERT(true);
		return;
	}

	if (nullptr != m_DepthTex)
	{
		pDepthView = m_DepthTex->m_pTexDSView;
		CDevice::GetContext()->OMSetRenderTargets(1, &m_vecRTView[_Idx], pDepthView);
	}
	else
	{
		if (true == _IsDepthNull)
		{
			CDevice::GetContext()->OMSetRenderTargets(1, &m_vecRTView[_Idx], nullptr);
			return;
		}
		CDevice::GetContext()->OMGetRenderTargets(0, nullptr, &pDepthView);
		CDevice::GetContext()->OMSetRenderTargets(1, &m_vecRTView[_Idx], pDepthView);
		if (nullptr != pDepthView)
		{
			pDepthView->Release();
		}
	}
}

void MultiRenderTarget::SetMultiRenderTarget(bool _IsDepthNull)
{
	ID3D11DepthStencilView* pDepthView = nullptr;

	if (nullptr != m_DepthTex)
	{
		pDepthView = m_DepthTex->m_pTexDSView;
		CDevice::GetContext()->OMSetRenderTargets((UINT)m_vecRenderTarget.size(), &m_vecRTView[0], pDepthView);
	}
	else
	{
 		if (true == _IsDepthNull)
		{
			CDevice::GetContext()->OMSetRenderTargets((UINT)m_vecRenderTarget.size(), &m_vecRTView[0], nullptr);
			return;
		}
		CDevice::GetContext()->OMGetRenderTargets(0, nullptr, &pDepthView);
		CDevice::GetContext()->OMSetRenderTargets((UINT)m_vecRenderTarget.size(), &m_vecRTView[0], pDepthView);
		if (nullptr != pDepthView)
		{
			pDepthView->Release();
		}
	}
}

void MultiRenderTarget::SetMultiRenderTarget(ID3D11DepthStencilView* _pDepthView)
{
	/*if (nullptr != _pDepthView)
	{*/
		CDevice::GetContext()->OMSetRenderTargets((UINT)m_vecRenderTarget.size(), &m_vecRTView[0], _pDepthView);
	//}
	//else 
	//{
	//	CDevice::GetContext()->OMSetRenderTargets((UINT)m_vecRenderTarget.size(), &m_vecRTView[0], );
	//	ID3D11DepthStencilView* pDepthView;

	//	CDevice::GetContext()->OMGetRenderTargets(0, nullptr, &pDepthView);
	//	CDevice::GetContext()->OMSetRenderTargets((UINT)m_vecRenderTarget.size(), &m_vecRTView[0], pDepthView);
	//	pDepthView->Release();
	//}
}