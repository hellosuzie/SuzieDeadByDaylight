#include "Gizmo.h"
#include "Camera.h"
#include "Actor.h"
#include "Material.h"
#include "KeyMgr.h"
#include "TransForm.h"

CGizmo::CGizmo()
	: m_eCurPickGizmo(PICK_NONE), m_HoldGizmoDir(Vec3())
	,m_bGizmoHolding(false), m_eTransMode(GIZMO_TRANS)
	, m_TransCallBack(nullptr)
{
	ZeroMemory(m_GizmoCol, sizeof(SPTR<COBBCollider>));
}


CGizmo::~CGizmo()
{
}

void CGizmo::SetTransMode() 
{
	m_eTransMode = GIZMO_TRANS;

	m_GizmoCol[AXIS_X]->SetDebugColor(Vec4::Green.vec3);
	m_GizmoCol[AXIS_Y]->SetDebugColor(Vec4::Green.vec3);
	m_GizmoCol[AXIS_Z]->SetDebugColor(Vec4::Green.vec3);
}

void CGizmo::SetRotateMode()
{
	m_eTransMode = GIZMO_ROTATE;

	m_GizmoCol[AXIS_X]->SetDebugColor(Vec3(0.1f, 0.4f, 0.8f));
	m_GizmoCol[AXIS_Y]->SetDebugColor(Vec3(0.1f, 0.4f, 0.8f));
	m_GizmoCol[AXIS_Z]->SetDebugColor(Vec3(0.1f, 0.4f, 0.8f));
}

void CGizmo::ChangeGizmoMode()
{
	m_eTransMode = (GIZMO_TRANSMODE)((m_eTransMode + 1) % MAX_GIZMO_TRANSMODE);

	switch (m_eTransMode)
	{
	case GIZMO_TRANS:
		SetTransMode();
		break;
	case GIZMO_ROTATE:
		SetRotateMode();
		break;
	default:
		TASSERT(true)
		return;
	}
}
const bool CGizmo::Init(const RENDATA& _InitData)
{
	PushMesh(L"GizmoMesh");
	PushMaterial(L"Gizmo");

	AddTag(L"NonePrefab");
	
	m_GizmoCol[AXIS_X] = ACTOR->AddCom<COBBCollider>(COLDATA{ L"Gizmo",COLLISION_3D });
	m_GizmoCol[AXIS_X]->SetColFi(Vec3(GIZMOHEIGHT * 0.5f + 10.f, 0.f, 0.f), Vec3(GIZMOHEIGHT, GIZMOSIZE, GIZMOSIZE));
	m_GizmoCol[AXIS_X]->SetBorderSize(8.f);
	m_GizmoCol[AXIS_X]->AddTag(L"NonePrefab");

	m_GizmoCol[AXIS_Y] = ACTOR->AddCom<COBBCollider>(COLDATA{ L"Gizmo",COLLISION_3D });
	m_GizmoCol[AXIS_Y]->SetColFi(Vec3( 0.f, GIZMOHEIGHT * 0.5f + 10.f, 0.f), Vec3(GIZMOSIZE, GIZMOHEIGHT, GIZMOSIZE));
	m_GizmoCol[AXIS_Y]->SetBorderSize(8.f);
	m_GizmoCol[AXIS_Y]->AddTag(L"NonePrefab");

	m_GizmoCol[AXIS_Z] = ACTOR->AddCom<COBBCollider>(COLDATA{ L"Gizmo",COLLISION_3D });
	m_GizmoCol[AXIS_Z]->SetColFi(Vec3(0.f, 0.f, GIZMOHEIGHT * 0.5f + 10.f), Vec3(GIZMOSIZE, GIZMOSIZE, GIZMOHEIGHT));
	m_GizmoCol[AXIS_Z]->SetBorderSize(8.f);
	m_GizmoCol[AXIS_Z]->AddTag(L"NonePrefab");

	m_GizmoCol[AXIS_X]->SetEnterCallBack(this, &CGizmo::PickGizmo);
	m_GizmoCol[AXIS_Y]->SetEnterCallBack(this, &CGizmo::PickGizmo);
	m_GizmoCol[AXIS_Z]->SetEnterCallBack(this, &CGizmo::PickGizmo);

	m_GizmoCol[AXIS_X]->SetExitCallBack(this, &CGizmo::PickEndGizmo);
	m_GizmoCol[AXIS_Y]->SetExitCallBack(this, &CGizmo::PickEndGizmo);
	m_GizmoCol[AXIS_Z]->SetExitCallBack(this, &CGizmo::PickEndGizmo);

	SetTransMode();

	return CRenderer::Init(_InitData);
}	
void CGizmo::PreUpdate(const float& _DeltaTime)
{
	if (true == KEYDOWN("GizmoMode")) 
	{
		ChangeGizmoMode();
	}

	if (true == KEY("GizmoHold"))
	{
		if (m_eCurPickGizmo < PICK_NONE)
		{
			m_bGizmoHolding = true;
			m_HoldGizmoDir = TRANS->GetWorldAxis((AXIS_TYPE)m_eCurPickGizmo);
			m_HoldingAxis = (AXIS_TYPE)m_eCurPickGizmo;
		}
	}

	if (true == KEYUP("GizmoHold"))
	{
		m_bGizmoHolding = false;
	}
}

void CGizmo::Update(const float& _DeltaTime)
{
	if (true == m_bGizmoHolding) 
	{
		switch (m_eTransMode)
		{
		case GIZMO_TRANS:
			PositionGizmoUpdate();
			break;
		case GIZMO_ROTATE:
			RotationGizmoUpdate();
			break;
		default:
			break;
		}
	}
}

void CGizmo::PositionGizmoUpdate() 
{
	Vec3 MouseDir = CKeyMgr::GetMouseDir();
	MouseDir.y = -MouseDir.y;

	Vec3 ViewGizmoDir = m_HoldGizmoDir * MAINCAM->GetViewMatrix();

	float Dist = DXMath::Vector3Dot(ViewGizmoDir, MouseDir);

	if (nullptr == m_TransCallBack)
	{
		TRANS->Move(m_HoldGizmoDir * Dist);
	}
	else
	{
		m_TransCallBack(m_HoldGizmoDir * Dist);
	}
}
void CGizmo::RotationGizmoUpdate() 
{
	Vec3 MouseDir = CKeyMgr::GetMouseDir();
	MouseDir.y = -MouseDir.y;

	Vec3 ViewGizmoDir = m_HoldGizmoDir * MAINCAM->GetViewMatrix();

	float Angle = DXMath::Vector3Dot(ViewGizmoDir, MouseDir);

	TRANS->AccRotate(m_HoldingAxis, Angle);

}
void CGizmo::RenderUpdate(SPTR<class CCamera> _Camera)
{
	SubTransUpdate();

	Mat World = GetSubTransCRWorld();
	World.Row[0].Normalize();
	World.Row[1].Normalize();
	World.Row[2].Normalize();

	m_GizmoBuff.GizmoMat = ( World * _Camera->GetVPMatrix()).GetTransposeMat();
	GetMaterial(0)->SetConstantBufferData(L"Gizmo", m_GizmoBuff);
	CRenderer::RenderUpdate(_Camera);
}


const GIZMO_PICKTYPE CGizmo::GetGizmoIdx(SPTR<CCollider> _PickCol)
{
	GIZMO_PICKTYPE Type = PICK_NONE;
	
	for (UINT i = 0; i < PICK_NONE; i++)
	{
		if (_PickCol == m_GizmoCol[i])
		{
			Type = (GIZMO_PICKTYPE)i;
			break;
		}
	}

	return Type;
}


void CGizmo::PickGizmo(SPTR<class CCollider> _This, SPTR<class CCollider> _Other)
{
	if (m_bGizmoHolding)
		return;

	if (m_eCurPickGizmo >= PICK_NONE) 
	{
		m_eCurPickGizmo = GetGizmoIdx(_This);
		m_setPick.insert(m_eCurPickGizmo);
	}
	else 
	{
		GIZMO_PICKTYPE Pick = GetGizmoIdx(_This);
	
		if (m_GizmoCol[m_eCurPickGizmo]->GetRayCollisionLength() >= m_GizmoCol[Pick]->GetRayCollisionLength()) 
		{
			m_eCurPickGizmo = Pick;
		}

		m_setPick.insert(Pick);
	}
}

void CGizmo::PickEndGizmo(SPTR<class CCollider> _This, SPTR<class CCollider> _Other)
{
	GIZMO_PICKTYPE Pick = GetGizmoIdx(_This);
	m_setPick.erase(Pick);

	if (true == m_setPick.empty())
	{
		m_eCurPickGizmo = PICK_NONE;
		return;
	}

	if (Pick == m_eCurPickGizmo) 
	{
		float Len = D3D11_FLOAT32_MAX;
		for (const GIZMO_PICKTYPE& i : m_setPick)
		{
			if (Len > 0.f &&  Len > m_GizmoCol[i]->GetRayCollisionLength())
			{
				m_eCurPickGizmo = i;
			}
		}
	}
}

void CGizmo::SetGizmoPivot(const Vec3& _Pivot)
{
	SetRenPivot(_Pivot);
	m_GizmoCol[AXIS_X]->SetColPivot(_Pivot+ Vec3(GIZMOHEIGHT * 0.5f + 10.f, 0.f, 0.f));
	m_GizmoCol[AXIS_Y]->SetColPivot(_Pivot + Vec3(0.f, GIZMOHEIGHT * 0.5f + 10.f, 0.f));
	m_GizmoCol[AXIS_Z]->SetColPivot(_Pivot +Vec3(0.f, 0.f, GIZMOHEIGHT * 0.5f + 10.f));
}

void CGizmo::GizmoOn()
{
	On();
	
	for (size_t i = 0; i < 3; ++i)
	{
		m_GizmoCol[i]->On();
		m_GizmoCol[i]->ObbDebugOn();
	}
}

void CGizmo::GizmoOff()
{
	Off();

	for (size_t i = 0; i < 3; ++i)
	{
		m_GizmoCol[i]->Off();
		m_GizmoCol[i]->ObbDebugOff();
	}
}
