#pragma once
#include"ReferenceBase.h"
#include"TypeBase.h"
#include"NameBase.h"

class CGameObject : public CRefBase, public CTypeBase , public CNameBase
{
private:
	bool								m_bActive;
	bool								m_bDeath;
	bool								m_bDebug;


public:
	virtual void On() { m_bActive = true; }
	virtual void Off() { m_bActive = false; }
	virtual void DebugOn() { m_bDebug = true; }
	virtual void DebugOff() { m_bDebug = false; }
	void SetActive(const bool& _Active) { m_bActive = _Active; }
		
public:
	virtual void Death()
	{
		m_bDeath = true; 
	}
	virtual const bool IsActive() 
	{ 
		return m_bActive  && !m_bDeath; 
	}
	virtual const bool IsDeath()
	{
		return m_bDeath; 
	}
	virtual const bool IsDebug()
	{
		return m_bDebug;
	}


public:
	CGameObject();
	virtual ~CGameObject() = 0;
};

