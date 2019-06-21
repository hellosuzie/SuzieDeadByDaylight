#include "NameBase.h"



CNameBase::CNameBase()
{

}

CNameBase::CNameBase(const CNameBase& _Other)
	:m_Name(_Other.m_Name)
{
	m_setTag.insert(_Other.m_setTag.begin(), _Other.m_setTag.end());
}
CNameBase::~CNameBase()
{
}


void CNameBase::AddTag(const wchar_t* _Tag) 
{
	auto Iter = m_setTag.find(_Tag);

	if (Iter == m_setTag.end()) 
		m_setTag.insert(_Tag);
	
}
void CNameBase::DelTag(const wchar_t* _Tag) 
{
	auto Iter = m_setTag.find(_Tag);

	if (Iter != m_setTag.end())
		m_setTag.erase(_Tag);

}
const bool CNameBase::ExistTag(const wchar_t* _Tag) 
{
	auto Iter = m_setTag.find(_Tag);

	if (Iter != m_setTag.end())
		return true;
		
	return false;
}