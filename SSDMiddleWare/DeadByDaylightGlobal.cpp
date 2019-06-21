#include "DeadByDaylightGlobal.h"
#include <KeyMgr.h>
#include <FilePathMgr.h>
#include <SSDCore.h>
#include <ResourceMgr.h>
#include <DXFont.h>
#include <Terrain.h>
#include <Scene.h>
#include <FbxRenderer.h>
#include <OBBCollider.h>
#include <SphereCollider.h>
#include <WriteStream.h>
#include <ReadStream.h>
#include <Light.h>
#include <Fbx.h>
#include <Mesh.h>
#include <Material.h>
#include <TransForm.h>
#include "MagicCircle.h"
#include <NavigationMesh.h>
#include "NavigationObject.h"
#include <Terrain.h>
#include <TerrainRenderer.h>
#include <Skybox.h>
#include "Hook.h"
//#include "DBDRule.h"


CDXFont*												DeadByDaylightGlobal::pPrettyFont = nullptr;
CTerrain*												DeadByDaylightGlobal::pMainTerrain = nullptr;
CParticleRenderer*										DeadByDaylightGlobal::pMainParticle = nullptr;
//CDBDRule*												DeadByDaylightGlobal::pGameRule = nullptr;


std::vector<SPTR<ActorSaveData>>						DeadByDaylightGlobal::m_vecActorData;
std::vector<SPTR<ActorSaveData>>						DeadByDaylightGlobal::m_vecActorLoadData;
std::vector<SPTR<FilterSaveData>>						DeadByDaylightGlobal::m_vecFilterData;

std::list<SPTR<CFbxRenderer>>							DeadByDaylightGlobal::m_FbxList;
std::list<SPTR<COBBCollider>>							DeadByDaylightGlobal::m_ObbList;
std::list<SPTR<CSphereCollider>>						DeadByDaylightGlobal::m_SphereList;
std::list<SPTR<CNavigationObject>>						DeadByDaylightGlobal::m_NaviObjList;

std::map<int, std::list<SPTR<CActor>>>					DeadByDaylightGlobal::m_mapAllActor;
std::map<int, std::list<SPTR<CActor>>>					DeadByDaylightGlobal::m_mapAllLoadActor;

TerrainData												DeadByDaylightGlobal::m_TerrainLoadData;
RoomList												DeadByDaylightGlobal::m_RecvRoomList;
LobbyEnterData											DeadByDaylightGlobal::m_RecvRoomData;
WorldData												DeadByDaylightGlobal::m_RecvWorldData;

bool													DeadByDaylightGlobal::bGameWin = false;

void DeadByDaylightGlobal::Init()
{
	pPrettyFont = CResourceMgr<CDXFont>::Find(L"나눔바른펜");


	PathSetting();
	KeySetting();
}

void DeadByDaylightGlobal::Release()
{

}

void DeadByDaylightGlobal::PathSetting()
{
	CFilePathMgr::AddPlusFolder(L"Fbx", L"Architecture");
	CFilePathMgr::AddPlusFolder(L"Fbx", L"Animation");
	CFilePathMgr::AddPlusFolder(L"Texture", L"UI");
	CFilePathMgr::AddRootFolder(L"Scene");
}

void DeadByDaylightGlobal::KeySetting()
{
	CKeyMgr::BindingKey(L"CAM_FRONT", L"VK_UP");
	CKeyMgr::BindingKey(L"CAM_BACK", L"VK_DOWN");
	CKeyMgr::BindingKey(L"CAM_LEFT", L"VK_LEFT");
	CKeyMgr::BindingKey(L"CAM_RIGHT", L"VK_RIGHT");
	CKeyMgr::BindingKey(L"CAM_UP", L"Q");
	CKeyMgr::BindingKey(L"CAM_DOWN", L"E");
	CKeyMgr::BindingKey(L"CAM_RESET", L"CtrlR");
	CKeyMgr::BindingKey(L"CAM_BOOST", L"LShift");
	CKeyMgr::BindingKey(L"CAM_HOLD", L"RMouse");

	CKeyMgr::BindingKey(L"UP", L"W");
	CKeyMgr::BindingKey(L"DOWN", L"S");
	CKeyMgr::BindingKey(L"LEFT", L"A");
	CKeyMgr::BindingKey(L"RIGHT", L"D");

	CKeyMgr::BindingKey(L"PrevAni", L"PAGE_UP");
	CKeyMgr::BindingKey(L"NextAni", L"PAGE_DOWN");

	CKeyMgr::BindingKey(L"R", L"R");
	CKeyMgr::BindingKey(L"F", L"F");
	CKeyMgr::BindingKey(L"T", L"T");
	CKeyMgr::BindingKey(L"G", L"G");
	CKeyMgr::BindingKey(L"Y", L"Y");
	CKeyMgr::BindingKey(L"H", L"H");

	CKeyMgr::BindingKey(L"U", L"U");
	CKeyMgr::BindingKey(L"J", L"J");
	CKeyMgr::BindingKey(L"I", L"I");
	CKeyMgr::BindingKey(L"K", L"K");
	CKeyMgr::BindingKey(L"O", L"O");
	CKeyMgr::BindingKey(L"L", L"L");

	CKeyMgr::BindingKey(L"CameraKey", L"Tab");
	CKeyMgr::BindingKey(L"NaviBakeKey", L"CtrlB");
	CKeyMgr::BindingKey(L"NaviAreaKey", L"CtrlA");

	CKeyMgr::BindingKey(L"EscapeKey", L"Escape");

	CKeyMgr::BindingKey(L"f5", L"F5");

	CKeyMgr::BindingKey(L"SurvivorControl", L"F1");
	CKeyMgr::BindingKey(L"KillerControl", L"F2");

	CKeyMgr::BindingKey(L"Crouch", L"Ctrl");
	CKeyMgr::BindingKey(L"Run", L"LShift");
	CKeyMgr::BindingKey(L"Space", L"SPACE");

	CKeyMgr::BindingKey(L"Attack", L"LMouse");
	CKeyMgr::BindingKey(L"SelfHealing", L"1");
	CKeyMgr::BindingKey(L"Dissolve", L"Enter");

	CKeyMgr::BindingKey(L"LMouseClick", L"LMouse");

}

void DeadByDaylightGlobal::SaveScene(const wchar_t* _Path, CScene* _WriteScene)
{
	m_vecActorData.clear();
	m_vecFilterData.clear();

	//엑터 가져오기
	m_mapAllActor = _WriteScene->GetAllActor();

	if (0 >= m_mapAllActor.size())
	{
		return;
	}

	std::map<int, std::list<SPTR<CActor>>>::iterator ActorListStart = m_mapAllActor.begin();
	std::map<int, std::list<SPTR<CActor>>>::iterator ActorListEnd = m_mapAllActor.end();

	for (; ActorListStart != ActorListEnd; ++ActorListStart)
	{
		std::list<SPTR<CActor>>::iterator ListStartIter = ActorListStart->second.begin();
		std::list<SPTR<CActor>>::iterator ListEndIter = ActorListStart->second.end();

		for (; ListStartIter != ListEndIter; ++ListStartIter)
		{
			//저장할 데이터 만들기 - 엑터 트렌스폼 넣어주면서 생성
			SPTR<ActorSaveData> NewData = new ActorSaveData(
				(*ListStartIter)->TRANS->GetScale(),
				(*ListStartIter)->GetWorldPos(),
				(*ListStartIter)->TRANS->GetLocalRotate());

			NewData->UpdateLayer = ActorListStart->first;

			//마법진 정보 저장 - 무조건 하나
			SPTR<CMagicCircle> FindCircle = (*ListStartIter)->FindCom<CMagicCircle>();
			if (nullptr != FindCircle)
			{
				NewData->IsMagicCircle = true;
				NewData->MagicCircleColor = FindCircle->GetMagicCircleColor();
				NewData->MagicCircleScale = FindCircle->GetMagicCircleRen()->GetRenSize();
				NewData->MagicCirclePivot = FindCircle->GetMagicCircleRen()->GetRenPivot();
				m_vecActorData.push_back(NewData);
				continue;
			}

			//fbx랜더러 정보
			(*ListStartIter)->FindAllCom<CFbxRenderer>(&m_FbxList);
			if (0 < m_FbxList.size())
			{
				std::list<SPTR<CFbxRenderer>>::iterator RenStartIter = m_FbxList.begin();
				std::list<SPTR<CFbxRenderer>>::iterator RenEndIter = m_FbxList.end();
				for (; RenStartIter != RenEndIter; ++RenStartIter)
				{
					PrefabData::FbxSaveData TempData = PrefabData::FbxSaveData();
					const std::vector<SPTR<CFBXMesh>> TempFbxMeshVec = (*RenStartIter)->GetRenderingFbx()->GetAllFBXMeshVec();
					const std::vector<SPTR<CMesh>> TempAllMeshVec = (*RenStartIter)->GetAllMeshVec();
					if (0 < TempFbxMeshVec.size())
					{
						if (TempFbxMeshVec.size() == TempAllMeshVec.size())
						{
							TempData.AllCheck = true;
						}
						else
						{
							TempData.AllCheck = false;
						}

						//매쉬 이름 넣기
						TempData.MeshName = (*RenStartIter)->GetRenderingFbx()->GetFileName();
						//인덱스 넣기
						for (int i = 0; i < TempAllMeshVec.size(); ++i)
						{
							CFBXMesh* Test = (CFBXMesh*)TempAllMeshVec[i].PTR;
							UINT Index = Test->GetMeshIdx();
							TempData.MatInfoMap.insert(std::map<int, std::wstring>::value_type(Index, Test->GetMatStart(0)->GetPath()));
						}

						//랜더러 사이즈 포스 넣기
						TempData.RenderSize = (*RenStartIter)->GetRenSize();
						TempData.RenderPos = (*RenStartIter)->GetRenPivot();
						TempData.IsDeferred = /*(*RenStartIter)->GetFbxMesh(0)->GetMatStart(0)->IsDeferred();*/true;
						//TempData.RenderLayer = (*RenStartIter)->GetRenderLayer();

						NewData->vecFbxData.push_back(TempData);
					}
				}
			}

			//Obb충돌체 정보 
			(*ListStartIter)->FindAllCom<COBBCollider>(&m_ObbList);
			if (0 < m_ObbList.size())
			{
				std::list<SPTR<COBBCollider>>::iterator ColStartIter = m_ObbList.begin();
				std::list<SPTR<COBBCollider>>::iterator ColEndIter = m_ObbList.end();
				for (; ColStartIter != ColEndIter; ++ColStartIter)
				{
					if (true != (*ColStartIter)->ExistTag(NOSAVE))
					{
						PrefabData::ColSaveData TempData = PrefabData::ColSaveData(COL_TYPE::COLTYPE_OBB);
						TempData.ColGroupName = (*ColStartIter)->GetChannel().c_str();
						TempData.ColPos = (*ColStartIter)->GetColliderPivot();
						TempData.ColSize = (*ColStartIter)->GetColliderSize();

						NewData->vecColData.push_back(TempData);
					}
				}
			}

			//Sphere 충돌체 정보
			(*ListStartIter)->FindAllCom<CSphereCollider>(&m_SphereList);
			if (0 < m_SphereList.size())
			{
				std::list<SPTR<CSphereCollider>>::iterator SColStartIter = m_SphereList.begin();
				std::list<SPTR<CSphereCollider>>::iterator SColEndIter = m_SphereList.end();
				for (; SColStartIter != SColEndIter; ++SColStartIter)
				{
					PrefabData::ColSaveData TempData = PrefabData::ColSaveData(COL_TYPE::COLTYPE_SPHERE);
					TempData.ColGroupName = (*SColStartIter)->GetChannel().c_str();
					TempData.ColPos = (*SColStartIter)->GetColliderPivot();
					TempData.ColSize = (*SColStartIter)->GetColliderSize();

					NewData->vecColData.push_back(TempData);
				}
			}


			// 네비게이션 메쉬 정보 
			(*ListStartIter)->FindAllCom<CNavigationObject>(&m_NaviObjList);
			if (false == m_NaviObjList.empty())
			{
				std::list<SPTR<CNavigationObject>>::iterator NaviStart = m_NaviObjList.begin();
				std::list<SPTR<CNavigationObject>>::iterator NaviEnd = m_NaviObjList.end();

				for (; NaviStart != NaviEnd; ++NaviStart)
				{
					PrefabData::NavigationData NaviData = PrefabData::NavigationData();
					NaviData.NaviObjName = (*NaviStart)->GetNaviRen()->GetName();
					NaviData.NaviSerialNumber = (*NaviStart)->GetNaviRen()->GetSerialNumber();
					NaviData.RenderLayer = (int)(*NaviStart)->GetNaviRen()->GetRenderLayer();

					NewData->vecNaviData.push_back(NaviData);
				}

			}

			//라이트 정보 - 엑터에 라이트는 무조건 하나
			SPTR<CLight> FindLight = (*ListStartIter)->FindCom<CLight>();
			if (nullptr != FindLight)
			{
				ActorSaveData::LightSaveData TempLightData = ActorSaveData::LightSaveData();
				TempLightData.Layer = FindLight->m_LightData.Layer;
				TempLightData.LightSubSize = FindLight->GetLightSize();
				TempLightData.VolumeCheck = FindLight->m_LightData.VolumeCheck;
				if (true == TempLightData.VolumeCheck)
				{
					TempLightData.MeshName = FindLight->GetVolumeMeshName();
				}
				else
				{
					TempLightData.MeshName = L"";
				}
				TempLightData.LightBuffer = FindLight->m_LightBufferData;
				NewData->IsLight = true;
				NewData->LightData = TempLightData;
			}

			if (0 >= m_FbxList.size() && 0 >= m_ObbList.size() && 0 >= m_SphereList.size() && nullptr == FindLight)
			{
				continue;
			}

			m_vecActorData.push_back(NewData);

			m_FbxList.clear();
			m_ObbList.clear();
			m_SphereList.clear();
			m_NaviObjList.clear();
		}
	}

	WriteScene(_Path, _WriteScene);
}
void DeadByDaylightGlobal::WriteScene(const wchar_t* _Path, CScene* _WriteScene)
{
	CWriteStream WriteData = CWriteStream(_Path);

	// 씬파일 저장순서
	// 1. 씬 해더 파일
	// 2. 액터 정보
	// 3. 터레인 정보

	// 1. 씬파일의 요약 정보를 담은 씬파일해더 정보를 파일에 저장
	SceneFileHeader SceneHeader;
	memset(&SceneHeader, 0, sizeof(SceneFileHeader));
	SceneHeader.FileVersion = (int)LAST_SCENE_VERSION;
	SceneHeader.ActorCount = (int)m_vecActorData.size();
	WriteData.Write(SceneHeader);

	// 2. 액터 정보 저장
	for (size_t i = 0; i < m_vecActorData.size(); ++i)
	{
		//엑터 정보
		WriteData.Write(m_vecActorData[i]->UpdateLayer);
		WriteData.Write(m_vecActorData[i]->ActorPos);
		WriteData.Write(m_vecActorData[i]->ActorSize);
		WriteData.Write(m_vecActorData[i]->ActorRot);

		//fbx정보 읽기
		int FbxSize = (int)m_vecActorData[i]->vecFbxData.size();
		WriteData.Write(FbxSize);

		for (size_t j = 0; j < FbxSize; ++j)
		{
			WriteData.Write(m_vecActorData[i]->vecFbxData[j].AllCheck);
			WriteData.Write(m_vecActorData[i]->vecFbxData[j].MeshName);
			WriteData.Write(m_vecActorData[i]->vecFbxData[j].RenderPos);
			WriteData.Write(m_vecActorData[i]->vecFbxData[j].RenderSize);
			WriteData.Write(m_vecActorData[i]->vecFbxData[j].IsDeferred);
			//WriteData.Write(m_vecActorData[i]->vecFbxData[j].RenderLayer);

			int MatMapSize = (int)m_vecActorData[i]->vecFbxData[j].MatInfoMap.size();
			WriteData.Write(MatMapSize);

			std::map<int, std::wstring>::iterator MatStartIter = m_vecActorData[i]->vecFbxData[j].MatInfoMap.begin();
			std::map<int, std::wstring>::iterator MatEndIter = m_vecActorData[i]->vecFbxData[j].MatInfoMap.end();
			for (; MatStartIter != MatEndIter; ++MatStartIter)
			{
				int Temp = MatStartIter->first;
				WriteData.Write(Temp);
				WriteData.Write(MatStartIter->second);
			}
		}

		int ColVecSize = (int)m_vecActorData[i]->vecColData.size();
		WriteData.Write(ColVecSize);

		for (size_t k = 0; k < ColVecSize; ++k)
		{
			WriteData.Write(m_vecActorData[i]->vecColData[k].ColType);
			WriteData.Write(m_vecActorData[i]->vecColData[k].ColGroupName);
			WriteData.Write(m_vecActorData[i]->vecColData[k].ColPos);
			WriteData.Write(m_vecActorData[i]->vecColData[k].ColSize);
		}

		int NaviVecSize = (int)m_vecActorData[i]->vecNaviData.size();
		WriteData.Write(NaviVecSize);

		for (size_t k = 0; k < NaviVecSize; ++k)
		{
			WriteData.Write(m_vecActorData[i]->vecNaviData[k].RenderLayer);
			WriteData.Write(m_vecActorData[i]->vecNaviData[k].NaviSerialNumber);
			WriteData.Write(m_vecActorData[i]->vecNaviData[k].NaviObjName);
		}

		bool IsLightSave = m_vecActorData[i]->IsLight;
		WriteData.Write(IsLightSave);

		if (true == IsLightSave)
		{
			WriteData.Write(m_vecActorData[i]->LightData.Layer);
			WriteData.Write(m_vecActorData[i]->LightData.LightBuffer);
			WriteData.Write(m_vecActorData[i]->LightData.VolumeCheck);
			WriteData.Write(m_vecActorData[i]->LightData.MeshName);
			WriteData.Write(m_vecActorData[i]->LightData.LightSubSize);
		}

		bool IsCircleSave = m_vecActorData[i]->IsMagicCircle;
		WriteData.Write(IsCircleSave);

		if (true == IsCircleSave)
		{
			WriteData.Write(m_vecActorData[i]->MagicCircleColor);
			WriteData.Write(m_vecActorData[i]->MagicCircleScale);
			WriteData.Write(m_vecActorData[i]->MagicCirclePivot);
		}
	}

	// 3. 터레인 정보 저장
	SPTR<CTerrainRenderer> TerrainRen = DBD::pMainTerrain->GetTerrainRen();
	std::list<SPTR<CNaviLinkTri>>* pLinkList = TerrainRen->GetLinkTriList();

	TerrainData::TerrainDataHeader HeaderData;
	HeaderData.TerrainSizeX = TerrainRen->GetTerrainRowCount();
	HeaderData.TerrainSizeZ = TerrainRen->GetTerrainColmCount();
	HeaderData.TerrainCellSize = TerrainRen->GetCellSize();
	HeaderData.TerrainMaxHeight = TerrainRen->GetMaxHeight();
	HeaderData.LinkTriCount = (unsigned int)pLinkList->size();

	WriteData.Write(HeaderData);
	size_t MaxTerrainCount = (size_t)(HeaderData.TerrainSizeX * HeaderData.TerrainSizeZ);

	for (size_t i = 0; i < MaxTerrainCount; i++)
	{
		GEOTERRAINVTX GeoData = TerrainRen->GetGeoTerrainVtx(i);

		if (GeoData.Color != NONAVITRICOLOR)
		{
			GeoData.Color = NAVITRICOLOR;
		}

		WriteData.Write(GeoData);
	}

	std::list<SPTR<CNaviLinkTri>>::iterator Start = pLinkList->begin();
	std::list<SPTR<CNaviLinkTri>>::iterator End = pLinkList->end();

	TerrainData::NaviLinkData LData;

	for (; Start != End; ++Start)
	{
		LData.NaviMeshID = (*Start)->GetLinkNaviMesh()->GetSerialNumber();
		LData.NaviTriIdx = (*Start)->GetLinkNaviTriIdx();
		LData.TerrainTriIdx = (*Start)->GetLinkTerrainIdx();
		for (size_t i = 0; i < 3; i++)
		{
			LData.LinkVtxData[i] = (*Start)->m_LinkVtx[i];
		}

		WriteData.Write(LData);
	}
}
void DeadByDaylightGlobal::ReadScene(const wchar_t* _Path, CScene* _ReadScene, const bool& _bCircle /*= false*/)
{
	CReadStream ReadData = CReadStream(_Path);


	// 1. 씬 해더파일 읽기
	// 2. 액터 데이터 일기
	// 3. 터레인 데이터 읽기

	SceneFileHeader SHeader;

	ReadData.Read(SHeader);
	m_vecActorLoadData.clear();

	switch (SHeader.FileVersion)
	{
	case TERRAIN_NAVIGATION_SCENE_FILE:
		ReadScene_TerrainNavi(ReadData, SHeader);
		break;
	case TERRAIN_MAGIC_FILE:
		ReadScene_TerrainMagic(ReadData, SHeader);
		break;
	case TERRAIN_MAGIC_DEFERRED_FILE:
		ReadScene_TerrainMagicDeferrd(ReadData, SHeader);
		break;
		//case TERRAIN_MAGIC_LAYER_FILE:
		//	ReadScene_TerrainMagicDeferrd(ReadData, SHeader);
		break;
	default:
	{
		int a = 0;
		return;
	}
	break;
	}


	// 3. 터레인 정보 읽어오기
	m_TerrainLoadData.NaviLinkTriList.clear();
	m_TerrainLoadData.TerrainDataVec.clear();

	ReadData.Read(m_TerrainLoadData.Header);

	size_t MaxVtx = (size_t)(m_TerrainLoadData.Header.TerrainSizeX*m_TerrainLoadData.Header.TerrainSizeZ);

	m_TerrainLoadData.TerrainDataVec.resize(MaxVtx);
	for (size_t i = 0; i < MaxVtx; i++)
	{
		ReadData.Read(m_TerrainLoadData.TerrainDataVec[i]);
	}

	TerrainData::NaviLinkData LData;
	for (size_t i = 0; i < m_TerrainLoadData.Header.LinkTriCount; i++)
	{
		ReadData.Read(LData);
		m_TerrainLoadData.NaviLinkTriList.push_back(LData);
	}

	//씬에 배치해주기
	LoadScene(_ReadScene, (SCENEFILEVISION)SHeader.FileVersion, _bCircle);

	// 4. 파일에서 읽어들인 터레인 정보를 적용한다.  
	DBD::pMainTerrain->LoadTerrainData(m_TerrainLoadData);

}

void DeadByDaylightGlobal::ReadScene(const wchar_t* _Path)
{
	CReadStream ReadData = CReadStream(_Path);


	// 1. 씬 해더파일 읽기
	// 2. 액터 데이터 일기
	// 3. 터레인 데이터 읽기

	SceneFileHeader SHeader;

	ReadData.Read(SHeader);
	m_vecActorLoadData.clear();

	switch (SHeader.FileVersion)
	{
	case TERRAIN_NAVIGATION_SCENE_FILE:
		ReadScene_TerrainNavi(ReadData, SHeader);
		break;
	case TERRAIN_MAGIC_FILE:
		ReadScene_TerrainMagic(ReadData, SHeader);
		break;
	case TERRAIN_MAGIC_DEFERRED_FILE:
		ReadScene_TerrainMagicDeferrd(ReadData, SHeader);
		break;
		//case TERRAIN_MAGIC_LAYER_FILE:
		//	ReadScene_TerrainMagicDeferrd(ReadData, SHeader);
		break;
	default:
	{
		int a = 0;
		return;
	}
	break;
	}


	// 3. 터레인 정보 읽어오기
	m_TerrainLoadData.NaviLinkTriList.clear();
	m_TerrainLoadData.TerrainDataVec.clear();

	ReadData.Read(m_TerrainLoadData.Header);

	size_t MaxVtx = (size_t)(m_TerrainLoadData.Header.TerrainSizeX*m_TerrainLoadData.Header.TerrainSizeZ);

	m_TerrainLoadData.TerrainDataVec.resize(MaxVtx);
	for (size_t i = 0; i < MaxVtx; i++)
	{
		ReadData.Read(m_TerrainLoadData.TerrainDataVec[i]);
	}

	TerrainData::NaviLinkData LData;

	for (size_t i = 0; i < m_TerrainLoadData.Header.LinkTriCount; i++)
	{
		ReadData.Read(LData);
		m_TerrainLoadData.NaviLinkTriList.push_back(LData);
	}
}

void DeadByDaylightGlobal::ReadScene_TerrainNavi(CReadStream& _ReadData, const SceneFileHeader& _Header)
{
	for (size_t i = 0; i < _Header.ActorCount; ++i)
	{
		int UpdateLayer;
		_ReadData.Read(UpdateLayer);
		Vec3 Pos;
		_ReadData.Read(Pos);
		Vec3 Size;
		_ReadData.Read(Size);
		Vec3 Rot;
		_ReadData.Read(Rot);

		ActorSaveData* NewData = new ActorSaveData(Size, Pos, Rot);
		NewData->UpdateLayer = UpdateLayer;

		int FbxVecSize;
		_ReadData.Read(FbxVecSize);

		NewData->vecFbxData.resize(FbxVecSize);
		for (size_t j = 0; j < NewData->vecFbxData.size(); ++j)
		{
			bool AllCheck;
			_ReadData.Read(AllCheck);
			std::wstring MeshName;
			_ReadData.Read(MeshName);
			Vec3 RenderPos;
			_ReadData.Read(RenderPos);
			Vec3 RenderSize;
			_ReadData.Read(RenderSize);

			NewData->vecFbxData[j].AllCheck = AllCheck;
			NewData->vecFbxData[j].MeshName = MeshName;
			NewData->vecFbxData[j].RenderPos = RenderPos;
			NewData->vecFbxData[j].RenderSize = RenderSize;

			int MatMapSize;
			_ReadData.Read(MatMapSize);
			for (size_t k = 0; k < MatMapSize; ++k)
			{
				int Index;
				_ReadData.Read(Index);
				std::wstring MatName;
				_ReadData.Read(MatName);

				NewData->vecFbxData[j].MatInfoMap.insert(std::map<int, std::wstring>::value_type(Index, MatName));
			}
		}

		int ColVecSize;
		_ReadData.Read(ColVecSize);

		NewData->vecColData.resize(ColVecSize);
		for (size_t k = 0; k < ColVecSize; ++k)
		{
			COL_TYPE Type;
			_ReadData.Read(Type);
			std::wstring GroupName;
			_ReadData.Read(GroupName);
			Vec3 ColPos;
			_ReadData.Read(ColPos);
			Vec3 ColSize;
			_ReadData.Read(ColSize);

			NewData->vecColData[k].ColType = Type;
			NewData->vecColData[k].ColGroupName = GroupName;
			NewData->vecColData[k].ColPos = ColPos;
			NewData->vecColData[k].ColSize = ColSize;
		}

		int NaviVecSize;
		_ReadData.Read(NaviVecSize);

		NewData->vecNaviData.resize(NaviVecSize);
		for (size_t k = 0; k < NaviVecSize; ++k)
		{
			_ReadData.Read(NewData->vecNaviData[k].RenderLayer);
			_ReadData.Read(NewData->vecNaviData[k].NaviSerialNumber);
			_ReadData.Read(NewData->vecNaviData[k].NaviObjName);
		}

		bool IsLight;
		_ReadData.Read(IsLight);
		NewData->IsLight = IsLight;
		if (true == IsLight)
		{
			RENDER_LAYER Layer;
			_ReadData.Read(Layer);
			CBUFFER::LightBuffer LightBuffer;
			_ReadData.Read(LightBuffer);
			bool VolumeCheck;
			_ReadData.Read(VolumeCheck);
			std::wstring MeshName;
			_ReadData.Read(MeshName);
			Vec3 LightSubSize;
			_ReadData.Read(LightSubSize);

			NewData->LightData.Layer = Layer;
			NewData->LightData.LightBuffer = LightBuffer;
			NewData->LightData.VolumeCheck = VolumeCheck;
			NewData->LightData.MeshName = MeshName;
			NewData->LightData.LightSubSize = LightSubSize;
		}

		m_vecActorLoadData.push_back(NewData);
	}
}
void DeadByDaylightGlobal::ReadScene_TerrainMagic(CReadStream& _ReadData, const SceneFileHeader& _Header)
{
	for (size_t i = 0; i < _Header.ActorCount; ++i)
	{
		int UpdateLayer;
		_ReadData.Read(UpdateLayer);
		Vec3 Pos;
		_ReadData.Read(Pos);
		Vec3 Size;
		_ReadData.Read(Size);
		Vec3 Rot;
		_ReadData.Read(Rot);

		ActorSaveData* NewData = new ActorSaveData(Size, Pos, Rot);
		NewData->UpdateLayer = UpdateLayer;

		int FbxVecSize;
		_ReadData.Read(FbxVecSize);

		NewData->vecFbxData.resize(FbxVecSize);
		for (size_t j = 0; j < NewData->vecFbxData.size(); ++j)
		{
			bool AllCheck;
			_ReadData.Read(AllCheck);
			std::wstring MeshName;
			_ReadData.Read(MeshName);
			Vec3 RenderPos;
			_ReadData.Read(RenderPos);
			Vec3 RenderSize;
			_ReadData.Read(RenderSize);

			NewData->vecFbxData[j].AllCheck = AllCheck;
			NewData->vecFbxData[j].MeshName = MeshName;
			NewData->vecFbxData[j].RenderPos = RenderPos;
			NewData->vecFbxData[j].RenderSize = RenderSize;

			int MatMapSize;
			_ReadData.Read(MatMapSize);
			for (size_t k = 0; k < MatMapSize; ++k)
			{
				int Index;
				_ReadData.Read(Index);
				std::wstring MatName;
				_ReadData.Read(MatName);

				NewData->vecFbxData[j].MatInfoMap.insert(std::map<int, std::wstring>::value_type(Index, MatName));
			}
		}

		int ColVecSize;
		_ReadData.Read(ColVecSize);

		NewData->vecColData.resize(ColVecSize);
		for (size_t k = 0; k < ColVecSize; ++k)
		{
			COL_TYPE Type;
			_ReadData.Read(Type);
			std::wstring GroupName;
			_ReadData.Read(GroupName);
			Vec3 ColPos;
			_ReadData.Read(ColPos);
			Vec3 ColSize;
			_ReadData.Read(ColSize);

			NewData->vecColData[k].ColType = Type;
			NewData->vecColData[k].ColGroupName = GroupName;
			NewData->vecColData[k].ColPos = ColPos;
			NewData->vecColData[k].ColSize = ColSize;
		}

		int NaviVecSize;
		_ReadData.Read(NaviVecSize);

		NewData->vecNaviData.resize(NaviVecSize);
		for (size_t k = 0; k < NaviVecSize; ++k)
		{
			_ReadData.Read(NewData->vecNaviData[k].RenderLayer);
			_ReadData.Read(NewData->vecNaviData[k].NaviSerialNumber);
			_ReadData.Read(NewData->vecNaviData[k].NaviObjName);
		}

		bool IsLight;
		_ReadData.Read(IsLight);
		NewData->IsLight = IsLight;
		if (true == IsLight)
		{
			RENDER_LAYER Layer;
			_ReadData.Read(Layer);
			CBUFFER::LightBuffer LightBuffer;
			_ReadData.Read(LightBuffer);
			bool VolumeCheck;
			_ReadData.Read(VolumeCheck);
			std::wstring MeshName;
			_ReadData.Read(MeshName);
			Vec3 LightSubSize;
			_ReadData.Read(LightSubSize);

			NewData->LightData.Layer = Layer;
			NewData->LightData.LightBuffer = LightBuffer;
			NewData->LightData.VolumeCheck = VolumeCheck;
			NewData->LightData.MeshName = MeshName;
			NewData->LightData.LightSubSize = LightSubSize;
		}

		bool IsCircleSave;
		_ReadData.Read(IsCircleSave);
		if (true == IsCircleSave)
		{
			MAGICCIRCLE_COLOR TempColor;
			_ReadData.Read(TempColor);
			Vec3 TempScale;
			_ReadData.Read(TempScale);
			Vec3 TempPivot;
			_ReadData.Read(TempPivot);

			NewData->IsMagicCircle = IsCircleSave;
			NewData->MagicCircleColor = TempColor;
			NewData->MagicCircleScale = TempScale;
			NewData->MagicCirclePivot = TempPivot;
		}

		m_vecActorLoadData.push_back(NewData);
	}

}
void DeadByDaylightGlobal::ReadScene_TerrainMagicDeferrd(CReadStream& _ReadData, const SceneFileHeader& _Header)
{
	for (size_t i = 0; i < _Header.ActorCount; ++i)
	{
		int UpdateLayer;
		_ReadData.Read(UpdateLayer);
		Vec3 Pos;
		_ReadData.Read(Pos);
		Vec3 Size;
		_ReadData.Read(Size);
		Vec3 Rot;
		_ReadData.Read(Rot);

		ActorSaveData* NewData = new ActorSaveData(Size, Pos, Rot);
		NewData->UpdateLayer = UpdateLayer;

		int FbxVecSize;
		_ReadData.Read(FbxVecSize);

		NewData->vecFbxData.resize(FbxVecSize);
		for (size_t j = 0; j < NewData->vecFbxData.size(); ++j)
		{
			bool AllCheck;
			_ReadData.Read(AllCheck);
			std::wstring MeshName;
			_ReadData.Read(MeshName);
			Vec3 RenderPos;
			_ReadData.Read(RenderPos);
			Vec3 RenderSize;
			_ReadData.Read(RenderSize);
			bool IsDeferred;
			_ReadData.Read(IsDeferred);

			NewData->vecFbxData[j].AllCheck = AllCheck;
			NewData->vecFbxData[j].MeshName = MeshName;
			NewData->vecFbxData[j].RenderPos = RenderPos;
			NewData->vecFbxData[j].RenderSize = RenderSize;
			NewData->vecFbxData[j].IsDeferred = IsDeferred;

			int MatMapSize;
			_ReadData.Read(MatMapSize);
			for (size_t k = 0; k < MatMapSize; ++k)
			{
				int Index;
				_ReadData.Read(Index);
				std::wstring MatName;
				_ReadData.Read(MatName);

				NewData->vecFbxData[j].MatInfoMap.insert(std::map<int, std::wstring>::value_type(Index, MatName));
			}
		}

		int ColVecSize;
		_ReadData.Read(ColVecSize);

		NewData->vecColData.resize(ColVecSize);
		for (size_t k = 0; k < ColVecSize; ++k)
		{
			COL_TYPE Type;
			_ReadData.Read(Type);
			std::wstring GroupName;
			_ReadData.Read(GroupName);
			Vec3 ColPos;
			_ReadData.Read(ColPos);
			Vec3 ColSize;
			_ReadData.Read(ColSize);

			NewData->vecColData[k].ColType = Type;
			NewData->vecColData[k].ColGroupName = GroupName;
			NewData->vecColData[k].ColPos = ColPos;
			NewData->vecColData[k].ColSize = ColSize;
		}

		int NaviVecSize;
		_ReadData.Read(NaviVecSize);

		NewData->vecNaviData.resize(NaviVecSize);
		for (size_t k = 0; k < NaviVecSize; ++k)
		{
			_ReadData.Read(NewData->vecNaviData[k].RenderLayer);
			_ReadData.Read(NewData->vecNaviData[k].NaviSerialNumber);
			_ReadData.Read(NewData->vecNaviData[k].NaviObjName);
		}

		bool IsLight;
		_ReadData.Read(IsLight);
		NewData->IsLight = IsLight;
		if (true == IsLight)
		{
			RENDER_LAYER Layer;
			_ReadData.Read(Layer);
			CBUFFER::LightBuffer LightBuffer;
			_ReadData.Read(LightBuffer);
			bool VolumeCheck;
			_ReadData.Read(VolumeCheck);
			std::wstring MeshName;
			_ReadData.Read(MeshName);
			Vec3 LightSubSize;
			_ReadData.Read(LightSubSize);

			NewData->LightData.Layer = Layer;
			NewData->LightData.LightBuffer = LightBuffer;
			NewData->LightData.VolumeCheck = VolumeCheck;
			NewData->LightData.MeshName = MeshName;
			NewData->LightData.LightSubSize = LightSubSize;
		}

		bool IsCircleSave;
		_ReadData.Read(IsCircleSave);
		if (true == IsCircleSave)
		{
			MAGICCIRCLE_COLOR TempColor;
			_ReadData.Read(TempColor);
			Vec3 TempScale;
			_ReadData.Read(TempScale);
			Vec3 TempPivot;
			_ReadData.Read(TempPivot);

			NewData->IsMagicCircle = IsCircleSave;
			NewData->MagicCircleColor = TempColor;
			NewData->MagicCircleScale = TempScale;
			NewData->MagicCirclePivot = TempPivot;
		}

		m_vecActorLoadData.push_back(NewData);
	}
}

Vec2 DeadByDaylightGlobal::NumbertoUIRaio(Vec2 _Pos)
{
	float FinalW = (_Pos.x * Core::GetClientWidth()) / 1600.f;
	float FinalH = (_Pos.y * Core::GetClientHeight()) / 900.f;

	return Vec2(FinalW, FinalH);
}

void DeadByDaylightGlobal::LoadScene(CScene* _LoadScene, const SCENEFILEVISION& _FileVersion, const bool& _bCircle)
{
	//터레인, 카메라, 스카이박스 제외한 엑터 전부 삭제하기
	m_mapAllLoadActor = _LoadScene->GetAllActor();

	std::map<int, std::list<SPTR<CActor>>>::iterator Startiter = m_mapAllLoadActor.begin();
	std::map<int, std::list<SPTR<CActor>>>::iterator Enditer = m_mapAllLoadActor.end();

	for (; Startiter != Enditer; ++Startiter)
	{
		std::list<SPTR<CActor>> ActorList = Startiter->second;
		std::list<SPTR<CActor>>::iterator SIter = ActorList.begin();
		std::list<SPTR<CActor>>::iterator EIter = ActorList.end();
		for (; SIter != EIter; ++SIter)
		{
			SPTR<CCamera> TempCamera = (*SIter)->FindCom<CCamera>();
			if (nullptr != TempCamera)
			{
				continue;
			}

			SPTR<CSkybox> TempSky = (*SIter)->FindCom<CSkybox>();
			if (nullptr != TempSky)
			{
				continue;
			}

			SPTR<CTerrain> TempTerrain = (*SIter)->FindCom<CTerrain>();

			if (nullptr != TempTerrain)
			{
				continue;
			}

			if (nullptr != (*SIter))
			{
				(*SIter)->Death();
			}
		}
	}

	Engine::ClearNaviMeshMap();

	// 1. 파일에서 읽어들인 액터 정보를 적용
	for (size_t k = 0; k < m_vecActorLoadData.size(); ++k)
	{
		if (nullptr != m_vecActorLoadData[k])
		{
			ACTOR_UPDATE_LAYER Layer = (ACTOR_UPDATE_LAYER)m_vecActorLoadData[k]->UpdateLayer;
			bool IsLight = m_vecActorLoadData[k]->IsLight;
			bool IsCircle = m_vecActorLoadData[k]->IsMagicCircle;

			//라이트 엑터
			if (true == IsLight)
			{
				SPTR<CActor> NewActor = _LoadScene->CreateActor(_LoadScene, Layer, L"LightActor");
				NewActor->TRANS->SetPos(m_vecActorLoadData[k]->ActorPos);
				NewActor->TRANS->SetScale(m_vecActorLoadData[k]->ActorSize);
				NewActor->TRANS->SetRotate(m_vecActorLoadData[k]->ActorRot);

				ActorSaveData::LightSaveData TempData = m_vecActorLoadData[k]->LightData;

				SPTR<CLight> TempLight = NewActor->AddCom<CLight>(LIGHT_DATA{
					TempData.Layer,
					(LIGHT_TYPE)TempData.LightBuffer.LightType,
					TempData.VolumeCheck,
					TempData.MeshName });

				TempLight->SetLightBuffer(TempData.LightBuffer);
				TempLight->SetLightSize(TempData.LightSubSize);
				TempLight->SetLightDir(TempData.LightBuffer.LightDir.vec3);

				//if (0 == CSceneRenderMgr::FLightBuffer.LightCount)
				//{
				//	CSceneRenderMgr::FLightBuffer.ArrLightData[0] = TempLight->m_LightBufferData;
				//	CSceneRenderMgr::FLightBuffer.LightCount = 1;
				//}
			}
			//마법진 엑터
			else if (true == IsCircle)
			{
				if (false == _bCircle)
					continue;

				SPTR<CActor> NewActor = _LoadScene->CreateActor(_LoadScene, Layer, L"LightActor");
				NewActor->TRANS->SetPos(m_vecActorLoadData[k]->ActorPos);
				NewActor->TRANS->SetScale(m_vecActorLoadData[k]->ActorSize);
				NewActor->TRANS->SetRotate(m_vecActorLoadData[k]->ActorRot);

				SPTR<CMagicCircle> TempCircle = NewActor->AddCom<CMagicCircle>(m_vecActorLoadData[k]->MagicCircleColor);
				TempCircle->GetMagicCircleRen()->SetRenSize(m_vecActorLoadData[k]->MagicCircleScale);
				TempCircle->GetMagicCircleRen()->SetRenPivot(m_vecActorLoadData[k]->MagicCirclePivot);
				
			}
			else
			{
				SPTR<CActor> NewActor = _LoadScene->CreateActor(_LoadScene, Layer, L"NewActor");
				NewActor->TRANS->SetPos(m_vecActorLoadData[k]->ActorPos);
				NewActor->TRANS->SetScale(m_vecActorLoadData[k]->ActorSize);
				NewActor->TRANS->SetRotate(m_vecActorLoadData[k]->ActorRot);

				//랜더러
				std::vector<PrefabData::FbxSaveData> FbxVector = m_vecActorLoadData[k]->vecFbxData;
				for (size_t i = 0; i < FbxVector.size(); ++i)
				{
					SPTR<CFbxRenderer> TempRender = NewActor->AddCom<CFbxRenderer>(RENDATA{ FbxVector[i].RenderLayer/*RENDER_LAYER_PLAYER */ });
					TempRender->SetRenSize(FbxVector[i].RenderSize);
					TempRender->SetRenPivot(FbxVector[i].RenderPos);

					TempRender->SetFbx(FbxVector[i].MeshName.c_str());
					if (true == FbxVector[i].AllCheck)
					{
						TempRender->SetAllFbxMesh();
					}
					else if (false == FbxVector[i].AllCheck)
					{
						std::map<int, std::wstring>::iterator StartIter = FbxVector[i].MatInfoMap.begin();
						std::map<int, std::wstring>::iterator EndIter = FbxVector[i].MatInfoMap.end();
						for (; StartIter != EndIter; ++StartIter)
						{
							TempRender->SettingFbxMesh(StartIter->first);
						}
					}

					//for (UINT m = 0; m < TempRender->GetFbxMesh(0)->GetMatSize(); ++m)
					//{
					//	TempRender->GetFbxMesh(0)->GetMatStart(m)->SetDeferred(FbxVector[i].IsDeferred);
					//}
				}

				//충돌체
				
				std::vector<PrefabData::ColSaveData> ColSaveVector = m_vecActorLoadData[k]->vecColData;
				for (size_t j = 0; j < ColSaveVector.size(); ++j)
				{
					if (COL_TYPE::COLTYPE_OBB == m_vecActorLoadData[k]->vecColData[j].ColType)
					{
						SPTR<COBBCollider> TempColl = NewActor->AddCom<COBBCollider>(COLDATA{ m_vecActorLoadData[k]->vecColData[j].ColGroupName, COLLISION_3D });
						TempColl->SetColSize(m_vecActorLoadData[k]->vecColData[j].ColSize);
						TempColl->SetColPivot(m_vecActorLoadData[k]->vecColData[j].ColPos);
					}
					else if (COL_TYPE::COLTYPE_SPHERE == m_vecActorLoadData[k]->vecColData[j].ColType)
					{
						SPTR<CSphereCollider> TempColl = NewActor->AddCom<CSphereCollider>(COLDATA{ m_vecActorLoadData[k]->vecColData[j].ColGroupName, COLLISION_3D });
						TempColl->SetColSize(m_vecActorLoadData[k]->vecColData[j].ColSize);
						TempColl->SetColPivot(m_vecActorLoadData[k]->vecColData[j].ColPos);
					}
				}
				

				// 네비게이션 메쉬
				std::vector<PrefabData::NavigationData>& NaviDataVec = m_vecActorLoadData[k]->vecNaviData;
				size_t NaviCount = NaviDataVec.size();
				for (size_t i = 0; i < NaviCount; i++)
				{

					SPTR<CNavigationObject> NaviObj = NewActor->AddCom<CNavigationObject>();

					NaviObj->CreateNaviArea(DBD::pMainTerrain);
					NaviObj->CreateNavigationRenderer(NAVIMESH_DATA{ (RENDER_LAYER)NaviDataVec[i].RenderLayer, NaviDataVec[i].NaviSerialNumber, NaviDataVec[i].NaviObjName });
					NaviObj->GetNaviRen()->CreateNaviMeshCollider(L"NaviTri");
					NaviObj->CreateNavigationMesh(NaviDataVec[i].NaviObjName.c_str(), NAVIMESH_OBJ);
				}
			}
		}
	}


	m_vecActorLoadData.clear();

}


void DeadByDaylightGlobal::RecordRoomListData(const RoomList& _RecvData)
{
	m_RecvRoomList = _RecvData;
}
void DeadByDaylightGlobal::RecordRecvRoomData(const LobbyEnterData& _RecvData)
{
	m_RecvRoomData = _RecvData;
}