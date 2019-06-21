#pragma once

#define INITUSER 5000
#define MAXCRITICALTIME 20.f
#define GAMEUPDATETIME 0.016666667f

#define ENDSIGNAL 0

#define DELETE_POINTER(P) if(nullptr != P){ delete P; P = nullptr;}
#define DELETE_ARRPOINTER(P) if(nullptr != P){delete[] P; P = nullptr;}\



#define DeductionPointer(ReturnPointerType, InputPointer, Member) \
reinterpret_cast<ReturnPointerType>(reinterpret_cast<char*>(InputPointer) - reinterpret_cast<ULONG_PTR>(&reinterpret_cast<ReturnPointerType>(0)->Member));
