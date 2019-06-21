#pragma once
#include "FilterEffect.h"
#include "DXStruct.h"

class CFogFilter
	: public CFilterEffect
{
private:
	CBUFFER::FogBuffer			m_FogBuffer;
	CBUFFER::RenBaseBuffer		m_BaseBuffer;

	class CMaterial*			m_FogMat;
	class CCamera*				m_FilterCam;

public:
	void Init(CCamera* _FilterCam);
	void Update(SPTR<MultiRenderTarget> _Target);

public:
	void SetFogColor(Vec3 _FogColor)
	{
		m_FogBuffer.FogColor = _FogColor;
	}
	void SetFogStartDepth(float _FogStartDepth)
	{
		m_FogBuffer.FogStartDepth = _FogStartDepth;
	}
	void SetFogHighlightColor(Vec3 _FogHightlightColor)
	{
		m_FogBuffer.FogHighlightColor = _FogHightlightColor;
	}
	void SetFogGlobalDensity(float _FogGlobalDensity)
	{
		m_FogBuffer.FogGlobalDensity = _FogGlobalDensity;
	}
	void SetFogSunDir(Vec3 _FogSunDir)
	{
		m_FogBuffer.FogSunDir = _FogSunDir;
	}
	void SetFogHeightFallOff(float _FogHeightFallOff)
	{
		m_FogBuffer.FogHeightFallOff = _FogHeightFallOff;
	}

	Vec3 GetFogColor()
	{
		return m_FogBuffer.FogColor;
	}
	float GetFogStartDepth()
	{
		return m_FogBuffer.FogStartDepth;
	}
	Vec3 GetFogHightlightColor()
	{
		return m_FogBuffer.FogHighlightColor;
	}
	float GetFogGlobalDensity()
	{
		return m_FogBuffer.FogGlobalDensity;
	}
	Vec3 GetFogSunDir()
	{
		return m_FogBuffer.FogSunDir;
	}
	float GetFogHeightFallOff()
	{
		return m_FogBuffer.FogHeightFallOff;
	}
	const CBUFFER::FogBuffer& GetFogBuffer()
	{
		return m_FogBuffer;
	}

public:
	CFogFilter();
	~CFogFilter();
};

