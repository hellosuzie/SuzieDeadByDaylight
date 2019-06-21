#include "FrustumCollider.h"
#include "Actor.h"
#include "TransForm.h"
#include "DebugRenderer.h"
#include "Camera.h"


CFrustumCollider::CFrustumCollider()
	:m_FrustumCamera(nullptr)
{
	m_pFi = new FRUSTUM_FI();
}

CFrustumCollider::~CFrustumCollider()
{
}


const bool CFrustumCollider::Init(const COLDATA& _InitData)
{
	m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG, DF_FRUSTUM });

	return CCollider::Init(_InitData);
}
const bool CFrustumCollider::Init(const COL3D_DATA& _InitData)
{
	if (true == _InitData.DebugRen)
	{
		m_DebugRen = ACTOR->AddCom<CDebugRenderer>(DEBUG_RENDATA{ RENDER_LAYER_DEBUG, DF_FRUSTUM });
	}

	return CCollider::Init(_InitData.ColData);
}
void CFrustumCollider::PostUpdate(const float& _DeltaTime)
{

	SupplyFrustumMatrix();
}

void CFrustumCollider::SupplyFrustumMatrix()
{
	Mat View = m_FrustumCamera->GetViewMatrix();


	m_DebugRen->SetFrustumMat((View*m_FrustumProj).GetInverseMat());
}
void CFrustumCollider::CollisionSyncro()
{
	FRUSTUM_FI*pFi = reinterpret_cast<FRUSTUM_FI*>(m_pFi);

	pFi->FrustumFi.Origin = TRANS->GetWorldPos();
	pFi->FrustumFi.Orientation = TRANS->GetCRWorldMat().GetAxisQuaternion().XMF4;

}
void CFrustumCollider::SetFrustumFi(SPTR<CCamera> _Cam, const float& FovyAngle, const float& Aspect, const float& _Near, const float& _Far)
{
	m_FrustumCamera = _Cam;

	FRUSTUM_FI*pFi =  reinterpret_cast<FRUSTUM_FI*>(m_pFi);

	m_FrustumProj = DirectX::XMMatrixPerspectiveFovLH(FovyAngle * D2R, Aspect, _Near, _Far);
	pFi->FrustumFi = DirectX::BoundingFrustum(m_FrustumProj);

	SupplyFrustumMatrix();
}

