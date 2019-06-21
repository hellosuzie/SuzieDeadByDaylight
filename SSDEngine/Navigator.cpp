#include "Navigator.h"
#include "TransForm.h"
#include "DXMath.h"


CNavigator::CNavigator()
	: m_CurNaviMesh(nullptr), m_CurNaviTri(nullptr), m_PrevPos(Vec3())
{
}


CNavigator::~CNavigator()
{
}
void CNavigator::SetCurNaviMesh(SPTR<CNavigationMesh> _Navi)
{
	m_CurNaviMesh = _Navi;
	m_CurNaviMesh->SubTransUpdate();
	m_CurNaviTri =  m_CurNaviMesh->SearchNaviTri(TRANS->GetWorldPos());
	m_CurNaviTri->ChangeNaviTriColor();
	RisingNavigator(m_CurNaviTri->GetNavigationHeight(TRANS->GetWorldPos(), m_CurNaviMesh->GetSubTransCRWorld()));
	m_PrevPos = TRANS->GetWorldPos();
}
void CNavigator::InitCurNaviMesh()
{
	if (nullptr == m_CurNaviMesh)
		return;

	m_CurNaviMesh->ExitNavigatorEvent();
	m_CurNaviMesh = nullptr;
	m_CurNaviTri = nullptr;
}
void  CNavigator::PostUpdate(const float& _DeltaTime)
{
	if (nullptr == m_CurNaviMesh)
		return;

	if (true == IsTransMove()) 
	{
		Vec3 CurPos = TRANS->GetWorldPos();
		NaviTri* HandOverTri  = nullptr;
		CNavigationMesh* HandOverNavi = m_CurNaviTri->CheckHandOverNaviMesh(CurPos, &HandOverTri);

		if (nullptr != HandOverNavi)
		{
			m_CurNaviTri = nullptr;
			m_CurNaviMesh = HandOverNavi;
			m_CurNaviTri = HandOverTri;
			RisingNavigator(m_CurNaviTri->GetNavigationHeight(CurPos, m_CurNaviMesh->GetSubTransCRWorld()));
			return;
		}

		Mat MeshWorld = m_CurNaviMesh->GetSubTransCRWorld();
		NaviTri* MeleeTri = nullptr;

		if (true == m_CurNaviTri->CheckMoveable(CurPos, &MeleeTri)) 
		{
			if (nullptr != MeleeTri) 
			{
				m_CurNaviTri = MeleeTri;
			}

			RisingNavigator(m_CurNaviTri->GetNavigationHeight(CurPos, MeshWorld));	
		}
		else 
		{
			CalculateMovablePos(CurPos, Mat(m_CurNaviTri->Tri[0].GetVec4_Coord(), m_CurNaviTri->Tri[1].GetVec4_Coord(), m_CurNaviTri->Tri[2].GetVec4_Coord()) * MeshWorld);
			return;
		}

	}

}
void CNavigator::LastUpdate(const float& _DeltaTime)
{
	m_PrevPos = TRANS->GetWorldPos();
}

//void CNavigator::RollBackPrevPos(const Vec3& _CurPos)
//{
//	Vec3 MoveDir = Vec3(_CurPos.x - m_PrevPos.x, 0.f, _CurPos.z - m_PrevPos.z);
//
//	if (DXMath::ExceptSmallNumber(MoveDir.x) == 0.f|| DXMath::ExceptSmallNumber(MoveDir.z) == 0.f)
//	{
//		TRANS->SetPos(m_PrevPos);
//	}
//	else 
//	{
//		// 움직이는 방향이 대각선 방향인경우 움직일수 있는 방향을 재조정해준다.
//		Vec3 Ratio;
//		Vec3 DistX = Vec3(NAVIDISTFACTOR, 0.f, 0.f), DistZ = Vec3(0.f, 0.f, NAVIDISTFACTOR);
//	
//		while (true == m_CurNaviMesh->CheckMoveable(m_PrevPos + (MoveDir * Ratio)))
//		{
//			Ratio += Vec3(NAVIDISTFACTOR, 0.f, NAVIDISTFACTOR);
//		}
//		
//		Ratio -= Vec3(NAVIDISTFACTOR, 0.f, NAVIDISTFACTOR);
//		
//		while (true == m_CurNaviMesh->CheckMoveable(m_PrevPos + (MoveDir * (Ratio + DistX))) && DistX.x < 1.f + NAVIDISTFACTOR - Ratio.x)
//		{
//			DistX.x += NAVIDISTFACTOR;
//		}
//
//		while (true == m_CurNaviMesh->CheckMoveable(m_PrevPos + (MoveDir * (Ratio + DistZ ))) && DistZ.z < 1.f + NAVIDISTFACTOR - Ratio.z)
//		{
//			DistZ.z += NAVIDISTFACTOR;
//		}
//
//		DistX.x = DistX.x - NAVIDISTFACTOR;
//		DistZ.z  = DistZ.z - NAVIDISTFACTOR;
//		Ratio += (DistX + DistZ);
//		Ratio.x = DXMath::Clamp(Ratio.x, 0.f, 1.f);
//		Ratio.z = DXMath::Clamp(Ratio.z, 0.f, 1.f);
//
//		m_PrevPos += Ratio * MoveDir;
//		m_PrevPos.y = m_CurNaviMesh->GetNavigationHeight(m_PrevPos);
//		TRANS->SetPos(m_PrevPos);
//	}
//
//}
void CNavigator::CalculateMovablePos(const Vec3& _CurPos, const Mat& _TriPos)
{
	Vec3 Tri[3] = { _TriPos.Row[0],_TriPos.Row[1] ,_TriPos.Row[2] };

	size_t StartIdx = -1;

	for (size_t i = 0; i < 3; i++)
	{
		Vec2 TriLine = (Vec2(Tri[(i + 1) % 3].x, Tri[(i + 1) % 3].z) - Vec2(Tri[i].x, Tri[i].z)).GetNormalizeVec();
		Vec2 CurDir = (Vec2(_CurPos.x, _CurPos.z) - Vec2(Tri[i].x, Tri[i].z)).GetNormalizeVec();
		Vec2 PrevDir = (Vec2(m_PrevPos.x, m_PrevPos.z) - Vec2(Tri[i].x, Tri[i].z)).GetNormalizeVec();

		// 외적을 한 결과가 서로 다를때의 시작 Index를 저장
		if (DXMath::Vector2Cross(TriLine, CurDir) * DXMath::Vector2Cross(TriLine, PrevDir) < 0) 
		{
			StartIdx = i;
			break;
		}
	}

	if (-1 == StartIdx) 
	{
		TASSERT(true);
		return;
	}

	Vec2 LineDir = (Vec2(Tri[(StartIdx + 1) % 3].x, Tri[(StartIdx + 1) % 3].z) - Vec2(Tri[StartIdx].x, Tri[StartIdx].z)).GetNormalizeVec();
	Vec2 NorDir = Vec2(-LineDir.y, LineDir.x);
	
	Vec2 StartPos = Vec2(Tri[StartIdx].x, Tri[StartIdx].z);
	Vec2 U = LineDir * DXMath::Vector2Dot(Vec2(_CurPos.x - m_PrevPos.x, _CurPos.z - m_PrevPos.z),LineDir);
	Vec2 V = NorDir * DXMath::Vector2Dot(Vec2(_CurPos.x - m_PrevPos.x, _CurPos.z - m_PrevPos.z), NorDir); 
	Vec2 Ratio;
	Vec2 Prev = Vec2(m_PrevPos.x, m_PrevPos.z);
	Vec3 MovePos = m_PrevPos;

	if (DXMath::Vector2Cross(LineDir, Prev - StartPos) * DXMath::Vector2Cross(LineDir, Prev + U - StartPos) > 0)
	{
		MovePos += Vec3(U.x, 0.f, U.y);
	}

	if (DXMath::Vector2Cross(LineDir, Prev - StartPos) * DXMath::Vector2Cross(LineDir, Prev + V - StartPos) > 0)
	{
		MovePos +=  Vec3(V.x, 0.f, V.y);
	}

	if (true == m_CurNaviTri->TriangleTest(MovePos,m_CurNaviMesh->GetSubTransCRWorld()))
	{
		m_PrevPos = MovePos;
		m_PrevPos.y = m_CurNaviTri->GetNavigationHeight(m_PrevPos,m_CurNaviMesh->GetSubTransCRWorld());
	}
	else 
	{
		int a = 0;
	}

	TRANS->SetPos(m_PrevPos);
}
void CNavigator::RisingNavigator(const float& _Height)
{
	TRANS->SetPosY(_Height);
	//m_PrevPos = TRANS->GetWorldPos();
}