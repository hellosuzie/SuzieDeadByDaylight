#pragma once
#include "Resource.h"
#include "DXHeader.h"


class CRasterizerState : public CResource
{
	friend class Engine;

private:
	ID3D11RasterizerState * m_pRSState;
	D3D11_RASTERIZER_DESC	m_RSDesc;

public:
	void Update();

public:
	CRasterizerState();
	~CRasterizerState();

public:
	NONE_LOAD_RESOURCE(CRasterizerState)
	CREATE_DESC_BODY(
		D3D11_RASTERIZER_DESC ResterizerDesc;
	)

public:
	const bool Create(const CREATE_DESC& _Desc);

};

