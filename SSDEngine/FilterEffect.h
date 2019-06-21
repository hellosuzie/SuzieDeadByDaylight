#pragma once
#include "GameObject.h"
#include "MultiRenderTarget.h"
#include "DXStruct.h"

class CFilterEffect :
	public CGameObject
{
public:
	friend class CCamera;

private:
	bool									m_FilterOn;
	class CBaseComponent*					m_ParentComponent;

protected:
	SPTR<class CMesh>						m_FilterMesh;

protected:
	//½¦ÀÌ´õ ¸®¼Ò½º·Î »ç¿ëÇÏ´Â Å¸°Ù - º¸Åë Ä·Å¸°Ù
	SPTR<MultiRenderTarget>					m_ShaderResFilterTarget;
	SPTR<MultiRenderTarget>					m_OutFilterTarget;

public:
	CFilterEffect();
	~CFilterEffect();

public:
	virtual void Init();
	virtual void Update(SPTR<MultiRenderTarget> _Target) = 0;
	void SetOutputTarget();
	
public:
	void FilterOff()
	{
		m_FilterOn = false;
	}
	void FilterOn()
	{
		m_FilterOn = true;
	}
	bool IsFilterOn()
	{
		return m_FilterOn;
	}
	void SetFilterBaseComponent(CBaseComponent* _Component)
	{
		m_ParentComponent = _Component;
	}
	CBaseComponent* GetComponent()
	{
		return m_ParentComponent;
	}


public:
	static SPTR<class CMaterial>			Mini64Mat;
	static SPTR<class CMaterial>			Mini128Mat;
	static SPTR<class CMaterial>			Mini256Mat;
	static SPTR<class CMaterial>			Mini512Mat;
	static SPTR<class MultiRenderTarget>	MinimizeTarget;
	static void MiniBufferInit();

protected:
	void MiniBufferUpdate(MINIMIZE_BUFFER_TYPE _Type, SPTR<MultiRenderTarget> _Target, CBUFFER::MinimizeBuffer&  _Buffer, const UINT _TargetSlotNum = 0);
	void AllMiniBufferUpdate(SPTR<MultiRenderTarget> _InputTarget, CBUFFER::MinimizeBuffer&  _Buffer, const UINT _TargetSlotNum = 0);
};

