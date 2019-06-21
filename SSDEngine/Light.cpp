#include "Light.h"
#include "Scene.h"
#include "Mesh.h"
#include "Material.h"
#include "TransForm.h"
#include "Camera.h"
#include "Device.h"
#include "DebugRenderer.h"

CLight::CLight()
	:m_LightMesh(nullptr), m_LightMaterial(nullptr), m_LightBufferData(CBUFFER::LightBuffer())
	, m_bBufferUpdate(false), m_VolumeMesh(nullptr), m_VolumeMaterial(nullptr), m_VolumeCheck(false), m_DebugRen(nullptr)
{
}

CLight::~CLight()
{

}

const bool CLight::Init(const LIGHT_DATA& _InitData)
{
	m_LightBufferData.LightType = _InitData.Type;
	m_VolumeCheck = _InitData.VolumeCheck;

	SetSubPivot(Vec3(0.f, 0.f, 0.f));
	SetSubSize(Vec3(1.f, 1.f, 1.f));

	if (true == m_VolumeCheck)
	{
		if (L"" != _InitData.MeshName)
		{
			m_VolumeMesh = CResourceMgr<CMesh>::Find(_InitData.MeshName);
			m_VolumeMaterial = CResourceMgr<CMaterial>::Find(L"VolumeLight");

			if (L"Cone_Lv360" == _InitData.MeshName)
			{
				m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG, DF_CONE });
			}
			else if (L"Cube" == _InitData.MeshName)
			{
				m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG, DF_CUBE });
			}
			else
			{
				m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG, DF_SPHERE });
			}

			m_DebugRen->SetRenPivot(m_pSubData->SubPivot);
			m_DebugRen->SetRenSize(m_pSubData->SubSize);
		}
	}
	m_LightMesh = CSceneRenderMgr::pDefferredMesh;
	m_LightMaterial = CSceneRenderMgr::pDefferredMaterial->Clone();

	m_LightMaterial->SetConstantBufferData(L"LightBuffer", m_LightBufferData);

	m_LightData = _InitData;

	return m_pBelongScene->GetRenderMgr()->PushLight(this);
}

void CLight::PostUpdate(const float& _DeltaTime)
{
	if (m_LightBufferData.LightType != DIRECTION_LIGHT && true == TRANS->ChangeTransform())
	{
		m_bBufferUpdate = true;
	}
}

void CLight::Lighting(SPTR<class CCamera> _Camera)
{
	if (nullptr != m_DebugRen)
	{
		m_DebugRen->SetRenPivot(m_pSubData->SubPivot);
		m_DebugRen->SetRenSize(m_pSubData->SubSize);
	}

	//if (true == TRANS->ChangeTransform() || true == _Camera->TRANS->ChangeTransform())
	//{
		m_LightBufferData.LightPos.vec3 = TRANS->GetWorldPos();
		m_LightBufferData.LightPos.w = 1.f; 
		//m_LightBufferData.LightDir = m_LightDir;
		m_LightBufferData.LightDir = TRANS->GetDown();
		m_LightBufferData.LightInvDir = -m_LightBufferData.LightDir;

		m_LightBufferData.ViewMat = _Camera->GetViewMatrix().GetTransposeMat();
		m_LightBufferData.ProjMat = _Camera->GetProjMatrix().GetTransposeMat();
		m_LightBufferData.VP = _Camera->GetVPMatrix().GetTransposeMat();

		m_LightBufferData.LightViewPos = m_LightBufferData.LightPos *_Camera->GetViewMatrix();
		m_LightBufferData.LightViewDir = (m_LightBufferData.LightDir * _Camera->GetViewMatrix()).GetNormalizeVec();
		m_LightBufferData.LightViewInvDir = (m_LightBufferData.LightInvDir *_Camera->GetViewMatrix()).GetNormalizeVec();
		m_LightBufferData.LightRange = m_pSubData->SubSize.y;

		m_LightBufferData.SpotPower = TriangletoExponent(m_pSubData->SubSize.x, m_pSubData->SubSize.y);

		m_LightMaterial->SetConstantBufferData(L"LightBuffer", m_LightBufferData);
		m_bBufferUpdate = false;
	//}

	m_LightDir = m_LightBufferData.LightDir;

	if (true == m_VolumeCheck)
	{
		SubTransUpdate();
		VolumeLighting(_Camera);
	}
	m_LightMesh->Update();
	m_LightMaterial->Update();
	m_LightMesh->Render();
	m_LightMaterial->TexSamRollBack();
	m_LightMaterial->StateRollBack();

	CDevice::SetDS(L"PassDepth", 0);
	CDevice::SetDisableDS();
}

void CLight::VolumeLighting(SPTR<class CCamera> _Camera)
{
	CBUFFER::RenBaseBuffer m_BaseBuffer;

	m_BaseBuffer.World = GetSubTransCRWorld().GetTransposeMat();
	m_BaseBuffer.InvWorld = m_BaseBuffer.World.GetInverseMat().GetTransposeMat();
	m_BaseBuffer.NormalWorld = GetSubTransCRWorld().GetInverseMat();

	m_BaseBuffer.View = _Camera->GetViewMatrix().GetTransposeMat();
	m_BaseBuffer.Proj = _Camera->GetProjMatrix().GetTransposeMat();
	m_BaseBuffer.WV = m_BaseBuffer.View * m_BaseBuffer.World;
	m_BaseBuffer.NormalWV = m_BaseBuffer.WV.GetInverseMat().GetTransposeMat();

	m_BaseBuffer.VP = m_BaseBuffer.Proj * m_BaseBuffer.View;
	m_BaseBuffer.WVP = m_BaseBuffer.Proj * m_BaseBuffer.WV;
	m_BaseBuffer.CamWorldPos = _Camera->GetTrans()->GetWorldPos();

	if (nullptr != CDevice::RenBaseCB)
	{
		CDevice::RenBaseCB->Update(m_BaseBuffer);
		CDevice::RenBaseCB->SupplyShader();
	}

	CDevice::SetRS(L"FrontCulling");
	CDevice::SetDS(L"BackDepthExtraction", 1);
	m_VolumeMaterial->ShaderUpdate();
	m_VolumeMesh->Update();
	m_VolumeMesh->Render();

	CDevice::SetRS(L"BackCulling");
	CDevice::SetDS(L"FrontDepthExtraction", 1);
	m_VolumeMaterial->ShaderUpdate();
	m_VolumeMesh->Update();
	m_VolumeMesh->Render();

	CDevice::SetDS(L"PassDepth", 1);
}

void CLight::SetVolumeLightMesh(const std::wstring& _MeshName)
{
	if (L"" != _MeshName)
	{
		m_VolumeMesh = CResourceMgr<CMesh>::Find(_MeshName);

		if (nullptr != m_DebugRen)
		{
			if (L"Cone_Lv360" == _MeshName)
			{
				m_DebugRen->ChangeDebugType(DEBUGFI_TYPE::DF_CONE);
			}
			else if (L"Cube" == _MeshName)
			{
				m_DebugRen->ChangeDebugType(DEBUGFI_TYPE::DF_CUBE);
			}
			else
			{
				m_DebugRen->ChangeDebugType(DEBUGFI_TYPE::DF_SPHERE);
			}
		}
	}
}

float CLight::TriangletoExponent(const float _Diameter, const float _Height)
{
	float Return = 0.f;
	float Down = log2f(_Height / sqrtf((_Diameter * _Diameter) + (_Height * _Height)));
	Return = -12 / Down;

	return Return;
}

const std::wstring CLight::GetVolumeMeshName()
{
	return m_VolumeMesh->GetPath();
}
