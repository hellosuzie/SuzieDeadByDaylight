#include "FilePathMgr.h"
#include<atlstr.h>

CFilePathMgr::CPathMgrDestroyer										CFilePathMgr::m_PathMgrDest;
wchar_t																CFilePathMgr::m_DirPath[MAXPATH] = L"";
std::wstring														CFilePathMgr::m_RootPath = L"";
std::unordered_map<std::wstring, std::wstring>						CFilePathMgr::m_mapFilePath;
std::unordered_map<std::wstring, std::wstring>::iterator			CFilePathMgr::m_FindFileNameIter;

wchar_t																CFilePathMgr::StrDrive[MAXSTRING] = L"";
wchar_t																CFilePathMgr::StrFolder[MAXSTRING] = L"";
wchar_t																CFilePathMgr::StrFile[MAXSTRING] = L"";
wchar_t																CFilePathMgr::StrExt[MAXSTRING] = L"";

const BOOL CFilePathMgr::Init()
{
	GetCurrentDirectoryW(sizeof(wchar_t)*MAXPATH, m_DirPath);
	m_RootPath = m_DirPath;
	size_t Count = m_RootPath.find_last_of(L"\\");
	m_RootPath.replace(Count, m_RootPath.size(), L"\\");
	m_RootPath = m_RootPath + L"BIN\\";

	if (m_RootPath == L"") 
		return FALSE;
	
	SettingRootPath();
	return TRUE;
}

void CFilePathMgr::SettingRootPath()
{
	AddRootFolder(L"Shader", L"Shaders");
	AddRootFolder(L"Sound");
	AddRootFolder(L"Data");
	AddRootFolder(L"Texture");
	AddRootFolder(L"Fbx");
	AddRootFolder(L"Obj");
	
}
const BOOL CFilePathMgr::Release()
{
	m_mapFilePath.clear();
	return TRUE;
}
BOOL CFilePathMgr::AddRootFolder(const std::wstring& _Path)
{
	return AddRootFolder(_Path, _Path);
}
BOOL CFilePathMgr::AddRootFolder(const std::wstring& _Name,const std::wstring& _FilePath)
{
	m_FindFileNameIter = m_mapFilePath.find(_Name);
	
	if (m_FindFileNameIter != m_mapFilePath.end())
		return FALSE;
	

	std::wstring NewPath = m_RootPath + _FilePath + L"\\";

	m_mapFilePath.insert(make_pair(_Name, NewPath));
	
	return TRUE;

}
BOOL  CFilePathMgr::AddPlusFolder(const std::wstring& _PlusName,const std::wstring& _FolderKey)
{
	return AddPlusFolder(_PlusName, _FolderKey, _FolderKey);
}
BOOL  CFilePathMgr::AddPlusFolder(
	const std::wstring& _PlusName
	,const std::wstring& _FolderKey
	,const std::wstring& _FolderName)
{
	m_FindFileNameIter = m_mapFilePath.find(_PlusName);

	if (m_FindFileNameIter == m_mapFilePath.end())
	{
		return FALSE;
	}

	std::wstring NewPath = m_FindFileNameIter->second + _FolderName + L"\\";

	m_mapFilePath.insert(std::unordered_map<std::wstring, std::wstring>::value_type(_FolderKey, NewPath));

	return TRUE;

}
const std::wstring	 CFilePathMgr::GetPath(const std::wstring& _Name) 
{
	m_FindFileNameIter = m_mapFilePath.find(_Name);

	if (m_FindFileNameIter != m_mapFilePath.end())
		return m_FindFileNameIter-> second;
	
	return L"";
}
const std::string CFilePathMgr::GetSTRPath(const std::wstring& _Name)
{
	std::wstring Path = GetPath(_Name);
	return std::string(Path.begin(), Path.end());
}
 const std::wstring CFilePathMgr::GetPath(const std::wstring& _FolderKey, const std::wstring& _FileName)
{
	m_FindFileNameIter = m_mapFilePath.find(_FolderKey);

	if (m_FindFileNameIter != m_mapFilePath.end())
		return m_FindFileNameIter->second + _FileName;
	
	return L"";
}
 const std::string CFilePathMgr::GetSTRPath(const std::wstring& _FolderKey, const std::wstring& _FileName)
 {
	 std::wstring Path = GetPath(_FolderKey, _FileName);
	 return std::string(Path.begin(), Path.end());
 }
const std::wstring   CFilePathMgr::GetDirPath(const std::wstring& _Name) 
{
	m_FindFileNameIter = m_mapFilePath.find(_Name);

	if (m_FindFileNameIter != m_mapFilePath.end())
	{
		std::wstring TempPath = m_FindFileNameIter->second;
		size_t Last_Index = TempPath.find_last_of(L"\\");
		size_t Count = TempPath.size() - Last_Index;
		TempPath.replace(Last_Index, Count, L"");
		return TempPath;
	}
	return L"";
}
const std::wstring CFilePathMgr::GetPath(const wchar_t* _Str) 
{
	std::wstring a = _Str;
	m_FindFileNameIter = m_mapFilePath.find(_Str);

	if (m_FindFileNameIter != m_mapFilePath.end())
	{
		return m_FindFileNameIter->second;
	}
	return L"";
}


std::wstring	 CFilePathMgr::FilePath(const std::wstring& _Key, const std::wstring& _FileName)
{
	std::wstring Path = CFilePathMgr::GetPath(_Key);

	return Path + _FileName;

}
std::wstring	CFilePathMgr::PathToFileName(const std::wstring& _Path)
{
	std::wstring FileName = _Path; 
	size_t Last_Index = FileName.find_last_of(L"\\");
	Last_Index += 1;

	FileName.replace(0, Last_Index, L"");

	size_t First_Index = FileName.find_last_of(L".");
	size_t Count = FileName.size() - First_Index;
	FileName.replace(First_Index, Count, L"");

	return FileName;

}

const wchar_t* CFilePathMgr::GetFileName(const wchar_t* _Path)
{
	_wsplitpath_s(_Path, StrDrive, StrFolder, StrFile, StrExt);


	return StrFile;
}