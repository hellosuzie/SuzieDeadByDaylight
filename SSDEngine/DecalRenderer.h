#pragma once
#include "Base3DRenderer.h"
#include "DXStruct.h"

class CDecalRenderer
	: public CBase3DRenderer
{
private:
	SPTR<class CMesh>				m_DecalMesh;
	SPTR<class CMaterial>			m_DecalMaterial;

	CBUFFER::DecalBuffer			m_Buffer;

private:
	std::wstring					m_TexName;
	std::wstring					m_NormalName;

public:
	const bool Init(const RENDATA& _InitData) override;
	void PostUpdate(const float& _DeltaTime) override;

	void RenderUpdate(SPTR<CCamera> _Cam) override;
	void Render(SPTR<class CCamera> _Cam) override;

public:
	void SetDecalTex(const wchar_t* _Name)
	{
		m_TexName = _Name;
	}
	void SetDecalNormalTex(const wchar_t* _Name)
	{
		m_NormalName = _Name;
	}

public:
	CDecalRenderer();
	~CDecalRenderer();
};

