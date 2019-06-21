#include "WriteStream.h"



CWriteStream::CWriteStream(const wchar_t* _Path) : CFileStream(_Path, L"wb")
{
}


CWriteStream::~CWriteStream()
{
}

void CWriteStream::Write(void * _WBuf, int _ByteSize)
{
	CFileStream::Write(_WBuf, _ByteSize);
}
