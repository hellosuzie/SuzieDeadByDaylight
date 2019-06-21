#include "Skybox.h"
#include "Material.h"
#include "TransForm.h"
#include  "Camera.h"

CSkybox::CSkybox()
{

}


CSkybox::~CSkybox()
{
}


const bool CSkybox::Init(const RENDATA& _InitData)
{
	PushMesh(L"Sphere_Lv8");
	PushMaterial(L"SkyboxCube");
	//PushMaterial(L"Skybox");
	m_vecMaterial[0]->PushSampler(0, LINEARSAMPLER);
	
	SetRenSize(Vec3(10000.f, 10000.f, 10000.f));

	return CRenderer::Init(_InitData);
}

void CSkybox::SetSkyBoxSize(const float& _Size)
{
	SetRenSize(Vec3(_Size, _Size, _Size));
}

void CSkybox::SetTexture(const wchar_t* _TexKey)
{
	m_vecMaterial[0]->PushTexture(0, _TexKey, SHADER_PIXEL);
}
void CSkybox::RenderUpdate(SPTR<class CCamera> _Cam)
{
	TRANS->SetPos(_Cam->GetPos());
	CRenderer::RenderUpdate(_Cam);
}
