#pragma once
#include<assert.h>

#define MAXPATH 512
#define MAXSTRING 256
#define TASSERT(VALUE) assert(!(VALUE));



#define NONE_CREATE_RESOURCE(RES) \
struct CREATE_DESC {}; 

#define NONE_LOAD_RESOURCE(RES) \
struct LOAD_DESC {}; 


#pragma region CREATE_DESC_DEFINITION
#define CREATE_DESC_BODY(DEF) \
struct CREATE_DESC \
{ \
	DEF \
};
#pragma endregion

#pragma region LOAD_DESC_DEFINITION
#define LOAD_DESC_BODY(DEF) \
struct LOAD_DESC \
{ \
	DEF \
};
#pragma endregion

#define SAFE_DELETE(P) if(nullptr != P) { delete P; P = nullptr; }
#define SAFE_RELEASE(P) if(nullptr != P) {P->Release(); P = nullptr; }
#define SAFE_DESTROY(P) if(nullptr != P) {P->Destroy(); P = nullptr; }


#define LINEARSAMPLER L"LinearSampler"
#define LINEARCLAMPSAMPLER L"LinearClampSampler"
#define POINTSAMPLER L"PointSampler"


#define ALLFRAME (unsigned int)-1


#define SINGLETONBODY(TYPE) \
public:							\
	static TYPE& Inst()			\
	{							\
		static TYPE Temp;		\
		return Temp;			\
	}							\
private	:						\
	TYPE();						\
	~TYPE();				

#define GETSINGLE(TYPE) TYPE::Inst()

#define NOSAVE L"NonePrefab"