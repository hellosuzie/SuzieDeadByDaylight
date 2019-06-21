#pragma once
#include <Script.h>

enum DOOR_TYPE 
{
	FRONT_DOOR,
	BACK_DOOR,
	MAX_DOOR_TYPE
};
class CDoor : public CScript
{
	SPTR<class COBBCollider>								m_pDoorCol[MAX_DOOR_TYPE];

public:
	const bool Init() override;

public:
	void Update(const float& _DeltaTime) override;
	void LastUpdate(const float& _DeltaTime) override;
	void DebugRender() override;

public:
	void OpenEscapeDoor();
	void EscapeEvent(SPTR<class CCollider> _This, SPTR<class CCollider> _Other);


public:
	CDoor();
	~CDoor();
};

