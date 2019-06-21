#pragma once
#include<string>
#include<set>


class CNameBase
{
private:
	friend class Engine;

private:
	std::wstring									m_Name;
	std::set<std::wstring>							m_setTag;
	

public:
	void SetName(const wchar_t* _Name) { m_Name = _Name; }
	void SetName(const std::wstring& _Name) 
	{
		m_Name = _Name; 
	}

public:
	std::wstring GetName() { return m_Name; }
	const wchar_t* GetName_str() { return m_Name.c_str(); }

public:
	void AddTag(const wchar_t* __Tag);
	void DelTag(const wchar_t* _Tag);
	const bool ExistTag(const wchar_t* _Tag);



public:
	CNameBase();
	CNameBase(const CNameBase& _Other);
	virtual ~CNameBase() = 0;
};

