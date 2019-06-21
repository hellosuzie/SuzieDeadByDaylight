#pragma once
#include "Resource.h"
#include"DXHeader.h"

class CBlendState : public CResource
{
	friend class Engine;
	 
private:
	ID3D11BlendState *							m_pBlendState;
	Vec4										m_BlendFactor;


public:
	void SetBlendFactor(const Vec4& _Factor) { m_BlendFactor = _Factor; }
private:
	CBlendState();
	~CBlendState();


public:
	void Update();

public:
	NONE_LOAD_RESOURCE(CBlendState)
	CREATE_DESC_BODY(
		D3D11_BLEND_DESC BlendDesc;
	)

public:
	const bool Create(const CREATE_DESC& _Desc);
};

