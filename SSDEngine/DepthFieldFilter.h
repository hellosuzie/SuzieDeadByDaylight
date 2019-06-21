#pragma once
#include "FilterEffect.h"
class CDepthFieldFilter : public CFilterEffect
{
private:
	CBUFFER::MinimizeBuffer									m_DoFMiniBuffer;
	SPTR<class RenderTarget>								m_DepthTarget;
	SPTR<class CMaterial>									m_DoFMaterial;
public:
	void Init() override;
	void Update(SPTR<MultiRenderTarget> _Target) override;

public:
	CDepthFieldFilter();
	~CDepthFieldFilter();
};

