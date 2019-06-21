#include "RasterizerState.h"
#include "Device.h"



CRasterizerState::CRasterizerState()
	:m_pRSState(nullptr), m_RSDesc()
{
}


CRasterizerState::~CRasterizerState()
{
	SAFE_RELEASE(m_pRSState)

}
void CRasterizerState::Update()
{
	if (nullptr == m_pRSState)
		return;

	m_RSDesc;
	DXCONTEXT->RSSetState(m_pRSState);
}
const bool CRasterizerState::Create(const CREATE_DESC& _Desc)
{
	m_RSDesc = _Desc.ResterizerDesc;

	HRESULT Check = DXDEVICE->CreateRasterizerState(&_Desc.ResterizerDesc, &m_pRSState);
	if (S_OK != Check)
	{
		TASSERT(true);
		return false;
	}

	return true;
}