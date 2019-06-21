#pragma once
#include "Resource.h"
#include <unordered_map>
#include "DXEnum.h"
#include "ConstantBuffer.h"
#include <array>

typedef struct _tagSHADERCB
{
	SPTR<CConstantBuffer>				m_pCB;
	void*								m_pData;

	_tagSHADERCB() :m_pCB(nullptr), m_pData(nullptr) {}
	_tagSHADERCB(SPTR<CConstantBuffer> _pCB) :m_pCB(_pCB), m_pData(nullptr) {}
	_tagSHADERCB(SPTR<CConstantBuffer> _pCB, void* _pData) :m_pCB(_pCB), m_pData(_pData) {}
	~_tagSHADERCB()
	{
		SAFE_DELETE(m_pData);
	}

}ShaderCB;

typedef struct _tagTEXTUREDATA
{
	SPTR<class CTexture>			pTex;
	unsigned int					ShaderFlag;
}TextureData, TexData;

typedef struct _tagSAMPLERDATA
{
	SPTR<class CSamplerState>		pSampler;
	unsigned int					ShaderFlag;
}SamplerData;

class CMaterial : public CResource
{
	friend class CRenderer;
	friend class CCollider;
	friend class CLight;
	friend class CFbx;
	friend class CFBXMesh;

private:
	bool											m_bOriginal;
	bool											m_bDeferred;

public:
	void DeferredOn() { m_bDeferred = true; }
	void DeferredOff() { m_bDeferred = false; }
	const bool IsDeferred() { return m_bDeferred; }
	void SetDeferred(const bool _Value)
	{
		m_bDeferred = _Value;
	}

	// Shader Part
private:
	SPTR<class CVtxShader>							m_VS;
	SPTR<class CGeoShader>							m_GS;
	SPTR<class CPixShader>							m_PS;
	SPTR<class CFbx>								m_DrawFbx;

public:
	void Update();

public:
	void SetShader(const SHADER_TYPE& _Shader, const wchar_t* _ShaderKey);
	void SetShader(const unsigned int& _ShaderFlag, const wchar_t* _ShaderKey);

public:
	SPTR<CVtxShader> GetVtxShader();
	SPTR<CGeoShader> GetGeoShader();
	SPTR<CPixShader> GetPixShader();


public:
	void ShaderUpdate();

	//	Constant Buffer Part
private:
	std::unordered_map<std::wstring, ShaderCB>							m_mapShaderCB;
	std::unordered_map<std::wstring, ShaderCB>::iterator				m_CBStart;
	std::unordered_map<std::wstring, ShaderCB>::iterator				m_CBEnd;

public:
	//void CreateConstantBuffer(const std::wstring& _CBKey,const CConstantBuffer::CREATE_DESC& _Desc);
	void PushConstantBuffer(const std::wstring& _Key, const std::wstring& _CBKey);

	template<typename T>
	void SetConstantBufferData(const std::wstring& _Key, const T& _Data)
	{
		std::unordered_map<std::wstring, ShaderCB>::iterator Find = m_mapShaderCB.find(_Key);

		if (Find == m_mapShaderCB.end() || false == Find->second.m_pCB->CheckCBufferType<T>())
			return;

		UINT ByteWidth = Find->second.m_pCB->GetByteWidth();
		// 이미 값이 등록되어있는 경우
		if (nullptr == Find->second.m_pData)
			Find->second.m_pData = new char[ByteWidth];

		memcpy_s(Find->second.m_pData, ByteWidth, &_Data, ByteWidth);
	}


	void ConstantBufferUpdate();

	// Textue & Sampler Part
private:
	std::unordered_map<unsigned int, TexData>								m_mapTexture;
	std::unordered_map<unsigned int, SamplerData>							m_mapSampler;

public:
	void PushTexture(const unsigned int& _Slot, const wchar_t* _TexKey
		, unsigned int _ShaderFlag = SHADER_PIXEL
		, const MATERIAL_TEXTURETYPE& _TexType = LOAD_TEXTURE);

	void PushTargetTexture(const unsigned int& _Slot
		, SPTR<class RenderTarget> _Target
		, unsigned int _ShaderFlag = SHADER_PIXEL);

	void PushSampler(const unsigned int& _Slot, const wchar_t* _SamplerKey, unsigned int _ShaderFlag = SHADER_PIXEL);

	void TextureUpdate();
	void SamplerUpdate();

	// State Part
private:
	SPTR<class CBlendState>							m_Blend;

public:
	void SetBlend(const wchar_t* _BlendKey);
	void BlendUpdate();

private:
	std::wstring									m_RSKey;
	std::wstring									m_DSKey;
	SPTR<class CRasterizerState>					m_MaterialRS;
	SPTR<class CDepthStencilState>					m_MaterialDS;

public:
	void SetRasterizerState(const wchar_t* _RSKey);
	void SetDepthStancilState(const wchar_t* _DSKey);
	void RSUpdate();
	void DSUpdate();

public:
	void TexSamRollBack();
	void StateRollBack();

	//Viewport Part
private:
	bool											m_MatViewportExist;
	D3D11_VIEWPORT									m_MatViewport;

public:
	bool CreateViewport(unsigned int _Width = 0, unsigned int _Height = 0, unsigned int _TopLeftX = 0, unsigned int _TopLeftY = 0);
	void ViewportUpdate();

public:
	void SetEmivColor(int _Index, Vec4 _Color);

public:
	CMaterial* Clone();

public:
	CMaterial();
	CMaterial(const CMaterial& _Other);
	~CMaterial();


public:
	NONE_LOAD_RESOURCE(CMaterial)
		CREATE_DESC_BODY(int a;)

		const bool Create(const CREATE_DESC& _Desc);

};

