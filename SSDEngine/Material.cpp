#include "Material.h"
#include "VtxShader.h"
#include "GeoShader.h"
#include "PixShader.h"
#include "ResourceMgr.h"
#include "Texture.h"
#include "SamplerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "Engine.h"
#include "RenderTarget.h"
#include "SSDCore.h"
#include "Fbx.h"

CMaterial::CMaterial()
	: m_bOriginal(false), m_VS(nullptr), m_GS(nullptr), m_PS(nullptr)
	, m_Blend(nullptr), m_RSKey(L""), m_DSKey(L""), m_bDeferred(false), m_MatViewportExist(false)
{
	m_mapTexture.clear();
	m_mapSampler.clear();
	m_mapShaderCB.clear();
}

CMaterial::CMaterial(const CMaterial& _Other) 
	:CResource(_Other),m_bOriginal(false)
	,m_VS(_Other.m_VS),m_GS(_Other.m_GS), m_PS(_Other.m_PS)
	, m_Blend(_Other.m_Blend), m_RSKey(_Other.m_RSKey), m_DSKey(_Other.m_DSKey)
	,m_MaterialRS(_Other.m_MaterialRS),m_MaterialDS(_Other.m_MaterialDS), m_bDeferred(_Other.m_bDeferred), m_MatViewportExist(false)
{
	m_mapTexture.clear();
	m_mapSampler.clear();
	m_mapShaderCB.clear();

	m_mapTexture.insert(_Other.m_mapTexture.begin(), _Other.m_mapTexture.end());
	m_mapSampler.insert(_Other.m_mapSampler.begin(), _Other.m_mapSampler.end());

	m_mapShaderCB.insert(_Other.m_mapShaderCB.begin(), _Other.m_mapShaderCB.end());
}

CMaterial::~CMaterial()
{
	m_mapTexture.clear();
	m_mapSampler.clear();
}
void CMaterial::Update()
{
	ViewportUpdate();
	ShaderUpdate();
	ConstantBufferUpdate();

	TextureUpdate();
	SamplerUpdate();

	BlendUpdate();
	RSUpdate();
	DSUpdate();
}

void CMaterial::SetShader(const SHADER_TYPE& _Shader, const wchar_t* _ShaderKey)
{
	switch (_Shader)
	{
	case SHADER_VERTEX: 
		m_VS = CResourceMgr<CVtxShader>::Find(_ShaderKey); 
		break;
	case SHADER_HULL: 
		break;
	case SHADER_DOMAIN:
		break;
	case SHADER_GEOMETRY:
		m_GS = CResourceMgr<CGeoShader>::Find(_ShaderKey); 
		break;
	case SHADER_PIXEL: 
		m_PS = CResourceMgr<CPixShader>::Find(_ShaderKey);
		break;
	default:
		return;
	}
}

void CMaterial::SetShader(const unsigned int& _ShaderFlag, const wchar_t* _ShaderKey) 
{
	if (0 != (_ShaderFlag & SHADER_VERTEX))
		m_VS = CResourceMgr<CVtxShader>::Find(_ShaderKey);
	if (0 != (_ShaderFlag & SHADER_GEOMETRY))
		m_GS = CResourceMgr<CGeoShader>::Find(_ShaderKey);
	if (0 != (_ShaderFlag & SHADER_PIXEL))
		m_PS = CResourceMgr<CPixShader>::Find(_ShaderKey);
	if (0 != (_ShaderFlag & SHADER_HULL))
		return;
	if (0 != (_ShaderFlag & SHADER_DOMAIN))
		return;
}

SPTR<CVtxShader> CMaterial::GetVtxShader() 
{
	return m_VS;
}

SPTR<CGeoShader> CMaterial::GetGeoShader() 
{
	return m_GS;
}

SPTR<CPixShader> CMaterial::GetPixShader()
{
	return m_PS;
}

void CMaterial::ShaderUpdate()
{
	if (nullptr == m_VS)
		DXCONTEXT->VSSetShader(nullptr, 0, 0);
	else
		m_VS->Update();

	if (nullptr == m_GS)
		DXCONTEXT->GSSetShader(nullptr, 0, 0);
	else
		m_GS->Update();


	if (nullptr == m_PS)
		DXCONTEXT->PSSetShader(nullptr, 0, 0);
	else
		m_PS->Update();
}


//void CMaterial::CreateConstantBuffer( const std::wstring& _CBKey, const CConstantBuffer::CREATE_DESC& _Desc)
//{
//	if (nullptr != MAPFINDSPTR<SPTR<CConstantBuffer>>(m_mapShaderCB, _CBKey))
//		return;
//
//	SPTR<CConstantBuffer> NewCB = Engine::CreateResourceObject<CConstantBuffer>();
//
//	if (false == NewCB->Create(_Desc))
//		return;
//
//	m_mapShaderCB.insert(std::unordered_map<std::wstring, SPTR<CConstantBuffer>>::value_type(_CBKey, NewCB));
//}

void CMaterial::PushConstantBuffer(const std::wstring& _Key, const std::wstring& _CBKey)
{
	SPTR<CConstantBuffer> FindCB = CResourceMgr<CConstantBuffer>::Find(_CBKey);

	if (nullptr == FindCB)
		return;

	m_mapShaderCB.insert(std::unordered_map<std::wstring, ShaderCB>::value_type(_Key, ShaderCB(FindCB)));
}

void CMaterial::ConstantBufferUpdate()
{
	m_CBStart = m_mapShaderCB.begin();
	m_CBEnd = m_mapShaderCB.end();

	// 상수 버퍼 업데이트 할때 맵에 등록된 데이터 값을 복제한후 
	// 쉐이더에 자원을 공급 한다.
	for (; m_CBStart != m_CBEnd; ++m_CBStart) 
	{
		UINT ByteWidth = m_CBStart->second.m_pCB->GetByteWidth();

		// 버퍼 데이터가 따로 등록되어있는경우 상수버퍼의 값을 m_pData로 갱신
		if (nullptr != m_CBStart->second.m_pData )
		{
			m_CBStart->second.m_pCB->Update(m_CBStart->second.m_pData, ByteWidth);
		}
		// 버퍼 데이터가 따로 없을경우 상수 버퍼의 원본 데이터로 복구
		else 
			m_CBStart->second.m_pCB->Update();

		m_CBStart->second.m_pCB->SupplyShader();
	}
}

void CMaterial::PushTexture(const unsigned int& _Slot, const wchar_t* _TexKey, unsigned int _ShaderFlag/* = SHADER_PIXEL*/, const MATERIAL_TEXTURETYPE& _TexType /*= LOAD_TEXTURE*/)
{
	SPTR<CTexture> Tex = nullptr;

	switch (_TexType)
	{
	case LOAD_TEXTURE:
	{
		Tex = CResourceMgr<CTexture>::Find(_TexKey);
	}
		break;
	case TARGET_TEXTURE: 
	{
		SPTR<RenderTarget> Target = CResourceMgr<RenderTarget>::Find(_TexKey);
		if (nullptr != Target) 
		{
			Tex = Target->GetRTTexture();
		}
	}
		break;
	default:
		return;
	}

	if (nullptr == Tex) 
	{
		TASSERT(true);
		return;
	}
		
	std::unordered_map<unsigned int, TexData>::iterator FindIter = m_mapTexture.find(_Slot);

	if (m_mapTexture.end() != FindIter) 
	{
		FindIter->second.pTex = Tex;
		FindIter->second.ShaderFlag = _ShaderFlag;
	}
	else 
	{
		m_mapTexture.insert(std::unordered_map<unsigned int, TexData>::value_type(_Slot, TexData{ Tex, _ShaderFlag }));
	}
}

void CMaterial::PushTargetTexture(const unsigned int& _Slot, SPTR<RenderTarget> _Target , unsigned int _ShaderFlag /*= SHADER_PIXEL*/)
{
	if (nullptr == _Target)
	{
		assert(nullptr);
		return;
	}

	SPTR<CTexture> Tex = _Target->GetRTTexture();
	std::unordered_map<unsigned int, TexData>::iterator FindIter = m_mapTexture.find(_Slot);

	if (m_mapTexture.end() != FindIter)
	{
		FindIter->second.pTex = Tex;
		FindIter->second.ShaderFlag = _ShaderFlag;
	}
	else
	{
		m_mapTexture.insert(std::unordered_map<unsigned int, TexData>::value_type(_Slot, TexData{ Tex, _ShaderFlag }));
	}

}

void CMaterial::PushSampler(const unsigned int& _Slot, const wchar_t* _SamplerKey, unsigned int _ShaderFlag /*= SHADER_PIXEL*/)
{
	SPTR<CSamplerState> Sampler = CResourceMgr<CSamplerState>::Find(_SamplerKey);

	if (nullptr == Sampler) 
	{
		TASSERT(true);
		return;
	}
	

	std::unordered_map<unsigned int, SamplerData>::iterator FindIter = m_mapSampler.find(_Slot);

	if (m_mapSampler.end() != FindIter)
	{
		FindIter->second.pSampler = Sampler;
		FindIter->second.ShaderFlag = _ShaderFlag;
	}
	else
	{
		m_mapSampler.insert(std::unordered_map<unsigned int, SamplerData>::value_type(_Slot, SamplerData{ Sampler, _ShaderFlag }));
	}
}

void CMaterial::TextureUpdate() 
{
	std::unordered_map<unsigned int, TexData>::iterator Start = m_mapTexture.begin();
	std::unordered_map<unsigned int, TexData>::iterator End = m_mapTexture.end();

	for (; Start!= End;++Start)
	{
		Start->second.pTex->Update(Start->first, Start->second.ShaderFlag);
	}
}

void CMaterial::SamplerUpdate() 
{
	std::unordered_map<unsigned int, SamplerData>::iterator Start = m_mapSampler.begin();
	std::unordered_map<unsigned int, SamplerData>::iterator End = m_mapSampler.end();

	for (; Start != End; ++Start)
	{
		Start->second.pSampler->Update(Start->first, Start->second.ShaderFlag);
	}
}

void CMaterial::SetBlend(const wchar_t* _BlendKey) 
{
	m_Blend = CResourceMgr<CBlendState>::Find(_BlendKey);
}

void CMaterial::BlendUpdate() 
{
	if (nullptr != m_Blend)
		m_Blend->Update();
}


void CMaterial::SetRasterizerState(const wchar_t* _RSKey) 
{
	m_RSKey = _RSKey;

	m_MaterialRS = CResourceMgr<CRasterizerState>::Find(_RSKey);
}
void CMaterial::SetDepthStancilState(const wchar_t* _DSKey) 
{
	m_DSKey = _DSKey;
	m_MaterialDS = CResourceMgr<CDepthStencilState>::Find(_DSKey);
}
void CMaterial::RSUpdate() 
{
	if (nullptr != m_MaterialRS)
		m_MaterialRS->Update();
}
void CMaterial::DSUpdate() 
{
	if (nullptr != m_MaterialDS)
		m_MaterialDS->Update(0);
}
void CMaterial::TexSamRollBack()
{
	for (std::unordered_map<unsigned int, TexData>::value_type& Data  : m_mapTexture)
	{
		Data.second.pTex->PopShaderResource(Data.first, Data.second.ShaderFlag);
	}

	for (std::unordered_map<unsigned int, SamplerData>::value_type& SamData : m_mapSampler)
	{
		SamData.second.pSampler->PopSampler(SamData.first, SamData.second.ShaderFlag);
	}
}
void CMaterial::StateRollBack()
{
	if (nullptr != m_MaterialRS)
		CDevice::SetBaseRS();

	if (nullptr != m_Blend)
		CDevice::SetBaseBlend();

	if (nullptr != m_MaterialDS)
		CDevice::SetBaseDS();
	
}

bool CMaterial::CreateViewport(unsigned int _Width, unsigned int _Height, unsigned int _TopLeftX, unsigned int _TopLeftY)
{
	m_MatViewport.TopLeftX = (FLOAT)_TopLeftX;
	m_MatViewport.TopLeftY = (FLOAT)_TopLeftY;

	if (0 == _Width)
	{
		m_MatViewport.Width = (float)SSDCore::GetWindowWidth();
	}
	else
	{
		m_MatViewport.Width = (float)_Width;
	}

	if (0 == _Height)
	{
		m_MatViewport.Height = (float)SSDCore::GetWindowHeight();
	}
	else
	{
		m_MatViewport.Height = (float)_Height;
	}

	m_MatViewport.MinDepth = 0;
	m_MatViewport.MaxDepth = 1;

	m_MatViewportExist = true;
	return true;
}

void CMaterial::ViewportUpdate()
{
	if (true == m_MatViewportExist)
	{
		CDevice::GetContext()->RSSetViewports(1, &m_MatViewport);
	}
}

CMaterial* CMaterial::Clone()
{
	return new CMaterial(*this);
}

const bool CMaterial::Create(const CREATE_DESC& _Desc)
{
	m_bOriginal = true;
	return true;
}

void CMaterial::SetEmivColor(int _Index, Vec4 _Color)
{
	m_DrawFbx->GetBoneMesh(0)->GetMaterialValue(_Index)->MaterialColor.Emissive = _Color;
}