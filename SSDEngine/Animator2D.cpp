#include "Animator2D.h"
#include"Actor.h"
#include"Sprite.h"
#include"ResourceMgr.h"
#include"Texture.h"

void CAnimator2D::CAnimation2D::ResetAnimation()
{
	m_bEndAnimation = false;
	m_bNotifyFlag = false;
	m_CurFrame = m_StartFrame;
	m_fAccTime = 0.f;
}
void CAnimator2D::CAnimation2D::RegisteNotify(const unsigned int& _Frame)
{
	if (_Frame > m_EndFrame - m_StartFrame)
		return;

	for (int& RegiFrame : m_listNotifyFrame)
	{
		if (_Frame == RegiFrame)
			return;
	}

	m_listNotifyFrame.push_back(_Frame);
}
const bool CAnimator2D::CAnimation2D::CheckNotifyFrame()
{
	for (int& RegiFrame : m_listNotifyFrame)
	{
		if (m_CurFrame - m_StartFrame == RegiFrame)
			return true;
	}
	return false;
}
size_t CAnimator2D::CAnimation2D::Update(const float& _DeltaTime) 
{
	// 애니메이션 프레임이 한장 일 경우 시간계산을 수행하지 않는다.
	if (true == m_bOneFrameAni)
		return m_CurFrame;

	// 반복 애니메이션이 아닌경우 마지막 프레임만 출력한다.
	if (false == m_bLoop && m_bEndAnimation)
		return m_EndFrame;

	m_bEndAnimation = false;
	m_bNotifyFlag = false;

	m_fAccTime += _DeltaTime;

	if (m_fAccTime >= m_fFrameInterval) 
	{
		unsigned int Frame = (unsigned int)(m_fAccTime / m_fFrameInterval);
		m_CurFrame += Frame;
		m_fAccTime = 0.f;

		if (CheckNotifyFrame())
			m_bNotifyFlag = true;
	}

	// 현재 프레임이 마지막 프레임을 넘긴 경우 애니메이션 루프체크를 수행한다.
	if (m_CurFrame > m_EndFrame) 
	{
		// 애니메이션이 끝난 순간에만 이 값이 true가 된다.
		m_bEndAnimation = true;

		if (true == m_bLoop) 
			m_CurFrame = m_StartFrame;	
	}

	return m_CurFrame;
}

CAnimator2D::CAnimator2D()
	:m_bAnimationPause(false)
{
}


CAnimator2D::~CAnimator2D()
{
}

const bool CAnimator2D::Init(const ANIDATA& _InitData)
{
	m_SpriteRenderer = ACTOR->AddCom<CSprite2DRenderer>(RENDATA{ _InitData.RenderLayer});

	// 2D 랜더러 생성 실패시 애니메이터생성을 하지 않는다.
	if (nullptr == m_SpriteRenderer)
		return false;

	if (_InitData.MultiSpriteKey != L"") 
	{
		SetName(_InitData.MultiSpriteKey);
		m_SpriteRenderer->SetSprite(_InitData.MultiSpriteKey.c_str());
	}
	return true;
}
const bool CAnimator2D::Init(const ANIDATA_SPRITE2D& _InitData)
{
	m_SpriteRenderer = ACTOR->AddCom<CSprite2DRenderer>(_InitData.SpriteRenData);

	// 2D 랜더러 생성 실패시 애니메이터생성을 하지 않는다.
	if (nullptr == m_SpriteRenderer)
		return false;

	if (_InitData.MultiSpriteKey != L"")
	{
		SetName(_InitData.MultiSpriteKey);
		m_SpriteRenderer->SetSprite(_InitData.MultiSpriteKey.c_str());
	}
	return true;
}

void CAnimator2D::Update(const float& _DeltaTime)
{
	if (nullptr == m_CurAnimation)
		return;

	if (true == m_bAnimationPause)
		return;

	m_SpriteRenderer->SetSpriteIndex(m_CurAnimation->Update(_DeltaTime));
}
void CAnimator2D::DebugOn()
{
	CGameObject::DebugOn();
	m_SpriteRenderer->DebugOn();
}
void CAnimator2D::DebugOff() 
{
	CGameObject::DebugOff();
	m_SpriteRenderer->DebugOff();
}
const Vec4 CAnimator2D::GetCurAnimationUV()
{
	return m_SpriteRenderer->GetSpriteUV();
}
const size_t CAnimator2D::GetCurAniSpriteIndex()
{
	return m_SpriteRenderer->GetSpriteIndex();
}
const bool CAnimator2D::ChangeSprite(const wchar_t* _SpriteKey)
{
	SetName(_SpriteKey);
	return m_SpriteRenderer->SetSprite(_SpriteKey);
}
void CAnimator2D::SetRenPivot(const Vec3& _Pivot) 
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetRenPivot(_Pivot);
}
void CAnimator2D::SetRenSize(const Vec3& _Size) 
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetRenSize(_Size);
}
void CAnimator2D::SetColorFactor(const Vec4& _ColorFactor) 
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetColorFactor(_ColorFactor);
}
void CAnimator2D::SetAutoColorFactor(const Vec4& _StartFactor, const Vec4& _EndFactor, const float& _Time)
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetAutoColorFactor(_StartFactor, _EndFactor, _Time);
}
void CAnimator2D::SetAlpha(const float& _Alpha) 
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetAlpha(_Alpha);
}
void CAnimator2D::SetRenMag(const float& _Mag)
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetSpriteSizeMag(_Mag);
}
void CAnimator2D::SetRenXMag(const float& _Mag) 
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetSpriteSizeXMag(_Mag);
}
void CAnimator2D::SetRenYMag(const float& _Mag) 
{
	if (nullptr == m_SpriteRenderer)
		return;

	m_SpriteRenderer->SetSpriteSizeYMag(_Mag);
}
const Vec3 CAnimator2D::GetRenSize()
{
	return m_SpriteRenderer->GetRenSize();
}
const Vec3 CAnimator2D::GetRenWorldPos() 
{
	return m_SpriteRenderer->GetRenWorldPos();
}
SPTR<CSprite2DRenderer> CAnimator2D::GetAniSpriteRen()
{
	return m_SpriteRenderer;
}
SPTR<CTexture> CAnimator2D::GetspriteTexture()
{
	if (nullptr == m_SpriteRenderer)
		return nullptr;

	return m_SpriteRenderer->GetSpriteTexture();
}
const std::wstring CAnimator2D::GetCurAnimtionKey()
{
	if (nullptr == m_CurAnimation)
		return L"";

	return m_CurAnimation->GetName();
}
void CAnimator2D::SetAnimationNotify(const wchar_t* _AniKey, const int& _NotifyFrame)
{
	SPTR<CAnimation2D> FindAni = FindAnimation(_AniKey);
	
	if (nullptr == FindAni)
		return;

	FindAni->RegisteNotify(_NotifyFrame);
}
SPTR<CAnimator2D::CAnimation2D> CAnimator2D::FindAnimation(const wchar_t* _AniKey)
{
	return UMAPFINDSPTR<SPTR<CAnimation2D>>( m_mapAnimation, _AniKey);
}
SPTR<CAnimator2D::CAnimation2D> CAnimator2D::ChangeAnimation(const wchar_t* _AniKey)
{
	SPTR<CAnimation2D> ChangeAni = FindAnimation(_AniKey);

	if (nullptr == ChangeAni)
		return nullptr;

	m_CurAnimation = ChangeAni;
	m_CurAnimation->ResetAnimation();
	m_bAnimationPause = false;
	m_SpriteRenderer->SetSpriteIndex(m_CurAnimation->m_CurFrame);

	return m_CurAnimation;
}
CAnimator2D::CAnimation2D*	CAnimator2D::CreateAnimation(const wchar_t* _AniKey
	, const unsigned int& _StartFrame, const unsigned int& _EndFrame
	, const float& _FrameTime /*= 0.1f*/, const bool& _Loop/* = true*/)
{	
	if (nullptr != FindAnimation(_AniKey))
		return nullptr;

	CAnimation2D* NewAnimation = Engine::CreateGameObject<CAnimation2D>();
	NewAnimation->m_StartFrame = _StartFrame;
	NewAnimation->m_EndFrame = _EndFrame;
	NewAnimation->m_fFrameInterval = _FrameTime;
	NewAnimation->m_bLoop = _Loop;
	NewAnimation->m_CurFrame = _StartFrame;
	NewAnimation->SetName(_AniKey);

	if (NewAnimation->m_StartFrame == NewAnimation->m_EndFrame)
		NewAnimation->m_bOneFrameAni = true;

	m_mapAnimation.insert(std::unordered_map<std::wstring, SPTR<CAnimation2D>>::value_type(_AniKey, NewAnimation));
	return NewAnimation;
}
CAnimator2D::CAnimation2D*	CAnimator2D::CreateAnimation(const wchar_t* _AniKey
	, const unsigned int& _StartFrame, const unsigned int& _EndFrame
	, const bool& _Loop, const float& _FrameTime/* = 0.1f*/) 
{
	return CreateAnimation(_AniKey, _StartFrame, _EndFrame, _FrameTime, _Loop);
}