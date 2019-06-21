#pragma once

class CTypeBase
{
private:
	friend class Engine;
	friend class CCamera;

public:
	const type_info*					m_TypeInfo;

private:
	void TypeInit() { SetTypeInfo(this); }
	
	template<typename T>
	void SetTypeInfo(T* _Info)
	{
		if (nullptr == _Info)
			return;

		m_TypeInfo = &typeid(*_Info);
	}

	template<typename T>
	void SetTypeInfo()
	{
		m_TypeInfo = &typeid(T);
	}

public:
	template<typename T>
	const bool EqualType()
	{
		return m_TypeInfo == &typeid(T);
	}

public:
	template<typename T>
	static void InitTypeInfo(T* _Obj) 
	{
		_Obj->SetTypeInfo(_Obj);
	}

public:
	const type_info* GetTypeInfo() { return  m_TypeInfo; }

public:
	CTypeBase();
	CTypeBase(const CTypeBase& _Other);
	virtual ~CTypeBase() = 0;
};