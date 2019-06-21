#pragma once
#include"Resource.h"
#include"DXHeader.h"

class CDepthStencilState :public CResource
{
	friend class Engine;

private:
	ID3D11DepthStencilState *					m_pDSState;


public:
	void Update(const UINT& _StencilRef);

public:
	CDepthStencilState();
	~CDepthStencilState();

public:
	NONE_LOAD_RESOURCE(CDepthStencilState)
	CREATE_DESC_BODY(
		D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	)

public:
	const bool Create(const CREATE_DESC& _Desc);

};

