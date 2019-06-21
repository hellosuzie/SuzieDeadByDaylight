#include "SamplerState.h"
#include"Device.h"
#include "SSDThread.h"

CSamplerState::CSamplerState()
	:m_pSamplerState(nullptr)
{
}


CSamplerState::~CSamplerState()
{
	SAFE_RELEASE(m_pSamplerState)
}


void CSamplerState::Update(const int& _SlotNum, const UINT& _ShaderFlag/* = SHADER_VERTEX | SHADER_PIXEL*/)
{
	CCriSectionObject<CDevice> Lock;
	if (0 != (_ShaderFlag & SHADER_VERTEX))
		DXCONTEXT->VSSetSamplers(_SlotNum, 1, &m_pSamplerState);

	if (0 != (_ShaderFlag & SHADER_HULL))
		DXCONTEXT->HSSetSamplers(_SlotNum, 1, &m_pSamplerState);

	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		DXCONTEXT->DSSetSamplers(_SlotNum, 1, &m_pSamplerState);

	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		DXCONTEXT->GSSetSamplers(_SlotNum, 1, &m_pSamplerState);

	if (0 != (_ShaderFlag & SHADER_PIXEL))
		DXCONTEXT->PSSetSamplers(_SlotNum, 1, &m_pSamplerState);
}

void CSamplerState::PopSampler(const int& _SlotNum, const UINT& _ShaderFlag /*= SHADER_VERTEX | SHADER_PIXEL*/)
{
	CCriSectionObject<CDevice> Lock;
	ID3D11SamplerState* Empty = nullptr;

	if (0 != (_ShaderFlag & SHADER_VERTEX))
		DXCONTEXT->VSSetSamplers(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_HULL))
		DXCONTEXT->HSSetSamplers(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		DXCONTEXT->DSSetSamplers(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		DXCONTEXT->GSSetSamplers(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_PIXEL))
		DXCONTEXT->PSSetSamplers(_SlotNum, 1, &Empty);
}
const bool CSamplerState::Create(const CREATE_DESC& _Desc)
{
	HRESULT Check;

	Check = DXDEVICE->CreateSamplerState(&_Desc.SampleDesc, &m_pSamplerState);

	if (S_OK != Check)
		return false;

	return true;
}

