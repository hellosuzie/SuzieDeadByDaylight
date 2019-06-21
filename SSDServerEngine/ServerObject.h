#pragma once
#include <string>
#include <set>

class CServerObject
{
private:
	std::wstring					m_Name;
	std::set<std::wstring>			m_setTag;


public:
	void SetName(const wchar_t* _Name) { m_Name = _Name; }
	void AddTag(const wchar_t* _Tag) 
	{
		if (m_setTag.end() != m_setTag.find(_Tag))
			return;

		m_setTag.insert(_Tag);
	}

public:
	const std::wstring GetName() { return m_Name; }
	const bool ExistTag(const wchar_t* _Tag) 
	{
		std::set<std::wstring>::iterator Find = m_setTag.find(_Tag);

		if (Find == m_setTag.end())
			return false;

		return true;
	}


public:
	CServerObject();
	~CServerObject();
};

