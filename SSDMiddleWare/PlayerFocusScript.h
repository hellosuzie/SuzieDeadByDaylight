#pragma once
#include <CameraScript.h>


class CPlayerFocusScript : public CCameraScript
{
private:
	SPTR<class CPlayer>						m_TracePlayer;

public:
	void SetFocusingPlayer(SPTR<CPlayer> _pPlayer);

public:
	CPlayerFocusScript();
	~CPlayerFocusScript();
};

