#include "RectCollider.h"
#include "DXStruct.h"
#include "VtxShader.h"
#include "PixShader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Scene.h"
#include "Material.h"




CRectCollider::CRectCollider()
{
	m_pFi = new BASE2D_FI(CT_RECT);
}


CRectCollider::~CRectCollider()
{

}

void CRectCollider::DebugRender()
{
	SPTR<CCamera> SceneCam = MAINCAM;

	if (nullptr == SceneCam)
		return;

	CollisionSyncro();
	SubTransUpdate();
	
	CBUFFER::Base2DBuffer BaseBuffer;


	if(m_bCollision)
		BaseBuffer = CBUFFER::Base2DBuffer(Vec4::Red.vec3, BASE2D_RECT,Vec4(1.f/m_ColSize.x, 1.f / m_ColSize.y, m_fBorderSize));
	else 
		BaseBuffer = CBUFFER::Base2DBuffer(m_DebugColor, BASE2D_RECT, Vec4(1.f / m_ColSize.x, 1.f / m_ColSize.y, m_fBorderSize));

	BaseBuffer.WVP = (GetSubTransCRWorld() * SceneCam->GetVPMatrix()).GetTransposeMat();
	
	Collider2D_Mesh->Update();
	Collider2D_Material->SetConstantBufferData(L"MainBuffer", BaseBuffer);
	Collider2D_Material->Update();

	Collider2D_Mesh->Render();

}	
