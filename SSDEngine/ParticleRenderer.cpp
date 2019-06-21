#include "ParticleRenderer.h"
#include "Mesh.h"
#include "ResourceMgr.h"
#include "TransForm.h"
#include "Actor.h"
#include "Scene.h"
#include "Texture.h"
#include "Material.h"
#include "DXHeader.h"
#include <algorithm>
#include "TimerMgr.h"

float								CParticleRenderer::m_FadeColor = 1.f;

CParticleRenderer::CParticleRenderer()
	:m_ParticleMesh(nullptr), m_ParticleMat(nullptr), m_VtxCount(0), m_AccTime(0.f)
	, m_FadeParticle(PARTICLEVTX()), m_bFade(false), m_fAccFadeTime(0.f), m_fFadeTime(0.f), 
	m_bFadeIn(false), m_fFadeColor(Vec4::White)
{
}


CParticleRenderer::~CParticleRenderer()
{
}

const bool CParticleRenderer::Init(const RENDATA& _InitData)
{
	m_vecParticleVtx.reserve(MAXPARTICLECOUNT);
	m_vecAnim.resize(MAXPARTICLECOUNT);

	//버텍스 버퍼 인덱스 버퍼 null인 메쉬를 만든다 - 사이즈만 할당해주기 - 나중에 map해서 정보 넣는다
	CMesh::CREATE_DESC Desc = CMesh::CREATE_DESC{ nullptr, sizeof(PARTICLEVTX), MAXPARTICLECOUNT, D3D11_USAGE_DYNAMIC
	, nullptr, IDX32::MemSize(), 0, D3D11_USAGE_DYNAMIC, IDX32::Format(), D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST };
	m_ParticleMesh = CResourceMgr<CMesh>::Create(Desc);
	m_ParticleMat = CResourceMgr<CMaterial>::Find(L"Particle");

	//페이드용 파티클
	m_FadeParticle = PARTICLEVTX{ Vec3(), Vec3(), 0.f, 10000.f, 10000.f, Vec2(0.f, 0.f), Vec4(0.f, 0.f, 1.f, 0.f), Vec3(), Vec3(), Vec4() };

	return CRenderer::Init(_InitData);
}

void CParticleRenderer::PostUpdate(const float & _DeltaTime)
{
	m_AccTime += _DeltaTime;
	m_ParticleBuffer.AccTime.x = m_AccTime;

	if (m_bFade)
	{
		if (m_fAccFadeTime >= m_fFadeTime)
		{
			m_bFade = false;
			m_fAccFadeTime = 0.f;
		}
		m_fAccFadeTime += _DeltaTime;

		float Alpha = min(m_fAccFadeTime / m_fFadeTime, 1.f);
		if (true == m_bFadeIn)
			Alpha = 1.f - Alpha;

		m_FadeBuffer.ParticleColor.a = Alpha;
	}
}

void CParticleRenderer::RenderUpdate(SPTR<CCamera> _Cam)
{
	CRenderer::RenderUpdate(_Cam);
	if (0 != m_vecParticleTex.size())
	{
		DXCONTEXT->PSSetShaderResources(9, (UINT)m_vecParticleTex.size(), &m_vecParticleTex[0]);
	}
	if (0 != m_vecFadeParticleTex.size())
	{
		DXCONTEXT->PSSetShaderResources(8, (UINT)m_vecFadeParticleTex.size(), &m_vecFadeParticleTex[0]);
	}
}

void CParticleRenderer::Render(SPTR<CCamera> _Cam)
{
	if (CSceneRenderMgr::IsDeferred != m_ParticleMat->IsDeferred())
	{
		return;
	}

	if (0 < m_vecParticleVtx.size())
	{
		ParticleSort(_Cam);

		CTransForm* ptr = _Cam->TRANS;
		m_ParticleBuffer.AxisX = ptr->GetWorldAxis(AXIS_X);
		m_ParticleBuffer.AxisY = ptr->GetWorldAxis(AXIS_Y);
		m_ParticleMat->SetConstantBufferData(L"ParticleBuffer", m_ParticleBuffer);
		m_ParticleMat->ConstantBufferUpdate();

		m_ParticleMat->Update();
		m_ParticleMesh->Update();

		for (UINT i = 0; i < m_vecParticleVtx.size(); ++i)
		{
			//애니메이션이 있는 파티클이라면 UV계산해서 업데이트해준다
			if (0 != m_vecParticleVtx[i].Check.y)
			{
				UpdateParticleAnim(i, m_vecAnim[i], DELTATIME);
			}
		}
		m_ParticleMesh->VBUpdate(0, (void*)&m_vecParticleVtx[0]);

		for (UINT i = 0; i < m_vecParticleVtx.size(); ++i)
		{
			if (m_vecParticleVtx[i].ParticleViewPos.z < 0.f)
				break;

			m_ParticleMesh->VtxRender(0, 1, i);
		}
		//m_ParticleMesh->VtxRender(0, m_VtxCount);
	}

	//페이드 그리기
	if (true == m_bFade)
	{
		m_ParticleMat->SetConstantBufferData(L"ParticleBuffer", m_FadeBuffer);
		m_FadeParticle = PARTICLEVTX{ _Cam->TRANS->GetWorldPos() + _Cam->TRANS->GetWorldAxis(AXIS_Z) * 2.f,
			Vec3(), CParticleRenderer::m_FadeColor, 10000.f, 10000.f, Vec2(0.f, 0.f), Vec4(0.f, 0.f, 1.f, 0.f),
			_Cam->TRANS->GetWorldAxis(AXIS_X), _Cam->TRANS->GetWorldAxis(AXIS_Y), Vec4() };

		m_ParticleMat->Update();
		m_ParticleMesh->Update();
		m_ParticleMesh->VBUpdate(0, (void*)&m_FadeParticle);
		m_ParticleMesh->VtxRender(0, 1);
	}

	m_ParticleMat->TexSamRollBack();
	m_ParticleMat->StateRollBack();
}

void CParticleRenderer::UpdateParticleAnim(int _Index, PARTICLEANIM& _Anim, float _DeltaTime)
{
	_Anim.AccTime += _DeltaTime;

	if (_Anim.AccTime >= FRAMETIME)
	{
		int Frame = (int)(_Anim.AccTime / FRAMETIME);
		_Anim.CurFrame += Frame;
		if (_Anim.CurFrame >= _Anim.FrameCount)
		{
			_Anim.CurFrame = 0;
		}

		m_vecParticleVtx[_Index].AnimUV.x = (float)(_Anim.CurFrame % (int)_Anim.SpriteCount.x) * m_vecParticleVtx[_Index].AnimUV.z;
		m_vecParticleVtx[_Index].AnimUV.y = (float)(_Anim.CurFrame / (int)_Anim.SpriteCount.x) * m_vecParticleVtx[_Index].AnimUV.w;
		_Anim.AccTime = 0.f;
	}
}

int CParticleRenderer::PushParticle(const PARTICLEVTX& _Particle)
{
	if (m_vecParticleVtx.size() >= MAXPARTICLECOUNT)
	{
		return 0;
	}
	m_vecParticleVtx.push_back(_Particle);
	m_VtxCount = (int)m_vecParticleVtx.size();
	return m_VtxCount - 1;
}

int CParticleRenderer::UpdateParticleData(int _Index, const PARTICLEVTX& _Particle)
{
	m_vecParticleVtx[_Index] = _Particle;
	return _Index;
}

void CParticleRenderer::PushParticleTex(const wchar_t * _MultiTexName, const wchar_t * _FadeMultiTexName)
{
	SPTR<CMultiTexture> MultiTex = CResourceMgr<CMultiTexture>::Find(_MultiTexName);
	if (nullptr == MultiTex)
	{
		assert(nullptr);
		return;
	}
	m_ParticleTex = MultiTex;
	m_vecParticleTex.push_back(MultiTex->GetShaderResourceView());

	SPTR<CMultiTexture> FadeMultiTex = CResourceMgr<CMultiTexture>::Find(_FadeMultiTexName);
	if (nullptr == FadeMultiTex)
	{
		assert(nullptr);
		return;
	}
	m_vecFadeParticleTex.push_back(FadeMultiTex->GetShaderResourceView());
}

void CParticleRenderer::PushFadeParticleTex(const wchar_t * _FadeMultiTexName)
{
	SPTR<CMultiTexture> FadeMultiTex = CResourceMgr<CMultiTexture>::Find(_FadeMultiTexName);
	if (nullptr == FadeMultiTex)
	{
		assert(nullptr);
		return;
	}
	m_vecFadeParticleTex.push_back(FadeMultiTex->GetShaderResourceView());
}

void CParticleRenderer::PushParticleAnimation(int _ParticleIndex, const PARTICLEANIM & _Anim)
{
	//애니메이션 정보 넣어주기
	m_vecAnim[_ParticleIndex] = _Anim;
	m_vecParticleVtx[_ParticleIndex].AnimUV.x = 0.f;
	m_vecParticleVtx[_ParticleIndex].AnimUV.y = 0.f;
	m_vecParticleVtx[_ParticleIndex].AnimUV.z = 1.f / m_vecAnim[_ParticleIndex].SpriteCount.x;
	m_vecParticleVtx[_ParticleIndex].AnimUV.w = 1.f / m_vecAnim[_ParticleIndex].SpriteCount.y;
}

void CParticleRenderer::ParticleSort(SPTR<CCamera> _Cam)
{
	if (0 >= m_vecParticleVtx.size())
	{
		return;
	}
	Mat View = _Cam->GetViewMatrix();

	for (size_t i = 0; i < m_vecParticleVtx.size(); i++)
	{
		m_vecParticleVtx[i].ParticleViewPos = View.Mul_Coord(m_vecParticleVtx[i].ParticlePos);
	}

	std::sort(m_vecParticleVtx.begin(), m_vecParticleVtx.end(), [](const PARTICLEVTX& _Left, const PARTICLEVTX& _Right)->const bool
	{
		return _Left.ParticleViewPos.z > _Right.ParticleViewPos.z;
	});
}

void CParticleRenderer::MoveParticle(const int& _Index, const float& _AccTime, const float& _Speed)
{
	if (0 >= m_vecParticleVtx.size())
	{
		return;
	}

	Mat TempMat = Mat();
	TempMat.SetRotateZMat_ForDegree(_AccTime * _Speed);

	m_vecParticleVtx[_Index].AxisX = TempMat.Mul_Normal(Vec3(1.f, 0.f, 0.f));
	m_vecParticleVtx[_Index].AxisY = TempMat.Mul_Normal(Vec3(0.f, 1.f, 0.f));
	m_ParticleMesh->VBUpdate(0, (void*)&m_vecParticleVtx[0]);
}

void CParticleRenderer::SetFadeEffect(const float & _Time, FADE_TYPE _Type/* = FADE_TYPE::FADE_IN*/, FADE_COLOR _Color/* = FADE_COLOR::FADE_COLOR_BLACK*/)
{
	m_fAccFactorTime = 0.f;
	m_fFadeTime = _Time;
	m_bFade = true;

	if (FADE_OUT == _Type)
	{
		m_bFadeIn = false;
		m_FadeBuffer.ParticleColor.a = 0.f;
	}
	else
	{
		m_bFadeIn = true;
		m_FadeBuffer.ParticleColor.a = 1.f;
	}

	switch (_Color)
	{
	case FADE_COLOR_BLACK:
		CParticleRenderer::m_FadeColor = 1.f;
		break;
	case FADE_COLOR_WHITE:
		CParticleRenderer::m_FadeColor = 0.f;
		break;
	case FADE_COLOR_RED:
		CParticleRenderer::m_FadeColor = 2.f;
		break;
	default:
		break;
	}
}
