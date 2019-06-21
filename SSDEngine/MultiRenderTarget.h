#pragma once
#include "RenderTarget.h"
#include <vector>

#define MAXRENDERTARGET 8


typedef class MultiRenderTarget : public CResource
{
private:
	std::vector<SPTR<RT>>								m_vecRenderTarget;
	std::vector<ID3D11RenderTargetView*>				m_vecRTView;
	std::vector<ID3D11ShaderResourceView*>				m_vecSRView;
	SPTR<CTexture>										m_DepthTex;

public:
	MultiRenderTarget();
	~MultiRenderTarget();
	
public:
	void Clear();
	void SetSingleRenderTarget(const int& _Idx ,bool _IsDepthNull /*= false*/);
	void SetMultiRenderTarget(bool _IsDepthNull = false);
	void SetMultiRenderTarget(ID3D11DepthStencilView* _pDepthView);
	const bool CreateDepth(UINT _Width = 0, UINT _Height = 0);
	void SetShaderResource(const UINT& _ShaderFlag, const UINT& StartSlot = 0);
	void SetShaderResource(const SHADER_TYPE& _Shader, const UINT& StartSlot = 0);
	void ClearShaderResource(const UINT& _ShaderFlag, const UINT& StartSlot = 0);
	void ClearShaderResource(const SHADER_TYPE& _Shader, const UINT& StartSlot = 0);
public:
	void PushRenderTarget(RenderTarget* _Ptr)
	{
		m_vecRenderTarget.push_back(_Ptr);
		m_vecRTView.push_back(_Ptr->GetRTTexture()->GetTexRTView());
	}
	SPTR<RenderTarget> GetRenderTarget(UINT _Index = 0);
	SPTR<CTexture> GetDepthTex();
	ID3D11DepthStencilView* GetDepthView();
public:
	NONE_LOAD_RESOURCE(MultiRenderTarget);
	CREATE_DESC_BODY(UINT RTCount;
	wchar_t* RTKey[MAXRENDERTARGET];)

	const bool Create(const CREATE_DESC& _Desc);

}MRT;

