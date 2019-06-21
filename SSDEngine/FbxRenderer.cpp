#include "FbxRenderer.h"
#include "ResourceMgr.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"

CFbxRenderer::CFbxRenderer()
	:m_RenderingFbx(nullptr)
{

}


CFbxRenderer::~CFbxRenderer()
{
	m_RenderingFbx = nullptr;
}


void CFbxRenderer::SetFbx(const wchar_t* _FbxKey)
{
	m_RenderingFbx = CResourceMgr<CFbx>::Find(_FbxKey);

	if(nullptr == m_RenderingFbx)
	{
#ifdef _DEBUG
		assert(nullptr);
#endif
		return;
	}

	m_vecMesh.clear();
	m_vecMaterial.clear();
}

void CFbxRenderer::Clear()
{
	m_vecMesh.clear();
	m_vecMaterial.clear();
}

void CFbxRenderer::Render(SPTR<class CCamera> _Cam)
{
	for (size_t i = 0; i < m_vecMesh.size(); i++)
	{
		if (true == m_vecMesh[i]->IsDraw()) 
		{
			m_vecMesh[i]->Render();
		}
	}
}

void CFbxRenderer::InstancingRender(SPTR<class CCamera> _Cam)
{
	for (size_t i = 0; i < m_vecMesh.size(); i++)
	{
		m_vecMesh[i]->InstancingRender();
	}
}

void CFbxRenderer::SetAllFbxMesh(const wchar_t* _MatKey /*= L"BaseFBXMaterial"*/)
{
	for (int Idx = 0; Idx < (int)m_RenderingFbx->m_AllFBXMeshVec.size(); ++Idx)
	{
		SPTR<CFBXMesh> Mesh = m_RenderingFbx->GetFBXMesh(Idx, _MatKey);

		if (nullptr == Mesh)
		{
#ifdef _DEBUG
			assert(nullptr);
			return;
#endif
		}
		else
			m_vecMesh.push_back(Mesh);
	}
}

void CFbxRenderer::DrawOn(const UINT& _Idx)
{
	if (m_vecMesh.size() <= _Idx)
		return;

	m_vecMesh[_Idx]->DrawOn();
}
void CFbxRenderer::DrawOff(const UINT& _Idx)
{
	if (m_vecMesh.size() <= _Idx)
		return;

	m_vecMesh[_Idx]->DrawOff();
}