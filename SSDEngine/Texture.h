#pragma once
#include "Resource.h"
#include "DXHeader.h"
#include "DXEnum.h"

#define MAXRENDERTARGET 8

#define VS_SET 0x00000001
#define HS_SET 0x00000002
#define DS_SET 0x00000004
#define GS_SET 0x00000008
#define PS_SET 0x00000010

class CTexture : public CResource
{
private:
	friend class RenderTarget;
	friend class MultiRenderTarget;
	friend class CFbx;

private:
	static SPTR<CTexture> CreateTexture(const UINT& _Width, const UINT& _Height, DXGI_FORMAT _eFormat, UINT _BindFlag, D3D11_USAGE _eUsage = D3D11_USAGE::D3D11_USAGE_DEFAULT);
	static SPTR<CTexture> CreateTexture(void* _pData ,const UINT& _Width, const UINT& _Height, DXGI_FORMAT _eFormat, UINT _BindFlag, D3D11_USAGE _eUsage = D3D11_USAGE::D3D11_USAGE_DEFAULT);

protected:
	DirectX::ScratchImage						m_Image;
	DirectX::TexMetadata						m_MetaData;
	ID3D11Resource*								m_pTexture;
	D3D11_TEXTURE2D_DESC						m_TexDesc;

protected:
	ID3D11RenderTargetView*						m_pTexRTView;
	ID3D11DepthStencilView*						m_pTexDSView;
	ID3D11ShaderResourceView*					m_pTexSRView;
	// 순서없는 접근뷰
	ID3D11UnorderedAccessView*					m_pTexUOAView;

public:
	SPTR<CTexture> GetCopyCreateTexture();

public:
	CTexture();
	~CTexture();

public:
	ID3D11RenderTargetView* GetTexRTView()
	{
		return m_pTexRTView;
	}
	ID3D11Resource* GetTextureResource()
	{
		return m_pTexture;
	}
	DirectX::ScratchImage* GetScratchImage()
	{
		return &m_Image;
	}

private:
	const BOOL LoadDDSFile();
	const BOOL LoadHDRFile();
	const BOOL LoadTGAFile();
	const BOOL LoadWICFile();


public:
	ID3D11Resource* GetResource() const { return m_pTexture; }
	ID3D11DepthStencilView* GetDepthStancilView() const { return m_pTexDSView; }
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pTexSRView; }
	const Vec2 GetTextureSize();

public:
	void Update(const UINT& _SlotNum, const UINT& _ShaderFlag = SHADER_VERTEX | SHADER_PIXEL);
	void PopShaderResource(const UINT& _SlotNum, const UINT& _ShaderFlag = SHADER_VERTEX | SHADER_PIXEL);

public:
	void SetPixel(void* _Data, UINT _ByteSize);
	Vec4 GetPixel(const int _X, const int _Y);

public:
	void CreateTextueView();


public:
	NONE_CREATE_RESOURCE(CTexture)
	LOAD_DESC_BODY(int a;)


public:
	const bool Load(const LOAD_DESC& _Desc);

};

class CMultiTexture
	: public CTexture
{
private:
	std::vector<SPTR<CTexture>>					m_vecTex;
	std::vector<ID3D11Resource*>				m_vecResource;

public:
	CMultiTexture();
	~CMultiTexture();

public:
	void CreateMultiTextureView(UINT _BindFlag);

public:
	const Vec2 GetMultiTextureSize();

public:
	NONE_LOAD_RESOURCE(CMultiTexture);
	CREATE_DESC_BODY(
		D3D11_USAGE Usage;
		UINT RTCount;
		wchar_t* RTKey[MAXRENDERTARGET];)

	const bool Create(const CREATE_DESC& _Desc);

};