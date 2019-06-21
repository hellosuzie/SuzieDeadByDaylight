#include "TraceScript.h"
#include <TransForm.h>
#include <Actor.h>
#include <OBBCollider.h>
#include <Gizmo.h>
#include <Actor.h>
#include <Light.h>

CTraceScript::CTraceScript()
	: m_TraceTrans(nullptr), m_TracePivot(Vec3())
{
}


CTraceScript::~CTraceScript()
{
}


const bool CTraceScript::Init()
{
	m_CameraGizmo = ACTOR->AddCom<CGizmo>(RENDATA{ RENDER_LAYER_DEBUG });
	m_CameraGizmo->SetTransCallBack(this, &CTraceScript::GizmoTransEvent);

	return true;
}

void CTraceScript::Update(const float& _DeltaTime) 
{

}
void CTraceScript::PostUpdate(const float& _DeltaTime) 
{
	if (nullptr == m_TraceTrans)
		return;

	TRANS->SetPos(m_TraceTrans->GetWorldPos() + m_TracePivot);
}

void CTraceScript::SetTraceTrans(SPTR<CTransForm> _pTrans, const Vec3& _Pivot /*= Vec3()*/)
{
	m_TraceTrans = _pTrans;
	m_TracePivot = _Pivot;
	TRANS->SetPos(m_TraceTrans->GetWorldPos() + m_TracePivot );
}


void CTraceScript::GizmoTransEvent(const Vec3& _Dir)
{
	m_TracePivot += _Dir;
}