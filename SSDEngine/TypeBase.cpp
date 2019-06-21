#include "TypeBase.h"



CTypeBase::CTypeBase()
	:m_TypeInfo(nullptr)
{

}
CTypeBase::CTypeBase(const CTypeBase& _Other)
	:m_TypeInfo(_Other.m_TypeInfo)
{

}

CTypeBase::~CTypeBase()
{
}
