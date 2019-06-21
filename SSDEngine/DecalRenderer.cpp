#include "DecalRenderer.h"
#include "Mesh.h"
#include "Material.h"
#include "ResourceMgr.h"
#include "Camera.h"
#include "TransForm.h"
#include "Actor.h"
#include "Scene.h"

CDecalRenderer::CDecalRenderer() :m_DecalMesh(nullptr), m_DecalMaterial(nullptr)
{
}


CDecalRenderer::~CDecalRenderer()
{
}


const bool CDecalRenderer::Init(const RENDATA & _InitData)
{
	m_DecalMesh = CResourceMgr<CMesh>::Find(L"Cube");
	m_DecalMaterial = CResourceMgr<CMaterial>::Find(L"Decal");

	return CRenderer::Init(_InitData);
}

void CDecalRenderer::PostUpdate(const float& _DeltaTime)
{

}

void CDecalRenderer::RenderUpdate(SPTR<CCamera> _Cam)
{
	CRenderer::RenderUpdate(_Cam);
	Mat WV = GetSubTransWorld() * _Cam->GetViewMatrix();

	m_Buffer.WorldViewInv = WV.GetInverseMat().GetTransposeMat();
	m_DecalMaterial->SetConstantBufferData(L"DecalBuffer", m_Buffer);
}

void CDecalRenderer::Render(SPTR<class CCamera> _Cam)
{
	if (CSceneRenderMgr::IsDeferred != m_DecalMaterial->IsDeferred())
	{
		return;
	}

	m_DecalMaterial->Update();
	m_DecalMesh->Update();
	if (L"" != m_TexName)
	{
		m_DecalMaterial->PushTexture(1, m_TexName.c_str());
	}

	if (L"" != m_NormalName)
	{
		m_DecalMaterial->PushTexture(2, m_NormalName.c_str());
	}
	m_DecalMesh->Render();
	m_DecalMaterial->TexSamRollBack();
	m_DecalMaterial->StateRollBack();
}