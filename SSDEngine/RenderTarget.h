#pragma once
#include "Resource.h"
#include "Texture.h"

typedef class RenderTarget : public CResource
{
private:
	friend class MultiRenderTarget;

private:
	SPTR<CTexture>					m_RTTex;
	FColor							m_ClearColor;

public:
	RenderTarget();
	~RenderTarget();


public:
	SPTR<CTexture> GetRTTexture() { return m_RTTex; }

public:
	void Update(const UINT& _TargetSlot);
	void Clear();

public:
	void SetTargetClearColor(const Vec4& _Color);

public:
	NONE_LOAD_RESOURCE(RenderTarget);
	CREATE_DESC_BODY( UINT Width;
		UINT Height;
		DXGI_FORMAT Format;)

	const bool Create(const CREATE_DESC& _Desc);

}RT;

