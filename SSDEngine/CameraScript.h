#pragma once
#include "Script.h"



class CActor;
class CCameraScript : public CScript
{
private:
	friend class CCamera;

protected:
	CCamera*								m_pPairCamera;

public:
	// 카메라 스크립트에 카메라가 셋팅된 이후에 호출되는 초기화용 함수
	virtual void MountingCameraEvent() {}
public:
	virtual void LastCamScriptUpdate(const float& _DeltaTime) {}

public:
	void SetCamera(CCamera* _Cam);

public:
	CCameraScript();
	~CCameraScript();
};





