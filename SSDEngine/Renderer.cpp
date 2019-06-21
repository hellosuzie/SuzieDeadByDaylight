#include "Renderer.h"
#include "TransForm.h"
#include "Actor.h"
#include "Scene.h"
#include "DXMath.h"
#include "Material.h"
#include "Mesh.h"
#include "Device.h"
#include "ConstantBuffer.h"
#include "Fbx.h"

void CRenderer::SetRenderMode(RENDER_MODE _Mode)
{
	m_RenderMode = _Mode;

	switch (_Mode)
	{
	case RENDMODE_NORMAL:
		RenderPtr = &CRenderer::NormalModeRender;
		break;
	default:
		break;
	}
}


CRenderer::CRenderer()
	:m_RenColorFactor(Vec4(1.f,1.f,1.f,1.f)), m_fFactorTime(0.f)
	, m_StartColorFactor(Vec4()), m_EndColorFactor(Vec4())
	, m_bAutoFactor(false), m_fAccFactorTime(0.f), RenderPtr(nullptr)
{
	m_vecMesh.clear();
	m_vecMaterial.clear();
	SetRenderMode(RENDER_MODE::RENDMODE_NORMAL);
}


CRenderer::~CRenderer()
{
	m_vecMesh.clear();
	m_vecMaterial.clear();
}

void CRenderer::PushMesh(const wchar_t* _MeshKey, const UINT& _Idx /*= 0*/)
{
	SPTR<CMesh> FindMesh = CResourceMgr<CMesh>::Find(_MeshKey);

	if (nullptr == FindMesh) 
	{
		assert(false);
		return;
	}

	if (m_vecMesh.size() <= _Idx + 1)
		m_vecMesh.resize(_Idx + 1);

	m_vecMesh[_Idx] = FindMesh;

}
void CRenderer::PushMesh(SPTR<CMesh> _Mesh, const UINT& _Idx /*= 0*/)
{
	if (nullptr == _Mesh)
		return;

	if (m_vecMesh.size() <= _Idx + 1)
		m_vecMesh.resize(_Idx + 1);

	m_vecMesh[_Idx] = _Mesh;
}
void CRenderer::PushMaterial(const wchar_t* _MatKey, const UINT& _Idx /*= 0*/)
{
	SPTR<CMaterial> FindMat = CResourceMgr<CMaterial>::Find(_MatKey);

	if (nullptr == FindMat)
		return;

	if (m_vecMaterial.size() <= _Idx + 1)
		m_vecMaterial.resize(_Idx + 1);

	m_vecMaterial[_Idx] = FindMat->Clone();
}

SPTR<CMesh> CRenderer::GetMesh(const UINT& _Idx)
{
	if (_Idx >= m_vecMesh.size())
		return nullptr;

	return m_vecMesh[_Idx];
}
SPTR<CMaterial> CRenderer::GetMaterial(const UINT& _Idx) 
{
	if (_Idx >= m_vecMaterial.size())
		return nullptr;

	return m_vecMaterial[_Idx];
}



void CRenderer::Render(SPTR<CCamera> _Cam)
{
#ifdef _DEBUG
	if (nullptr == RenderPtr)
	{
		TASSERT(true);
	}
#endif // _DEBUG

	(this->*RenderPtr)(_Cam);

}

void CRenderer::RenderUpdate(SPTR<CCamera> _Cam)
{
	SubTransUpdate();

	m_BaseBuffer.World = GetSubTransCRWorld().GetTransposeMat();
	m_BaseBuffer.InvWorld = m_BaseBuffer.World.GetInverseMat().GetTransposeMat();
	m_BaseBuffer.NormalWorld = GetSubTransCRWorld().GetInverseMat();
	
	m_BaseBuffer.View = _Cam->GetViewMatrix().GetTransposeMat();
	m_BaseBuffer.Proj = _Cam->GetProjMatrix().GetTransposeMat();
	m_BaseBuffer.WV = m_BaseBuffer.View * m_BaseBuffer.World;
	m_BaseBuffer.NormalWV = m_BaseBuffer.WV.GetInverseMat().GetTransposeMat();

	m_BaseBuffer.VP = m_BaseBuffer.Proj * m_BaseBuffer.View;
	m_BaseBuffer.WVP = m_BaseBuffer.Proj * m_BaseBuffer.WV;

	m_BaseBuffer.ColorFactor = m_RenColorFactor;
	m_BaseBuffer.CamWorldPos = _Cam->GetTrans()->GetWorldPos();

	if (nullptr != CDevice::RenBaseCB)
		CDevice::RenBaseCB->Update(m_BaseBuffer);
}

void CRenderer::NormalModeRender(SPTR<CCamera> _Cam)
{
	for (size_t MeshCount = 0; MeshCount < m_vecMesh.size(); ++MeshCount)
	{
		for (UINT IdxCount = 0; IdxCount < m_vecMesh[MeshCount]->IdxBufferSize(); ++IdxCount)
		{
			for (size_t MatCount = 0; MatCount < m_vecMaterial.size(); ++MatCount)
			{
				if (CSceneRenderMgr::IsDeferred != m_vecMaterial[MatCount]->IsDeferred())
				{
					continue;
				}
				m_vecMaterial[MatCount]->Update();
				m_vecMesh[MeshCount]->Update();
				m_vecMesh[MeshCount]->Render(IdxCount);
				m_vecMaterial[MatCount]->TexSamRollBack();
				m_vecMaterial[MatCount]->StateRollBack();
			}
		}
	}
}


void CRenderer::BaseCBUpdate(SPTR<CCamera> _Cam, const Mat& _World)
{
	m_BaseBuffer.World = _World.GetTransposeMat();
	m_BaseBuffer.InvWorld = _World.GetInverseMat().GetTransposeMat();
	m_BaseBuffer.NormalWorld = _World.GetInverseMat();


	m_BaseBuffer.View = _Cam->GetViewMatrix().GetTransposeMat();
	m_BaseBuffer.Proj = _Cam->GetProjMatrix().GetTransposeMat();

	m_BaseBuffer.WV = m_BaseBuffer.View * m_BaseBuffer.World;
	m_BaseBuffer.NormalWV = m_BaseBuffer.View * m_BaseBuffer.NormalWorld;
	m_BaseBuffer.VP = m_BaseBuffer.Proj * m_BaseBuffer.View;
	m_BaseBuffer.WVP = m_BaseBuffer.Proj * m_BaseBuffer.WV;
	m_BaseBuffer.ColorFactor = m_RenColorFactor;
	m_BaseBuffer.CamWorldPos = _Cam->GetTrans()->GetWorldPos();

	if (nullptr != CDevice::RenBaseCB)
		CDevice::RenBaseCB->Update(m_BaseBuffer);
}


void CRenderer::SetBaseBuffer(const SHADER_TYPE& _Type) 
{
	if (nullptr != CDevice::RenBaseCB)
		CDevice::RenBaseCB->SupplyShader(_Type, BASESLOT);
}
void CRenderer::SetBaseBuffer(const UINT& _ShaderFlag) 
{
	if (nullptr != CDevice::RenBaseCB)
		CDevice::RenBaseCB->SupplyShader(_ShaderFlag, BASESLOT);
}
const bool CRenderer::Init(const RENDATA& _InitData)
{
	m_RenderLayer = _InitData.RenderLayer;

	m_pBelongScene->GetRenderMgr()->PushRenderer(this);
	return true;
}


void CRenderer::PostUpdate(const float& _DeltaTime)
{
	if (false == m_bAutoFactor)
		return;


	if (m_fFactorTime <= m_fAccFactorTime)
	{
		m_bAutoFactor = false;
		m_RenColorFactor = m_EndColorFactor;
		return;
	}


	m_RenColorFactor = DXMath::Vec4Lerp(m_StartColorFactor, m_EndColorFactor, m_fAccFactorTime / m_fFactorTime);
	m_fAccFactorTime += _DeltaTime;
}
void CRenderer::SetRenPivot(const Vec3& _Pivot)
{
	m_RenPivot = _Pivot;
	SetSubPivot(_Pivot);
}
void CRenderer::SetRenSize(const Vec3& _Size) 
{
	m_RenSize = _Size;
	SetSubSize(_Size);
}
void CRenderer::SetAlpha(const float& _Alpha)
{
	m_RenColorFactor.w = _Alpha;
}
const Vec3 CRenderer::GetRenSize()
{
	if (nullptr == m_pSubData)
		return TRANS->GetScale();

	return m_pSubData->SubSize;
}
const Vec3 CRenderer::GetRenPivot()
{
	if (nullptr == m_pSubData)
		return Vec3::Zero3;

	return m_pSubData->SubPivot;
}