#include"Sprite2DRenderer.h"
#include"Mesh.h"
#include"VtxShader.h"
#include"PixShader.h"
#include"Device.h"
#include"BlendState.h"
#include"ResourceMgr.h"
#include"TransForm.h"
#include"ConstantBuffer.h"
#include"Sprite.h"
#include"TimerMgr.h"
#include"DXStruct.h"
#include"DepthStencilState.h"
#include"Camera.h"

_tagSPRITE2DDATA::_tagSPRITE2DDATA() 
	:RenderLayer(MAX_RENDER_LAYER), SizeType(RENSIZE_SPRITESIZE), SizeFactor(Vec3(0.f,0.f,1.f))
{

}
_tagSPRITE2DDATA::_tagSPRITE2DDATA(const RENDER_LAYER& _Layer, const SPRITE_RENSIZE_TYPE& _SizeType, const Vec3& _CustomSize) 
	: RenderLayer(_Layer), SizeType(_SizeType), SizeFactor(_CustomSize)
{

}
_tagSPRITE2DDATA::_tagSPRITE2DDATA(const RENDER_LAYER& _Layer, const SPRITE_RENSIZE_TYPE& _SizeType, const float& _SpriteMag) 
	: RenderLayer(_Layer), SizeType(_SizeType), SizeFactor(Vec3(_SpriteMag, _SpriteMag, 1.f))
{

}
_tagSPRITE2DDATA::~_tagSPRITE2DDATA() 
{

}


CSprite2DRenderer::CSprite2DRenderer()
	:m_SpriteIndex(0), m_SpriteSizeMag(Vec3())
	, m_Sprite(nullptr), m_Mesh(nullptr), m_VS(nullptr), m_PS(nullptr)
{

}


CSprite2DRenderer::~CSprite2DRenderer()
{

}

const bool CSprite2DRenderer::Init(const RENDATA& _InitData)
{
	m_Mesh = CResourceMgr<CMesh>::Find(L"2DMesh");
	m_VS = CResourceMgr<CVtxShader>::Find(L"Sprite2D");
	m_PS = CResourceMgr<CPixShader>::Find(L"Sprite2D");

	return CRenderer::Init(_InitData);
}
const bool CSprite2DRenderer::Init(const SPRITE2D_RENDATA& _InitData)
{
	m_Mesh = CResourceMgr<CMesh>::Find(L"2DMesh");
	m_VS = CResourceMgr<CVtxShader>::Find(L"Sprite2D");
	m_PS = CResourceMgr<CPixShader>::Find(L"Sprite2D");

	m_eSizeType = _InitData.SizeType;

	switch (_InitData.SizeType)
	{
	case RENSIZE_SPRITESIZE:
		m_SpriteSizeMag = _InitData.SizeFactor;
	break;
	case RENSIZE_CUSTOMSIZE:
		SetRenSize(_InitData.SizeFactor);
	break;
	default:
		break;
	}

	return CRenderer::Init(RENDATA{ _InitData.RenderLayer });
}
void CSprite2DRenderer::Render(SPTR<class CCamera> _Cam)
{
	if (nullptr == m_Sprite)
		return;

	if (true == CGameObject::IsDebug())
		int a = 0;

	DXCONTEXT->HSSetShader(nullptr, 0, 0);
	DXCONTEXT->DSSetShader(nullptr, 0, 0);
	DXCONTEXT->GSSetShader(nullptr, 0, 0);

	m_Mesh->Update();

	m_VS->Update();
	m_PS->Update();

	m_Sprite->Update();
	m_Mesh->Render();

	m_Sprite->PostRender();
}

void CSprite2DRenderer::RenderUpdate(SPTR<CCamera> _Cam)
{
	Vec4 SpriteUV = m_Sprite->GetSpriteUV(m_SpriteIndex);
	SubTransUpdate();

	Vec3 RenSize = GetRenSize();

	Mat World = GetSubTransCRWorld();

	if (RenSize.x < 0.f)
	{
		SpriteUV.x = SpriteUV.x + SpriteUV.z;
		SpriteUV.z = -SpriteUV.z;
		World.Row[0] = -World.Row[0];
	}
	if (RenSize.y < 0.f)
	{
		SpriteUV.y = SpriteUV.y + SpriteUV.w;
		SpriteUV.w = -SpriteUV.w;
		World.Row[1] = -World.Row[1];
	}

	BaseCBUpdate(_Cam, World);
	m_VS->BufferUpdate<CBUFFER::Sprite2DBuffer>(0, &CBUFFER::Sprite2DBuffer(SpriteUV));
}
const bool CSprite2DRenderer::SetSprite(const wchar_t* _SpriteKey)
{
	SPTR<CSprite> ChangeSprite = CResourceMgr<CSprite>::Find(_SpriteKey);

	if (nullptr == ChangeSprite)
		return false;

	m_Sprite = ChangeSprite;
	m_SpriteIndex = 0;

	if (RENSIZE_SPRITESIZE == m_eSizeType) 
		SetRenSize(m_Sprite->GetSpriteSize() * m_SpriteSizeMag);
	
	return true;
}
size_t CSprite2DRenderer::SetSpriteIndex(const size_t& _Index)
{
	if (m_Sprite->GetMaxSpriteIndex() < _Index)
		return m_SpriteIndex;

	return m_SpriteIndex = _Index;
}

const Vec4 CSprite2DRenderer::GetSpriteUV()
{
	return m_Sprite->GetSpriteUV(m_SpriteIndex);
}
const size_t CSprite2DRenderer::GetSpriteIndex()
{
	return m_SpriteIndex;
}
void CSprite2DRenderer::SetSpriteSizeMag(const Vec2& _Mag)
{
	if (RENSIZE_SPRITESIZE != m_eSizeType)
		return;

	m_SpriteSizeMag.vec2 = _Mag;
	SetRenSize(m_Sprite->GetSpriteSize() * m_SpriteSizeMag);
}
void CSprite2DRenderer::SetSpriteSizeMag(const float& _Mag)
{
	if (RENSIZE_SPRITESIZE != m_eSizeType)
		return;

	m_SpriteSizeMag.vec2 = Vec2(_Mag,_Mag);
	SetRenSize(m_Sprite->GetSpriteSize() * m_SpriteSizeMag);
}

void CSprite2DRenderer::SetSpriteSizeXMag(const float& _Mag) 
{
	if (RENSIZE_SPRITESIZE != m_eSizeType)
		return;

	m_SpriteSizeMag.x = _Mag;
	SetRenSize(m_Sprite->GetSpriteSize() * m_SpriteSizeMag);
} 
void CSprite2DRenderer::SetSpriteSizeYMag(const float& _Mag) 
{
	if (RENSIZE_SPRITESIZE != m_eSizeType)
		return;

	m_SpriteSizeMag.y = _Mag;
	SetRenSize(m_Sprite->GetSpriteSize() * m_SpriteSizeMag);
}

const Vec3 CSprite2DRenderer::GetRenWorldPos()
{
	return GetSubTransCRWorld().Row[3].vec3;
}
const Vec3 CSprite2DRenderer::GetRenSize()
{
	if (nullptr != m_pSubData)
		return m_pSubData->SubSize;

	return TRANS->GetScale();
}

const Vec3 CSprite2DRenderer::GetRenRotate()
{
	return TRANS->GetLocalRotate();
}
const Vec3 CSprite2DRenderer::GetRenRevolve()
{
	return TRANS->GetLocalRevolve();
}
const Vec3 CSprite2DRenderer::GetCurSpriteSize()
{
	return m_Sprite->GetSpriteSize();
}
SPTR<class CTexture> CSprite2DRenderer::GetSpriteTexture()
{
	if (nullptr == m_Sprite)
		return nullptr;

	return m_Sprite->GetTexture();
}