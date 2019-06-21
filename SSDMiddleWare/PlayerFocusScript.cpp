#include "PlayerFocusScript.h"



CPlayerFocusScript::CPlayerFocusScript()
	:m_TracePlayer(nullptr)
{
}


CPlayerFocusScript::~CPlayerFocusScript()
{

}

void CPlayerFocusScript::SetFocusingPlayer(SPTR<CPlayer> _pPlayer)
{
	m_TracePlayer = _pPlayer;


}