#include "Texture.h"
#include <DirectXTex.h>
#include "Device.h"
#include "Shader.h"
#include "DXMath.h"
#include "ResourceMgr.h"
#include "Shader.h"
#include "SSDThread.h"

SPTR<CTexture> CTexture::CreateTexture(const UINT& _Width, const UINT& _Height, DXGI_FORMAT _eFormat, UINT _BindFlag, D3D11_USAGE _eUsage /*= D3D11_USAGE::D3D11_USAGE_DEFAULT*/)
{
	CCriSectionObject<CDevice> Lock;
	SPTR<CTexture> NewTex = new CTexture();

	if (nullptr == NewTex)
		return nullptr;

	NewTex->m_TexDesc = CD3D11_TEXTURE2D_DESC(_eFormat, _Width, _Height);

	NewTex->m_TexDesc.BindFlags = _BindFlag;
	NewTex->m_TexDesc.Usage = _eUsage;
	NewTex->m_TexDesc.MipLevels = 1;

	if (NewTex->m_TexDesc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
		NewTex->m_TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if(NewTex->m_TexDesc.Usage == D3D11_USAGE::D3D11_USAGE_STAGING)
		NewTex->m_TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	if (S_OK != DXDEVICE->CreateTexture2D(&NewTex->m_TexDesc, nullptr, (ID3D11Texture2D**)&NewTex->m_pTexture))
	{
		assert(nullptr);
		return nullptr;
	}

	NewTex->CreateTextueView();
	return NewTex;
}


SPTR<CTexture> CTexture::CreateTexture(void* _pData, const UINT& _Width, const UINT& _Height, DXGI_FORMAT _eFormat, UINT _BindFlag, D3D11_USAGE _eUsage /*= D3D11_USAGE::D3D11_USAGE_DEFAULT*/)
{
	if (nullptr == _pData)
		return CreateTexture(_Width, _Height, _eFormat, _BindFlag, _eUsage);

	SPTR<CTexture> NewTex = new CTexture();

	if (nullptr == NewTex)
		return nullptr;

	NewTex->m_TexDesc = CD3D11_TEXTURE2D_DESC(_eFormat, _Width, _Height);

	NewTex->m_TexDesc.BindFlags = _BindFlag;
	NewTex->m_TexDesc.Usage = _eUsage;
	NewTex->m_TexDesc.MipLevels = 1;

	if (NewTex->m_TexDesc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
		NewTex->m_TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if (NewTex->m_TexDesc.Usage == D3D11_USAGE::D3D11_USAGE_STAGING)
		NewTex->m_TexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	D3D11_SUBRESOURCE_DATA Sub = D3D11_SUBRESOURCE_DATA();
	Sub.pSysMem = _pData;
	Sub.SysMemPitch = CShader::SizeofFmt(NewTex->m_TexDesc.Format) * _Width;
	Sub.SysMemSlicePitch = CShader::SizeofFmt(NewTex->m_TexDesc.Format) * _Width*_Height;

	HRESULT Check = DXDEVICE->CreateTexture2D(&NewTex->m_TexDesc, &Sub, (ID3D11Texture2D**)&NewTex->m_pTexture);
	
	if (S_OK != Check)
	{
		assert(nullptr);
		return nullptr;
	}

	NewTex->CreateTextueView();
	return NewTex;

}

CTexture::CTexture()
	:m_MetaData{}, m_Image{}, m_pTexture(nullptr), m_pTexSRView(nullptr)
	, m_pTexRTView(nullptr), m_pTexDSView(nullptr), m_pTexUOAView(nullptr)
{
}


CTexture::~CTexture()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pTexRTView);
	SAFE_RELEASE(m_pTexDSView);
	SAFE_RELEASE(m_pTexSRView);
	SAFE_RELEASE(m_pTexUOAView);

}
const BOOL CTexture::LoadDDSFile()
{
	if (S_OK != DirectX::LoadFromDDSFile(GetPath().c_str(), DirectX::DDS_FLAGS_NONE, &m_MetaData, m_Image))
	{
		TASSERT(TRUE);
		return FALSE;
	}
	return TRUE;
}
const BOOL CTexture::LoadHDRFile()
{
	if (S_OK != DirectX::LoadFromHDRFile(GetPath().c_str(), &m_MetaData, m_Image))
	{
		TASSERT(TRUE);
		return FALSE;
	}
	return TRUE;
}
const BOOL CTexture::LoadTGAFile()
{
	if (S_OK != DirectX::LoadFromTGAFile(GetPath().c_str(), &m_MetaData, m_Image))
	{
		//TASSERT(TRUE);
		return FALSE;
	}
	return TRUE;
}
const BOOL CTexture::LoadWICFile()
{
	BOOL Check = DirectX::LoadFromWICFile(GetPath().c_str(), DirectX::WIC_FLAGS_NONE, &m_MetaData, m_Image);

	if (S_OK != Check)
	{
		TASSERT(TRUE);
		return FALSE;
	}

	return TRUE;
}



const Vec2 CTexture::GetTextureSize()
{
	return Vec2((float)m_MetaData.width, (float)m_MetaData.height);
}

void CTexture::Update(const UINT& _SlotNum, const UINT& _ShaderFlag /*= SHADER_VERTEX | SHADER_PIXEL*/)
{
	if (0 != (_ShaderFlag & SHADER_VERTEX))
		DXCONTEXT->VSSetShaderResources(_SlotNum, 1, &m_pTexSRView);

	if (0 != (_ShaderFlag & SHADER_HULL))
		DXCONTEXT->HSSetShaderResources(_SlotNum, 1, &m_pTexSRView);

	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		DXCONTEXT->DSSetShaderResources(_SlotNum, 1, &m_pTexSRView);

	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		DXCONTEXT->GSSetShaderResources(_SlotNum, 1, &m_pTexSRView);

	if (0 != (_ShaderFlag & SHADER_PIXEL))
		DXCONTEXT->PSSetShaderResources(_SlotNum, 1, &m_pTexSRView);
}

void CTexture::PopShaderResource(const UINT& _SlotNum, const UINT& _ShaderFlag/* = SHADER_VERTEX | SHADER_PIXEL*/)
{
	ID3D11ShaderResourceView*  Empty = nullptr;

	if (0 != (_ShaderFlag & SHADER_VERTEX))
		DXCONTEXT->VSSetShaderResources(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_HULL))
		DXCONTEXT->HSSetShaderResources(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		DXCONTEXT->DSSetShaderResources(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		DXCONTEXT->GSSetShaderResources(_SlotNum, 1, &Empty);

	if (0 != (_ShaderFlag & SHADER_PIXEL))
		DXCONTEXT->PSSetShaderResources(_SlotNum, 1, &Empty);

}

void CTexture::SetPixel(void* _Data, UINT _ByteSize)
{
	D3D11_MAPPED_SUBRESOURCE Map;

	DXCONTEXT->Map(m_pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);
	memcpy_s(Map.pData, _ByteSize, _Data, _ByteSize);
	DXCONTEXT->Unmap(m_pTexture, 0);
}

Vec4 CTexture::GetPixel(const int _X, const int _Y)
{
	if (GetTextureSize().x <= _X || GetTextureSize().y <= _Y)
	{
		return Vec4::Zero;
	}

	size_t Size = CShader::SizeofFmt(m_Image.GetMetadata().format);
	unsigned char Color[4];
	uint8_t* ptr = m_Image.GetPixels();

	ptr += ((m_Image.GetMetadata().width * _Y) * Size) + (_X * Size);
	memcpy_s(&Color, Size, ptr, Size);

	Vec4 ReturnColor = Vec4(
		Color[0] / 255.f, 
		Color[1] / 255.f, 
		Color[2] / 255.f, 
		Color[3] / 255.f);

	return ReturnColor;
 }

void CTexture::CreateTextueView()
{
	if (nullptr == m_pTexture)
		return;


	if (m_TexDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (S_OK != DXDEVICE->CreateDepthStencilView(m_pTexture, nullptr, &m_pTexDSView))
		{
			assert(nullptr);
			return;
		}
	}
	else 
	{
	
		if (m_TexDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			if (S_OK != DXDEVICE->CreateShaderResourceView(m_pTexture, nullptr, &m_pTexSRView))
			{
				assert(nullptr);
				return;
			}
		}

		if (m_TexDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
		{

			if (S_OK != DXDEVICE->CreateRenderTargetView(m_pTexture, nullptr, &m_pTexRTView))
			{
				assert(nullptr);
				return;
			}
		}
	}
}

const bool CTexture::Load(const LOAD_DESC& _Desc)
{
	CCriSectionObject<CDevice> Lock;
	std::wstring Ext = GetExtension();

	if ((Ext == L".dds" || Ext == L".DDs"))
	{
		if (false == LoadDDSFile())
		{
			return false;
		}
	}
	else if ((Ext == L".tga" || Ext == L".TGA"))
	{
		if (false == LoadTGAFile())
		{
			return false;
		}
	}
	else if (Ext == L".hdr")
	{
		if (false == LoadHDRFile())
		{
			return false;
		}
	}
	else if (FALSE == LoadWICFile())
	{
		return false;
	}

	
	HRESULT Check = DirectX::CreateTexture(DXDEVICE, m_Image.GetImage(0, 0, 0), m_Image.GetImageCount(), m_Image.GetMetadata(), &m_pTexture);

	if (Check != S_OK)
	{
		TASSERT(true);
		return false;
	}

	Check = DirectX::CreateShaderResourceView(DXDEVICE, m_Image.GetImage(0, 0, 0), m_Image.GetImageCount(), m_Image.GetMetadata(), &m_pTexSRView);

	if (S_OK != Check)
	{
		TASSERT(true);
		return false;
	}

	m_MetaData = m_Image.GetMetadata();

	return true;
}


SPTR<CTexture> CTexture::GetCopyCreateTexture()
{
	return CreateTexture(m_TexDesc.Width, m_TexDesc.Height, m_TexDesc.Format, m_TexDesc.BindFlags, m_TexDesc.Usage);
}

////////////////////MultiTexture////////////////////////////

CMultiTexture::CMultiTexture()
{
}

CMultiTexture::~CMultiTexture()
{
}



const bool CMultiTexture::Create(const CREATE_DESC& _Desc)
{
	UINT Count = DXMath::Clamp(_Desc.RTCount, (UINT)0, (UINT)MAXRENDERTARGET);
	m_vecTex.reserve(Count);

	for (UINT i = 0; i < Count; ++i)
	{
		if (nullptr == _Desc.RTKey[i])
			continue;

		SPTR<CTexture> TempTexture = CResourceMgr<CTexture>::Find(_Desc.RTKey[i]);

		if (nullptr == TempTexture)
			assert(nullptr);
		else
			m_vecTex.push_back(TempTexture);
	}

	CCriSectionObject<CDevice> Lock;
	for (size_t i = 0; i < m_vecTex.size(); ++i)
	{
		if (nullptr == m_vecTex[i]
			|| nullptr == m_vecTex[i]->GetTextureResource())
		{
			assert(nullptr);
			continue;
		}

		D3D11_TEXTURE2D_DESC CopyDesc = { 0, };

		//텍스쳐 desc 가져오기 - 복사하기 위해서
		ID3D11Resource* TempTexture = m_vecTex[i]->GetTextureResource();
		((ID3D11Texture2D*)TempTexture)->GetDesc(&CopyDesc);

		CopyDesc.BindFlags = 0;
		CopyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		//내가 GPU에 올려놓은 값을 가져오고 싶을 때 사용하는 옵션
		CopyDesc.Usage = D3D11_USAGE_STAGING;

		D3D11_SUBRESOURCE_DATA SubData = { 0, };
		SubData.pSysMem = m_vecTex[i]->GetScratchImage()->GetPixels();
		SubData.SysMemPitch = (UINT)m_vecTex[i]->GetScratchImage()->GetImages()->rowPitch;
		SubData.SysMemSlicePitch = (UINT)m_vecTex[i]->GetScratchImage()->GetImages()->slicePitch;

		//내가 넣어준 텍스쳐를 새롭게 만든 텍스쳐에 복사한다 - 읽어서 사용할 수 있도록
		ID3D11Texture2D* CopyTex = nullptr;
		if (S_OK != DXDEVICE->CreateTexture2D(&CopyDesc, &SubData, &CopyTex))
		{
			assert(nullptr);
			return nullptr;
		}
		m_vecResource.push_back(CopyTex);
	}

	//가장 앞에 있는 이미지 정보를 가지고 멀티텍스쳐를 만든다
	D3D11_TEXTURE2D_DESC TempDesc = { 0, };
	TempDesc.Width = (UINT)m_vecTex[0]->GetScratchImage()->GetMetadata().width;
	TempDesc.Height = (UINT)m_vecTex[0]->GetScratchImage()->GetMetadata().height;
	TempDesc.Format = m_vecTex[0]->GetScratchImage()->GetMetadata().format;
	TempDesc.ArraySize = (UINT)m_vecTex.size();
	TempDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TempDesc.Usage = _Desc.Usage;
	TempDesc.SampleDesc.Count = 1;
	TempDesc.SampleDesc.Quality = 0;
	TempDesc.MipLevels = 1;

	if (TempDesc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
	{
		TempDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		TempDesc.CPUAccessFlags = 0;
	}
	
	//빈 공간만 생겨있는 상태
	if (S_OK != DXDEVICE->CreateTexture2D(&TempDesc, nullptr, (ID3D11Texture2D**)&m_pTexture))
	{
		assert(nullptr);
		return false;
	}
	
	for (size_t i = 0; i < m_vecTex.size(); ++i)
	{
		D3D11_MAPPED_SUBRESOURCE Sub = { 0, };
		DXCONTEXT->Map(m_vecResource[i], 0, D3D11_MAP_READ, 0, &Sub);

		//Cpu에서 읽는것
		DXCONTEXT->UpdateSubresource(
			m_pTexture,
			D3D11CalcSubresource(0, (UINT)i, TempDesc.MipLevels), 
			nullptr, 
			Sub.pData, 
			(UINT)Sub.RowPitch, 
			(UINT)Sub.DepthPitch);

		DXCONTEXT->Unmap(m_vecResource[i], 0);
	}

	CreateMultiTextureView(TempDesc.BindFlags);

	for (int i = 0; i < m_vecResource.size(); ++i)
	{
		if (nullptr != m_vecResource[i])
		{
			m_vecResource[i]->Release();
			m_vecResource[i] = nullptr;
		}
	}
	return true;
}

void CMultiTexture::CreateMultiTextureView(UINT _BindFlag)
{
	if (nullptr == m_pTexture)
	{
		assert(nullptr);
		return;
	}

	if (_BindFlag & D3D11_BIND_SHADER_RESOURCE)
	{
		if (S_OK != DXDEVICE->CreateShaderResourceView(m_pTexture, nullptr, &m_pTexSRView))
		{
			assert(nullptr);
			return;
		}
	}
}

const Vec2 CMultiTexture::GetMultiTextureSize()
{
	return Vec2((float)m_vecTex[0]->GetScratchImage()->GetMetadata().width, (float)m_vecTex[0]->GetScratchImage()->GetMetadata().height);
}
