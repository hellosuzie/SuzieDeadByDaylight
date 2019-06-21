#include "Resource.h"
#include"DXMacro.h"



CResource::CResource()
{
}
CResource::CResource(const CResource& _Other)
	: CRefBase(_Other),CTypeBase(_Other),CNameBase(_Other),m_Path(_Other.m_Path)
{

}

CResource::~CResource()
{
}

std::wstring	CResource::GetFullFileName()
{
	wchar_t ArrDrive[MAXSTRING];
	wchar_t ArrFolder[MAXSTRING];
	wchar_t ArrFile[MAXSTRING];
	wchar_t ArrExt[MAXSTRING];

	_wsplitpath_s(m_Path.c_str(), ArrDrive, ArrFolder, ArrFile, ArrExt);

	std::wstring FullFileName = ArrFile;
	FullFileName += ArrExt;

	return FullFileName;
}
std::wstring	CResource::GetFileName() 
{
	wchar_t ArrDrive[MAXSTRING];
	wchar_t ArrFolder[MAXSTRING];
	wchar_t ArrFile[MAXSTRING];
	wchar_t ArrExt[MAXSTRING];

	_wsplitpath_s(m_Path.c_str(), ArrDrive, ArrFolder, ArrFile, ArrExt);

	return ArrFile;
}
std::wstring	CResource::GetExtension() 
{
	wchar_t ArrDrive[MAXSTRING];
	wchar_t ArrFolder[MAXSTRING];
	wchar_t ArrFile[MAXSTRING];
	wchar_t ArrExt[MAXSTRING];

	_wsplitpath_s(m_Path.c_str(), ArrDrive, ArrFolder, ArrFile, ArrExt);

	return ArrExt;
}