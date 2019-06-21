#include "ReferenceBase.h"



CReferenceBase::CReferenceBase()
	:m_iRefCount(0)
{
	
}

CReferenceBase::CReferenceBase(const CReferenceBase& _Other)
	: m_iRefCount(0)
{

}

CReferenceBase::~CReferenceBase()
{
}

void CReferenceBase::AddRefCount()
{
	++m_iRefCount;
}
void CReferenceBase::ReleaseRef()
{
	--m_iRefCount;

	if (0 >= m_iRefCount)
		delete this;
}