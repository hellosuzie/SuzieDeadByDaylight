#include "CircleCollider.h"
#include"DXStruct.h"
#include"VtxShader.h"
#include"PixShader.h"
#include"Mesh.h"
#include"Camera.h"
#include"Scene.h"
#include "Material.h"


CCircleCollider::CCircleCollider()
{
	m_pFi = new BASE2D_FI(CT_CIRCLE);
}


CCircleCollider::~CCircleCollider()
{
}


void CCircleCollider::DebugRender()
{
	SPTR<CCamera> SceneCam = MAINCAM;

	if (nullptr == SceneCam)
		return;

	CollisionSyncro();
	SubTransUpdate();

	CBUFFER::Base2DBuffer BaseBuffer;


	if (m_bCollision)
		BaseBuffer = CBUFFER::Base2DBuffer(Vec4::Red.vec3, BASE2D_CIRCLE, Vec4(1.f / m_ColSize.x, 1.f / m_ColSize.y, m_fBorderSize));
	else
		BaseBuffer = CBUFFER::Base2DBuffer(m_DebugColor, BASE2D_CIRCLE, Vec4(1.f / m_ColSize.x, 1.f / m_ColSize.y, m_fBorderSize));

	//CCollider::Collider2D_Mesh->Update();
	//CCollider::Collider2D_VS->Update();
	
	//CCollider::Collider2D_PS->Update();
	//CCollider::Collider2D_VS->BufferUpdate(0, &BaseBuffer);
	//CCollider::Collider2D_PS->BufferUpdate(0, &BaseBuffer);

	//CCollider::Collider2D_Mesh->Render();

	BaseBuffer.WVP = (GetSubTransCRWorld() * SceneCam->GetVPMatrix()).GetTransposeMat();
	Collider2D_Mesh->Update();
	Collider2D_Material->SetConstantBufferData(L"MainBuffer", BaseBuffer);
	Collider2D_Material->Update();

	Collider2D_Mesh->Render(0);

}
