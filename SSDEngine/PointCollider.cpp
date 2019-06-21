#include "PointCollider.h"
#include "GlobalDebug.h"
#include "Scene.h"
#include "Camera.h"
#include "TransForm.h"





CPointCollider::CPointCollider()
{
	m_pFi = new BASE2D_FI(CT_POINT);
}


CPointCollider::~CPointCollider()
{
}


void CPointCollider::DebugRender()
{
	CollisionSyncro();
	SubTransUpdate();

	Vec3 Pos = Vec3();

	if (nullptr == m_pSubData)
		Pos = TRANS->GetWorldPos() + Vec3(0.f,0.f,-1.f);
	else
		Pos = m_pSubData->SubWorldMat.Row[3].vec3;

	GlobalDebug::DrawDebugRect(MAINCAM->GetVPMatrix(), Pos , Vec3(2.f, 2.f, -.1f), Vec3(1.f, 0.f, 0.f));

}