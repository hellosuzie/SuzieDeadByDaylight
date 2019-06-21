#include "DebugRenderer.h"
#include "Material.h"
#include "Camera.h"

CDebugRenderer::CDebugRenderer()
	:m_bChangeCB(false), m_DebugBuf(CBUFFER::DebugBuffer())
{

}


CDebugRenderer::~CDebugRenderer()
{

}

const bool CDebugRenderer::Init(const DEBUG_RENDATA& _InitData)
{
	m_DebugFiType = _InitData.FiOption;

	switch (m_DebugFiType)
	{
	case DF_SPHERE: 
	{
		PushMesh(L"Sphere_Lv8");
		PushMaterial(L"Debug3D");
	}
		break;
	case DF_CUBE: 
	{
		PushMesh(L"Cube");
		PushMaterial(L"Debug3D");
		GetMaterial(0)->SetRasterizerState(L"WireFrame");
	}
		break;
	case DF_CONE:
	{
		PushMesh(L"Cone_Lv6");
		PushMaterial(L"Debug3D");
		GetMaterial(0)->SetRasterizerState(L"WireFrame");
	}
		break;
	case DF_FRUSTUM:
	{
		PushMesh(L"FrustumCube");
		PushMaterial(L"Debug3D");
		GetMaterial(0)->SetRasterizerState(L"WireFrame");
	}
	break;
	default:
		return false;
	}

	m_DebugBuf.FiOption = m_DebugFiType;
	m_DebugBuf.BoarderSize = 1.f;

	GetMaterial(0)->SetConstantBufferData(L"DebugBuffer", m_DebugBuf);


	return CRenderer::Init(RENDATA{ _InitData.RenLayer });
}

void CDebugRenderer::RenderUpdate(SPTR<class CCamera> _Cam)
{
	CRenderer::RenderUpdate(_Cam);

	if (true == m_bChangeCB) 
	{
		GetMaterial(0)->SetConstantBufferData(L"DebugBuffer", m_DebugBuf);
	}
}

bool CDebugRenderer::ChangeDebugType(DEBUGFI_TYPE _Type)
{
	m_DebugBuf.FiOption = _Type;
	m_bChangeCB = true;

	switch (_Type)
	{
	case DF_SPHERE:
	{
		PushMesh(L"Sphere_Lv8");
		PushMaterial(L"Debug3D");
	}
	break;
	case DF_CUBE:
	{
		PushMesh(L"Cube");
		PushMaterial(L"Debug3D");
		GetMaterial(0)->SetRasterizerState(L"NoneCulling");
	}
	break;
	case DF_CONE:
	{
		PushMesh(L"Cone_Lv6");
		PushMaterial(L"Debug3D");
		GetMaterial(0)->SetRasterizerState(L"WireFrame");
	}
	break;
	case DF_FRUSTUM:
	{
		PushMesh(L"FrustumCube");
		PushMaterial(L"Debug3D");
		GetMaterial(0)->SetRasterizerState(L"WireFrame");
	}
	break;
	default:
		return false;
	}
	return true;
}
