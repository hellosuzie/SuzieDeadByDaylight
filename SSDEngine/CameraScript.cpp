#include "CameraScript.h"
#include"DXMath.h"
#include"TransForm.h"
#include"Camera.h"




CCameraScript::CCameraScript()
	:m_pPairCamera(nullptr)
{
}


CCameraScript::~CCameraScript()
{
}

void CCameraScript::SetCamera(CCamera* _Cam)
{ 
	m_pPairCamera = _Cam; 
}
