#pragma once
#include "Resource.h"
#include "DXHeader.h"
#include "DXEnum.h"
class CSamplerState :	public CResource
{
	friend class Engine;


private:
	ID3D11SamplerState *						m_pSamplerState;

private:
	CSamplerState();
	~CSamplerState();

public:
	void Update(const int& _SlotNum, const UINT& _ShaderFlag = SHADER_VERTEX | SHADER_PIXEL);
	void PopSampler(const int& _SlotNum, const UINT& _ShaderFlag = SHADER_VERTEX | SHADER_PIXEL);

public:
	ID3D11SamplerState** GetSamplerSate() 
	{
		return &m_pSamplerState;
	}

public:
	NONE_LOAD_RESOURCE(CSamplerState)
	CREATE_DESC_BODY(
		D3D11_SAMPLER_DESC SampleDesc;
	)

public:
	const bool Create(const CREATE_DESC& _Desc);
};

