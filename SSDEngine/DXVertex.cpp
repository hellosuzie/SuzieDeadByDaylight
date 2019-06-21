#include "DXVertex.h"
#include <string.h>
#include <stdio.h>
#include "ResourceMgr.h"
#include "FilePathMgr.h" 
#include "Mesh.h"


COLORVTX	DXVTX::ColorVtx[4] = { { Vec4(-0.5f,  0.5f, 0.0f, 1.0f)  , FColor::White ,Vec2(0.f, 0.f) }				// ¿ÞÂÊÀ§
								,{ Vec4(0.5f,  0.5f, 0.0f, 1.0f)  , FColor::White ,Vec2(1.f, 0.f) }					// ¿À¸¥ÂÊÀ§
								,{ Vec4(-0.5f, -0.5f, 0.0f, 1.0f)  , FColor::White ,Vec2(0.f, 1.f) }				// ¿ÞÂÊ ¾Æ·¡
								,{ Vec4(0.5f, -0.5f, 0.0f, 1.0f)   , FColor::White ,Vec2(1.f, 1.f) } };				// ¿À¸¥ÂÊ ¾Æ·¡

BASICVTX	DXVTX::BasicVtx[4] = { { Vec3(-0.5f,  0.5f, 0.0f) ,Vec3(0.f, 0.f, 1.0f) ,Vec2(0.f, 0.f) }
							   ,{ Vec3(0.5f,  0.5f, 0.0f) ,Vec3(0.f, 0.f, 1.0f), Vec2(1.f, 0.f) }
							   ,{ Vec3(-0.5f, -0.5f, 0.0f) ,Vec3(0.f, 0.f, 1.0f), Vec2(0.f, 1.f) }
							   ,{ Vec3(0.5f, -0.5f, 0.0f)  ,Vec3(0.f, 0.f, 1.0f), Vec2(1.f, 1.f) } };

POSUVVTX	DXVTX::TargetViewVtx[4] = { { Vec4(-1.f,  1.f, 0.0f, 1.f), Vec2(0.f, 0.f) }
									,{ Vec4(1.f,  1.f, 0.0f, 1.f), Vec2(1.f, 0.f) }
									,{ Vec4(-1.f, -1.f, 0.0f, 1.f), Vec2(0.f, 1.f) }
									,{ Vec4(1.f, -1.f, 0.0f, 1.f), Vec2(1.f, 1.f) } };


// 0 : ¾Õ¸é , 1 : µÞ¸é  ,2 : ¿À¸¥ÂÊ¸é ,3 : ¿ÞÂÊ¸é , 4 : ¾Æ·§¸é , 5 : À­¸é
BASICVTX	DXVTX::CubeVtx[6][4] = { {{ Vec3(-0.5f, 0.5f, -0.5f)	, Vec3(0.f, 0.f,-1.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(0.5f, 0.5f, -0.5f)		, Vec3(0.f, 0.f,-1.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(0.5f, -0.5f, -0.5f)		, Vec3(0.f, 0.f,-1.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-0.5f, -0.5f, -0.5f)	, Vec3(0.f, 0.f,-1.f) ,Vec2(0.f, 1.f) }}

									,{{ Vec3(-0.5f, 0.5f, 0.5f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(0.5f, 0.5f, 0.5f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(0.5f, -0.5f, 0.5f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(0.f, 1.f) }
									,{ Vec3(-0.5f, -0.5f, 0.5f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(1.f, 1.f) }}

									,{{ Vec3(0.5f, 0.5f, -0.5f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(0.5f, 0.5f, 0.5f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(0.5f, -0.5f, 0.5f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(0.f, 1.f) }
									,{ Vec3(0.5f, -0.5f, -0.5f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(1.f, 1.f) }}

									,{{ Vec3(-0.5f, 0.5f, -0.5f)	, Vec3(-1.f, 0.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(-0.5f, 0.5f, 0.5f)		, Vec3(-1.f, 0.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(-0.5f, -0.5f, 0.5f)		, Vec3(-1.f, 0.f, 0.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-0.5f, -0.5f, -0.5f)	, Vec3(-1.f, 0.f, 0.f) ,Vec2(0.f, 1.f) }}

									,{{ Vec3(-0.5f, -0.5f, -0.5f)	, Vec3(0.f, -1.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(0.5f, -0.5f, -0.5f)		, Vec3(0.f, -1.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(0.5f, -0.5f, 0.5f)		, Vec3(0.f, -1.f, 0.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-0.5f, -0.5f, 0.5f)		, Vec3(0.f, -1.f, 0.f) ,Vec2(0.f, 1.f) }}

									,{{ Vec3(-0.5f, 0.5f, -0.5f)	, Vec3(0.f, 1.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(0.5f, 0.5f, -0.5f)		, Vec3(0.f, 1.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(0.5f, 0.5f, 0.5f)		, Vec3(0.f, 1.f, 0.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-0.5f, 0.5f, 0.5f)		, Vec3(0.f, 1.f, 0.f) ,Vec2(0.f, 1.f) }} };

// 0 : ¾Õ¸é , 1 : µÞ¸é  ,2 : ¿À¸¥ÂÊ¸é ,3 : ¿ÞÂÊ¸é , 4 : ¾Æ·§¸é , 5 : À­¸é
BASICVTX	DXVTX::FrustumVtx[6][4] = { { { Vec3(-1.f, 1.f, -1.f)	, Vec3(0.f, 0.f,-1.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(1.f, 1.f, -1.f)		, Vec3(0.f, 0.f,-1.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(1.f, -1.f, -1.f)		, Vec3(0.f, 0.f,-1.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-1.f, -1.f, -1.f)	, Vec3(0.f, 0.f,-1.f) ,Vec2(0.f, 1.f) } }
									
									,{ { Vec3(-1.f, 1.f, 1.f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(1.f, 1.f, 1.f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(1.f, -1.f, 1.f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(0.f, 1.f) }
									,{ Vec3(-1.f, -1.f, 1.f)		, Vec3(0.f, 0.f, 1.f) ,Vec2(1.f, 1.f) } }
									
									,{ { Vec3(1.f, 1.f, -1.f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(1.f, 1.f, 1.f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(1.f, -1.f, 1.f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(0.f, 1.f) }
									,{ Vec3(1.f, -1.f, -1.f)		, Vec3(1.f, 0.f, 0.f) ,Vec2(1.f, 1.f) } }
									
									,{ { Vec3(-1.f, 1.f, -1.f)	, Vec3(-1.f, 0.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(-1.f, 1.f, 1.f)		, Vec3(-1.f, 0.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(-1.f, -1.f, 1.f)		, Vec3(-1.f, 0.f, 0.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-1.f, -1.f, -1.f)	, Vec3(-1.f, 0.f, 0.f) ,Vec2(0.f, 1.f) } }
									
									,{ { Vec3(-1.f, -1.f, -1.f)	, Vec3(0.f, -1.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(1.f, -1.f, -1.f)		, Vec3(0.f, -1.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(1.f, -1.f, 1.f)		, Vec3(0.f, -1.f, 0.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-1.f, -1.f, 1.f)		, Vec3(0.f, -1.f, 0.f) ,Vec2(0.f, 1.f) } }
									
									,{ { Vec3(-1.f, 1.f, -1.f)	, Vec3(0.f, 1.f, 0.f) ,Vec2(0.f, 0.f) }
									,{ Vec3(1.f, 1.f, -1.f)		, Vec3(0.f, 1.f, 0.f) ,Vec2(1.f, 0.f) }
									,{ Vec3(1.f, 1.f, 1.f)		, Vec3(0.f, 1.f, 0.f) ,Vec2(1.f, 1.f) }
									,{ Vec3(-1.f, 1.f, 1.f)		, Vec3(0.f, 1.f, 0.f) ,Vec2(0.f, 1.f) } } };

COLORVTX	DXVTX::ColorCubeVtx[6][4] = { {{ Vec4(-0.5f, 0.5f, -0.5f)	, FColor::PastelMag ,Vec2(0.f, 0.f) }
									,{ Vec4(0.5f, 0.5f, -0.5f)		, FColor::Pink ,Vec2(1.f, 0.f) }
									,{ Vec4(0.5f, -0.5f, -0.5f)		, FColor::Pink ,Vec2(1.f, 1.f) }
									,{ Vec4(-0.5f, -0.5f, -0.5f)	, FColor::PastelMag ,Vec2(0.f, 1.f) }}

									,{{ Vec4(-0.5f, 0.5f, 0.5f)		, FColor::Lavender ,Vec2(1.f, 0.f) }
									,{ Vec4(0.5f, 0.5f, 0.5f)		, FColor::PastelMag ,Vec2(0.f, 0.f) }
									,{ Vec4(0.5f, -0.5f, 0.5f)		, FColor::PastelMag ,Vec2(0.f, 1.f) }
									,{ Vec4(-0.5f, -0.5f, 0.5f)		, FColor::Lavender ,Vec2(1.f, 1.f) }}

									,{{ Vec4(0.5f, 0.5f, -0.5f)		, FColor::Pink ,Vec2(1.f, 0.f) }
									,{ Vec4(0.5f, 0.5f, 0.5f)		, FColor::PastelMag ,Vec2(0.f, 0.f) }
									,{ Vec4(0.5f, -0.5f, 0.5f)		, FColor::PastelMag ,Vec2(0.f, 1.f) }
									,{ Vec4(0.5f, -0.5f, -0.5f)		, FColor::Pink ,Vec2(1.f, 1.f) }}

									,{{ Vec4(-0.5f, 0.5f, -0.5f)	, FColor::PastelMag ,Vec2(0.f, 0.f) }
									,{ Vec4(-0.5f, 0.5f, 0.5f)		, FColor::Lavender ,Vec2(1.f, 0.f) }
									,{ Vec4(-0.5f, -0.5f, 0.5f)		, FColor::Lavender ,Vec2(1.f, 1.f) }
									,{ Vec4(-0.5f, -0.5f, -0.5f)	, FColor::PastelMag ,Vec2(0.f, 1.f) }}

									,{{ Vec4(-0.5f, -0.5f, -0.5f)	, FColor::PastelMag ,Vec2(0.f, 1.f) }
									,{ Vec4(0.5f, -0.5f, -0.5f)		, FColor::Pink ,Vec2(1.f, 1.f) }
									,{ Vec4(0.5f, -0.5f, 0.5f)		, FColor::PastelMag ,Vec2(0.f, 1.f) }
									,{ Vec4(-0.5f, -0.5f, 0.5f)		, FColor::Lavender ,Vec2(1.f, 0.f) }}

									,{{ Vec4(-0.5f, 0.5f, -0.5f)	, FColor::PastelMag ,Vec2(0.f, 0.f) }
									,{ Vec4(0.5f, 0.5f, -0.5f)		, FColor::Pink ,Vec2(1.f, 0.f) }
									,{ Vec4(0.5f, 0.5f, 0.5f)		, FColor::PastelMag ,Vec2(0.f, 0.f) }
									,{ Vec4(-0.5f, 0.5f, 0.5f)		, FColor::Lavender ,Vec2(1.f, 0.f) }} };

IDX16		DXVTX::BasicIdx[2] = { IDX16(0, 3, 2) , IDX16(0, 1, 3) };
IDX32		DXVTX::BasicIdx32[2] = { IDX32(0, 3, 2) , IDX32(0, 1, 3) };



IDX16		DXVTX::CubeIdx[6][2] = { {IDX16(0, 1, 2) , IDX16(0, 2, 3)}
									 , {IDX16(5, 4, 7) , IDX16(5, 7, 6)}
									 , {IDX16(8, 9, 11) , IDX16(11, 9, 10)}
									 , {IDX16(14, 13, 12) , IDX16(14, 12, 15)}
									 , {IDX16(0, 5, 1) , IDX16(0, 4, 5)}
									 , {IDX16(7, 3, 2) , IDX16(7, 2, 6)} };


IDX16		DXVTX::ColorCubeIdx[6][2] = { {IDX16(0, 1, 2) , IDX16(0, 2, 3)}
									 , {IDX16(5, 4, 7) , IDX16(5, 7, 6)}
									 , {IDX16(1, 5, 2) , IDX16(2, 5, 6)}
									 , {IDX16(7, 4, 0) , IDX16(7, 0, 3)}
									 , {IDX16(20, 22, 21) , IDX16(20, 23, 22)}
									 , {IDX16(19, 16, 17) , IDX16(19, 17, 18)} };

