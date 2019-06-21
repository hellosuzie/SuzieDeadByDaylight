#include "Script.h"
#include"FSMDriver.h"
#include"Engine.h"
#include"TransForm.h"
#include"Scene.h"

CScript::CScript()
{
}
CScript::CScript(CActor* _Actor)
	: CBaseComponent(_Actor)
{

}

CScript::~CScript()
{
}

void CScript::CreateFSM()
{
	m_FSMDriver = Engine::CreateGameObject<CFSMDriver>();
}

const bool CScript::Init()
{
	return SCENE->PushScript(this);
}
void CScript::Update(const float& _DeltaTime)
{
	if (nullptr != m_FSMDriver)
		m_FSMDriver->Update(_DeltaTime);
}
void CScript::LastUpdate(const float& _DeltaTime)
{
	int b = 0;
}

void CScript::SetSize(const Vec3& _Pos) 
{
	TRANS->SetPos(_Pos);
}
void CScript::SetPos(const Vec3& _Size) 
{
	TRANS->SetScale(_Size);
}
void CScript::ActorMove(const Vec3& _MoveVec,const float& _DeltaTime)
{
	TRANS->Move(_MoveVec, _DeltaTime);
}
CTransForm* CScript::GetScriptTrans()
{
	return TRANS;
}
CActor* CScript::GetScriptActor()
{
	return ACTOR;
}
CActor * CScript::CreateActor(const ACTOR_UPDATE_LAYER& _Layer, const std::wstring& _Name)
{
	if (nullptr == m_pBelongScene)
		return nullptr;

	return m_pBelongScene->CreateActor(m_pBelongScene, _Layer, _Name);
}