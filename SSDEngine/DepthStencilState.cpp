#include "DepthStencilState.h"
#include"Device.h"



CDepthStencilState::CDepthStencilState()
	:m_pDSState(nullptr)
{
}


CDepthStencilState::~CDepthStencilState()
{
	SAFE_RELEASE(m_pDSState);
}

void CDepthStencilState::Update(const UINT& _StencilRef)
{
	if (nullptr == m_pDSState)
		return;

	DXCONTEXT->OMSetDepthStencilState(m_pDSState,_StencilRef);
}

const bool CDepthStencilState::Create(const CREATE_DESC& _Desc)
{
	HRESULT Check = DXDEVICE->CreateDepthStencilState(&_Desc.DepthStencilDesc, &m_pDSState);

	if (S_OK != Check)
	{
		TASSERT(true);
		return false;
	}

	return true;
}