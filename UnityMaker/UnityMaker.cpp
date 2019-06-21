#include <stdio.h>
#include <io.h>
#include <conio.h>
#include<iostream>
#include <Windows.h>
#include <string>
#include <atlstr.h>
#include<algorithm>

#define MAXSTRING 512
#define MAXPATH 256

using namespace std;


// 1번째 매개변수 : 출력 디렉토리
// 2번째 ~		  : CPP 파일이 있는 디렉토리

const bool CheckMakeUnityFile(const std::string& _Dir, const std::string& _CppFile);

int main(int _argc, char* _argv[])
{
	if (_argc == 1) 
	{
		cout << "No Argument Error" << endl;
		return -1; 
	}
	
	string UnityCppOutDir = _argv[1];
	UnityCppOutDir += "\\";

	string UnityCppDir;
	string CppFileName;

	//size_t Index = test.find(":");

	//CppFileName = test.substr(Index + 1, test.size() - Index - 1);
	//UnityCppDir = test.substr(0, Index);

	//if (true == CheckMakeUnityFile(UnityCppDir, "..\\TestClient_Unity\\" + CppFileName))
	//	int a = 0;

	for (int i = 2; i < _argc; i++)
	{
		FILE*	fp = NULL;
		UnityCppDir = _argv[i];
		size_t Index = UnityCppDir.find(":");
		if (-1 == Index) 
		{
			cout << "Argument" << i << " Error" << endl;
			continue;
		}

		CppFileName = UnityCppDir.substr(Index + 1, UnityCppDir.size() - Index - 1);
		UnityCppDir = UnityCppDir.substr(0,Index);

		if (false == CheckMakeUnityFile(UnityCppDir, UnityCppOutDir + CppFileName))
		{
			cout << "Non - Modification Unity Cpp File : " << CppFileName << endl;
			continue;
		}

		fopen_s(&fp, (_argv[1] + ("\\" + CppFileName)).c_str(), "wt");
		
		if (nullptr == fp) 
		{
			cout << "File Open Error : " << CppFileName << endl;
			continue;
		}

		_finddata_t fd;
		intptr_t handle;
		int result = 1;
		string FileName;
		string Path = UnityCppDir;
		Path += "\\*.cpp";
		handle = _findfirst(Path.c_str(), &fd); 

		while (result != -1)
		{
			FileName = fd.name;
			FileName = "#include \"" + FileName + "\"\n";
			fwrite(FileName.c_str(), FileName.size(), 1, fp);
			result = _findnext(handle, &fd);
		}
		_findclose(handle);

		cout << "Create Unity Cpp File : " << CppFileName << endl;
		fclose(fp);
	}

	cout << "Complete Make Unity Cpp File" << endl;

	return 0;
}

const bool CheckMakeUnityFile(const std::string& _Dir, const std::string& _CppPath) 
{
	_finddata_t UnityCppfd;
	intptr_t UnityHandle;
	UnityHandle = _findfirst(_CppPath.c_str(), &UnityCppfd);
	
	if (-1 == UnityHandle) 
	{
		_findclose(UnityHandle);
		return true;
	}

	_finddata_t fd;
	intptr_t handle;
	int result = 1;
	string FileName;
	string Path = _Dir;
	Path += "\\*.cpp";
	handle = _findfirst(Path.c_str(), &fd);


	while (result != -1)
	{
		if (fd.time_write > UnityCppfd.time_write)
			return true;

		result = _findnext(handle, &fd);

	}

	_findclose(UnityHandle);
	_findclose(handle);
	return false;
}