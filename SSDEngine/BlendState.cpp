#include "BlendState.h"
#include"Device.h"


CBlendState::CBlendState()
	:m_BlendFactor(Vec4(0.f,0.f,0.f,1.f))
{
}


CBlendState::~CBlendState()
{
	SAFE_RELEASE(m_pBlendState)
}

void CBlendState::Update()
{
	DXCONTEXT->OMSetBlendState(m_pBlendState, m_BlendFactor.pf, (UINT)0xffffffff);
}

const bool CBlendState::Create(const CREATE_DESC& _Desc)
{
	HRESULT Check = DXDEVICE->CreateBlendState(&_Desc.BlendDesc, &m_pBlendState);

	if (S_OK != Check) 
	{
		TASSERT(true);
		return false;
	}

	return true;
}