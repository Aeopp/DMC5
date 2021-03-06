#include "stdafx.h"
#include "..\Header\Hotel_S01.h"
#include "LoadingScene.h"
#include "PreLoader.h"
#include "TempMap.h"
#include "Nero.h"
#include "BtlPanel.h"
#include "Camera.h"
#include "MainCamera.h"
#include "Renderer.h"
#include "MapObject.h"
#include "MapAniObject.h"
#include "Em0000.h"
#include "Em100.h"
#include "Em1000.h"
#include "Trigger.h"
#include "QliphothBlock.h"
#include "FadeOut.h"
#include "CollObject.h"
#include "BreakableObject.h"
#include "SoundSystem.h"
#include "TimeSystem.h"
#include "BrokenPeople.h"
#include "ShockWave.h"
#include "NeroFSM.h"
#include "ShopPanel.h"
#include <iostream>
#include <fstream>

using namespace std;

Hotel_S01::Hotel_S01()
{

}

void Hotel_S01::Free()
{
	for (auto& Element : m_vecQliphothBlock)
		Destroy(Element);
	m_vecQliphothBlock.clear();
	m_vecQliphothBlock.shrink_to_fit();

	Scene::Free();
}

Hotel_S01* Hotel_S01::Create()
{
	Hotel_S01* pInstance = new Hotel_S01;
	return pInstance;
}

HRESULT Hotel_S01::LoadScene()
{
	// Load Start
	SoundSystem::GetInstance()->ClearSound();
	SoundSystem::GetInstance()->Play("Rain", 0.15f, false, {}, 11000);
	SoundSystem::GetInstance()->Play("Rain2", 0.15f, false);
	SoundSystem::GetInstance()->Play("Hotel01", _Hotel01_Volume, false);

	m_fLoadingProgress = 0.01f;

#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera

	//AddGameObject<Camera>();
	   
	_MainCamera = AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage1_Map.json");
	LoadObjects("../../Data/Stage1_AniObject.json", true);
	LoadCollObjects("../../Data/Stage1_Object.json");
	LoadBreakablebjects("../../Data/Stage1_BreakableObject.json");

	auto Map = AddGameObject<TempMap>().lock();
	Map->LoadMap(1);
	 
#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger

	RenderDataSetUp(false);
	TriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect

	// Stage1 ??????
	m_vecQliphothBlock.reserve(7);

	// 0: StartPoint 
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.016f);
		ptr.lock()->SetRotation(Vector3(0.f, 262.286f, 0.f));
		//ptr.lock()->SetPosition(Vector3(-5.7f, 0.286f, -5.05f));
		ptr.lock()->SetPosition(Vector3(-10.2f, 0.286f, -5.05f));
		ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	// 1 ~ 3: Wave 1
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.007f);
		ptr.lock()->SetRotation(Vector3(0.f, 205.714f, 0.f));
		ptr.lock()->SetPosition(Vector3(-0.5f, 0.355f, -2.775f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.025f);
		ptr.lock()->SetRotation(Vector3(0.f, 339.429f, 0.f));
		ptr.lock()->SetPosition(Vector3(-0.857f, 1.143f, 0.f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.016f);
		ptr.lock()->SetRotation(Vector3(0.f, 25.714f, 0.f));
		ptr.lock()->SetPosition(Vector3(1.429f, 1.429f, 0.f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	// 4 : Wave 2
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.012f);
		ptr.lock()->SetRotation(Vector3(0.f, 0.f, 0.f));
		ptr.lock()->SetPosition(Vector3(-3.59f, 0.408f, 7.172f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	// 5 ~ 6 : Wave 3
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.012f);
		ptr.lock()->SetRotation(Vector3(0.f, 180.f, 0.f));
		ptr.lock()->SetPosition(Vector3(-3.59f, 0.408f, 7.372f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.005f);
		ptr.lock()->SetRotation(Vector3(0.f, 0.f, 0.f));
		ptr.lock()->SetPosition(Vector3(-3.62f, 0.1f, 11.35f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	// ?????? ???????????? ???????????? ?????? ??????
	if(!_MainCamera.expired())
		_MainCamera.lock()->SetQliphothBlock(m_vecQliphothBlock);

#pragma endregion

	m_fLoadingProgress = 0.8f;

#pragma region UI

	_BtlPanel = AddGameObject<BtlPanel>();

#pragma endregion

	m_fLoadingProgress = 0.9f;

#pragma region Misc

#pragma endregion

	// Load Complete
	m_fLoadingProgress = 1.f;

	return S_OK;
}

HRESULT Hotel_S01::Awake()
{
	Scene::Awake();
	return S_OK;
}

HRESULT Hotel_S01::Start()
{
	Scene::Start();
	return S_OK;
}

HRESULT Hotel_S01::Update(const float _fDeltaTime)
{
	if (!_LateInit)
		LateInit();

	Scene::Update(_fDeltaTime);

	/* ---------- ?????? ---------- */
	if (Input::GetKeyDown(DIK_HOME))
	{
		g_bRenderTargetVisible = !g_bRenderTargetVisible;
	}
	if (Input::GetKeyDown(DIK_END))
	{
		g_bCollisionVisible = !g_bCollisionVisible;
	}
	if (Input::GetKeyDown(DIK_NUMPAD8))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S01));
	}
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S02));
	}
	/* -------------------------- */

	if (_DecreaseHotel01_Volume)
		_Hotel01_Volume = FMath::Lerp(_Hotel01_Volume, 0.f, _fDeltaTime);
	else
		_Hotel01_Volume = FMath::Lerp(_Hotel01_Volume, 0.12f, _fDeltaTime * 0.5f);

	if (_DecreaseBattle1_Volume)
	{
		_Battle1_Volume = FMath::Lerp(_Battle1_Volume, 0.f, _fDeltaTime);
		SoundSystem::GetInstance()->Play("Battle1", _Battle1_Volume, false);
	}

	if (_DecreaseBattle2_Volume)
	{
		_Battle2_Volume = FMath::Lerp(_Battle2_Volume, 0.f, _fDeltaTime);
		SoundSystem::GetInstance()->Play("Battle2", _Battle2_Volume, false);
	}

	_Rain_Volume = FMath::Lerp(_Rain_Volume, 0.f, _fDeltaTime * 0.5f);
	SoundSystem::GetInstance()->Play("Hotel01", _Hotel01_Volume, false);
	SoundSystem::GetInstance()->Play("Rain", _Rain_Volume, false, {}, 11000);
	
	return S_OK;
}

HRESULT Hotel_S01::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void Hotel_S01::LoadObjects(const std::filesystem::path& path, const bool _bAni)
{
	std::ifstream inputStream{ path };

	if (false == inputStream.is_open())
		return;

	using namespace rapidjson;

	IStreamWrapper inputSW(inputStream);
	Document docu;
	docu.ParseStream(inputSW);

	if (docu.HasParseError())
		return;

	std::filesystem::path sBasePath = TEXT("../../");
	sBasePath = std::filesystem::canonical(sBasePath);

	const Value& loadData = docu["GameObject"];

	std::filesystem::path sFullPath;
	if (_bAni == false)
	{
		for (auto iter = loadData.Begin(); iter != loadData.End(); ++iter)
		{
			//
			sFullPath = iter->FindMember("Mesh")->value.GetString();
			sFullPath = sBasePath / sFullPath;
			//
			
			Resources::Load<StaticMesh>(sFullPath);
			//
			auto objectArr = iter->FindMember("List")->value.GetArray();
			//
			for (auto iterObject = objectArr.begin(); iterObject != objectArr.end(); ++iterObject)
			{
				auto pMapObject = AddGameObject<MapObject>();

				D3DXVECTOR3 vScale;
				auto scale = iterObject->FindMember("Scale")->value.GetArray();
				vScale.x = scale[0].GetFloat();
				vScale.y = scale[1].GetFloat();
				vScale.z = scale[2].GetFloat();

				D3DXVECTOR3 vRotation;
				auto rotation = iterObject->FindMember("Rotation")->value.GetArray();
				vRotation.x = rotation[0].GetFloat();
				vRotation.y = rotation[1].GetFloat();
				vRotation.z = rotation[2].GetFloat();

				D3DXVECTOR3 vPosition;
				auto position = iterObject->FindMember("Position")->value.GetArray();
				vPosition.x = position[0].GetFloat();
				vPosition.y = position[1].GetFloat();
				vPosition.z = position[2].GetFloat();

				pMapObject.lock()->SetUp(sFullPath, vScale, vRotation, vPosition);
			}
		}
	}
	else
	{
		for (auto iter = loadData.Begin(); iter != loadData.End(); ++iter)
		{
			//
			sFullPath = iter->FindMember("Mesh")->value.GetString();
			sFullPath = sBasePath / sFullPath;
			//
			Resources::Load<SkeletonMesh>(sFullPath);
			//
			auto objectArr = iter->FindMember("List")->value.GetArray();
			//
			for (auto iterObject = objectArr.begin(); iterObject != objectArr.end(); ++iterObject)
			{
				auto pMapObject = AddGameObject<MapAniObject>();

				D3DXVECTOR3 vScale;
				auto scale = iterObject->FindMember("Scale")->value.GetArray();
				vScale.x = scale[0].GetFloat();
				vScale.y = scale[1].GetFloat();
				vScale.z = scale[2].GetFloat();

				D3DXVECTOR3 vRotation;
				auto rotation = iterObject->FindMember("Rotation")->value.GetArray();
				vRotation.x = rotation[0].GetFloat();
				vRotation.y = rotation[1].GetFloat();
				vRotation.z = rotation[2].GetFloat();

				D3DXVECTOR3 vPosition;
				auto position = iterObject->FindMember("Position")->value.GetArray();
				vPosition.x = position[0].GetFloat();
				vPosition.y = position[1].GetFloat();
				vPosition.z = position[2].GetFloat();

				pMapObject.lock()->SetUp(sFullPath, vScale, vRotation, vPosition);
			}
		}
	}
}

void Hotel_S01::LoadCollObjects(const std::filesystem::path& path)
{
	std::ifstream inputStream{ path };

	if (false == inputStream.is_open())
		return;

	using namespace rapidjson;

	IStreamWrapper inputSW(inputStream);
	Document docu;
	docu.ParseStream(inputSW);

	if (docu.HasParseError())
		return;

	std::filesystem::path sBasePath = TEXT("../../");
	sBasePath = std::filesystem::canonical(sBasePath);

	const Value& loadData = docu["GameObject"];

	std::filesystem::path sFullPath;
	for (auto iter = loadData.Begin(); iter != loadData.End(); ++iter)
	{
		//
		sFullPath = iter->FindMember("Mesh")->value.GetString();
		sFullPath = sBasePath / sFullPath;
		//
		Mesh::InitializeInfo _InitInfo{};
		_InitInfo.bLocalVertexLocationsStorage = true;
		Resources::Load<StaticMesh>(sFullPath, _InitInfo);
		//
		auto objectArr = iter->FindMember("List")->value.GetArray();
		//
		for (auto iterObject = objectArr.begin(); iterObject != objectArr.end(); ++iterObject)
		{
			auto pMapObject = AddGameObject<CollObject>();

			D3DXVECTOR3 vScale;
			auto scale = iterObject->FindMember("Scale")->value.GetArray();
			vScale.x = scale[0].GetFloat();
			vScale.y = scale[1].GetFloat();
			vScale.z = scale[2].GetFloat();

			D3DXVECTOR3 vRotation;
			auto rotation = iterObject->FindMember("Rotation")->value.GetArray();
			vRotation.x = rotation[0].GetFloat();
			vRotation.y = rotation[1].GetFloat();
			vRotation.z = rotation[2].GetFloat();

			D3DXVECTOR3 vPosition;
			auto position = iterObject->FindMember("Position")->value.GetArray();
			vPosition.x = position[0].GetFloat();
			vPosition.y = position[1].GetFloat();
			vPosition.z = position[2].GetFloat();

			pMapObject.lock()->SetUp(sFullPath, vScale, vRotation, vPosition);
		}
	}
}

void Hotel_S01::LoadBreakablebjects(const std::filesystem::path& path)
{
	std::ifstream inputStream{ path };

	if (false == inputStream.is_open())
		return;

	using namespace rapidjson;

	IStreamWrapper inputSW(inputStream);
	Document docu;
	docu.ParseStream(inputSW);

	if (docu.HasParseError())
		return;

	std::filesystem::path sBasePath = TEXT("../../");
	sBasePath = std::filesystem::canonical(sBasePath);

	const Value& loadData = docu["GameObject"];

	std::filesystem::path sFullPath;
	for (auto iter = loadData.Begin(); iter != loadData.End(); ++iter)
	{
		//
		sFullPath = iter->FindMember("Mesh")->value.GetString();
		sFullPath = sBasePath / sFullPath;
		//
	
		Resources::Load<StaticMesh>(sFullPath);
		//
		auto objectArr = iter->FindMember("List")->value.GetArray();
		//
		for (auto iterObject = objectArr.begin(); iterObject != objectArr.end(); ++iterObject)
		{
			auto pMapObject = AddGameObject<BreakableObject>();

			D3DXVECTOR3 vScale;
			auto scale = iterObject->FindMember("Scale")->value.GetArray();
			vScale.x = scale[0].GetFloat();
			vScale.y = scale[1].GetFloat();
			vScale.z = scale[2].GetFloat();

			D3DXVECTOR3 vRotation;
			auto rotation = iterObject->FindMember("Rotation")->value.GetArray();
			vRotation.x = rotation[0].GetFloat();
			vRotation.y = rotation[1].GetFloat();
			vRotation.z = rotation[2].GetFloat();

			D3DXVECTOR3 vPosition;
			auto position = iterObject->FindMember("Position")->value.GetArray();
			vPosition.x = position[0].GetFloat();
			vPosition.y = position[1].GetFloat();
			vPosition.z = position[2].GetFloat();

			pMapObject.lock()->SetUp(sFullPath, vScale, vRotation, vPosition);
		}
	}
}

void Hotel_S01::ApplyShopUpgradeDesc()
{
	auto& UpgradeDesc = ShopPanel::GetUpgradeDesc();

	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		if (2u <= UpgradeDesc._BatteryUpgradeCount)
			SpPlayer->BuyUpgradedOverture();
		if (2u <= UpgradeDesc._TransformUpgradeCount)
			SpPlayer->BuyCbsMiddle();
		if (3u <= UpgradeDesc._TransformUpgradeCount)
			SpPlayer->BuyCbsLong();
	}

	BtlPanel::SetExGaugeLevel(UpgradeDesc._ExgaugeUpUpgradeCount);
	BtlPanel::SetTDTGaugeLevel(UpgradeDesc._PurpleOrbUpgradeCount);
}

void Hotel_S01::RenderDataSetUp(const bool bTest)
{
	// ????????? ??? ??? ????????? ?????? ??????
	auto _Renderer = Renderer::GetInstance();

	if (bTest)
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	else
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Hotel_S01.json");

	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
	_Renderer->ao = 0.001f;
	_Renderer->SkyIntencity = 0.005f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f,0.f };
	_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f };
	_Renderer->SoftParticleDepthScale = 0.7f;
	_Renderer->SkyRotationSpeed = 1.5f;
	_Renderer->StarScale = 4.f;
	_Renderer->StarFactor = 0.9f;
}

void Hotel_S01::BgmPlay()
{
	// SoundSystem::GetInstance()->Play("Maple", 10.f, false, true);
}

void Hotel_S01::TriggerSetUp()
{
	// ????????? ????????? ?????? ????????? ?????? 
	TriggerElectricBoard(
		TriggerElectricBoardBattle()
	);

	TriggerInFrontOfHotel(
		TriggerInFrontOfHotelBattle()
	);

	Trigger2nd();
	Trigger4st();
};

void Hotel_S01::TriggerElectricBoard(const std::weak_ptr<Trigger>& _BattleTrigger)
{
	// ?????? ?????? ????????? 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[_BattleTrigger, this]()
		{
			vector<Vector3> _LostTimes;
			_LostTimes.emplace_back(Vector3{ 3.f,1.f,0.5f });
			TimeSystem::GetInstance()->LostTime(_LostTimes);
			_MainCamera.lock()->Set_TriggerCam(MainCamera::STAGE1_WAVE1, Vector3(0.338f, 1.037f, 0.524f), 3.f);
			_MainCamera.lock()->Set_Trigger(_BattleTrigger);
			// ????????? ????????? ?????? ?????? ....
			
			// .............

			// ???????????? ?????? ????????? ?????? ????????? ?????? ????????????
			// _BattleTrigger.lock()->TriggerEnable()  <- ????????? ?????? ..
		};
		
		// ????????? ??????
		const Vector3 TriggerLocation{ -0.66720f,0.01168f,-2.18399f };
		// ???????????? ????????? 
		const Vector3 BoxSize{ 1.f,1.f,1.f };
		// ????????? ?????? ?????????????????? ????????? ?? 
		const bool ImmediatelyEnable = true;
		// ???????????? ????????? ???????????? ?????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	};
};

std::weak_ptr<Trigger> Hotel_S01::TriggerElectricBoardBattle()
{
	auto _SecondTrigger = AddGameObject<Trigger>().lock();
	if (_SecondTrigger)
	{
		// ????????? ????????? ????????? ??????. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// ????????? ????????? ?????? ????????????  . 

		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.162f, 0.02f, -0.152f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -1.187f, 0.02f, -1.137f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.602f, 0.02f, -0.374f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 1.313f, 0.02f, -0.993f });
	
		// ????????? ?????? .. . 
		const Vector3 TriggerLocation{ -0.66720f,0.01168f,-2.18399f };
		// ????????? ?????? ????????? 
		const Vector3 TriggerBoxSize = { 100.f,100.f,100.f };
		// ????????? ?????? ?????? ???????????? ???????????? ????????? 
		const bool ImmediatelyEnable = false;
		// ????????? ????????? ??????????????? ???????????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// ?????? ?????? ????????? . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*????????? ?????? ??????????????? ( ????????? ???????????? ????????? ) */this]()
		{
			//... ????????? ?????? ??????????????? . 
		};

		// ????????? ?????? ?????? ???????????? ????????? . 
		const std::function<void()> WaveEndEvent =
			[this/*????????? ?????? ??????????????? (????????? ???????????? ?????????) */]()
		{
			//... ????????? ?????? ??????????????? . 
			// ????????? ????????? ?????? ????????? .
			_MainCamera.lock()->Set_PlayerCamMode(MainCamera::CAM_MODE_WAVE_END);
			_DecreaseHotel01_Volume = false;
			_DecreaseBattle1_Volume = true;
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			}
		};

		_SecondTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	}

	// ????????? ?????? !! 
	std::shared_ptr<Trigger> _StartTrigger{};
	if (_StartTrigger = AddGameObject<Trigger>().lock();
		_StartTrigger)
	{
		// ????????? ????????? ????????? ??????. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// ????????? ????????? ?????? ????????????  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.162f, 0.02f, -0.152f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -1.187f, 0.02f, -1.137f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.602f, 0.02f, -0.374f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 1.313f, 0.02f, -0.993f });

		// ????????? ?????? .. . 
		const Vector3 TriggerLocation{ -0.66720f,0.01168f,-2.18399f };
		// ????????? ?????? ????????? 
		const Vector3 TriggerBoxSize = { 100.f,100.f,100.f };
		// ????????? ?????? ?????? ???????????? ???????????? ????????? 
		const bool ImmediatelyEnable = false;
		// ????????? ????????? ??????????????? ???????????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// ?????? ?????? ????????? . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*????????? ?????? ??????????????? ( ????????? ???????????? ????????? ) */]()
		{
			//... ????????? ?????? ??????????????? . 
			_DecreaseHotel01_Volume = true;
			SoundSystem::GetInstance()->Play("Battle1", _Battle1_Volume, false);
			SoundSystem::GetInstance()->Play("BattleStart1", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart2", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart4", 1.f, true);
			SoundSystem::GetInstance()->Play("Em100Spawn", 1.f, true);
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}

			for (uint32 i = 1u; i < 4u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->SetActive(true);
					m_vecQliphothBlock[i].lock()->PlayStart();
				}
			}
		};

		// ????????? ?????? ?????? ???????????? ????????? . 
		const std::function<void()> WaveEndEvent =
			[this/*????????? ?????? ??????????????? (????????? ???????????? ?????????) */,
			_SecondTrigger]()
		{
			//... ????????? ?????? ??????????????? . 

			_SecondTrigger->TriggerEnable();
			
		};

		_StartTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	};

	return _StartTrigger;
}

void Hotel_S01::Trigger2nd()
{
	std::shared_ptr<Trigger> _Trigger{};

	if (_Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		// ????????? ????????? ????????? ??????. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>()
		};

		// ????????? ????????? ?????? ????????????  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.19094f, 0.009f, 5.34162f});

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.99099f, 0.009f, 6.28210f});

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.37271f, 0.009f, 5.71682f});

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.52741f, 0.009f, 6.39310f});

		// ????????? ?????? .. . 
		const Vector3 TriggerLocation{ -3.1017f, 0.011680f, 4.624099f };
		// ????????? ?????? ????????? 
		const Vector3 TriggerBoxSize = { 6.f, 3.f, 1.f };
		// ????????? ?????? ?????? ???????????? ???????????? ????????? 
		const bool ImmediatelyEnable = true;
		// ????????? ????????? ??????????????? ???????????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// ?????? ?????? ????????? . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*????????? ?????? ??????????????? ( ????????? ???????????? ????????? ) */]()
		{
			_MainCamera.lock()->SetShakeInfo(0.5f, 7.f);
			_Player.lock()->GetFsm().lock()->ChangeState(NeroFSM::WINDPRESSURE);
			//... ????????? ?????? ??????????????? . 

			SoundSystem::GetInstance()->Play("Em1000Spawn", 1.f, true);
			SoundSystem::GetInstance()->Play("Stone1", 1.f, true);
			Renderer::GetInstance()->SkyDistortionStart();
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}

			for (uint32 i = 1u; i < 4u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->SetActive(false);
				}
			}
			for (uint32 i = 4u; i < 5u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->SetActive(true);
					m_vecQliphothBlock[i].lock()->PlayStart();
				}
			}
		};

		// ????????? ?????? ?????? ???????????? ????????? . 
		const std::function<void()> WaveEndEvent =
			[this/*????????? ?????? ??????????????? (????????? ???????????? ?????????) */]()
		{
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			}

			for (uint32 i = 4u; i < 5u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->Reset();
				}
			}
		};

		_Trigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	}
}

void Hotel_S01::TriggerInFrontOfHotel(const std::weak_ptr<Trigger>& _BattleTrigger)
{
	// ?????? ?????? ????????? 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[_BattleTrigger,this]()
		{
			// ????????? ????????? ?????? ?????? ....
			// .............
			_MainCamera.lock()->Set_TriggerCam(MainCamera::STAGE1_WAVE2_ENTER, Vector3(-3.628f, 0.831f, 11.067f), 3.f);
			_MainCamera.lock()->Set_Trigger(_BattleTrigger);
			// ???????????? ?????? ????????? ?????? ????????? ?????? ????????????
			// _BattleTrigger.lock()->TriggerEnable()  <- ????????? ?????? ..
		};

		// ????????? ??????
		const Vector3 TriggerLocation{ -3.7564f , -0.003732f , 9.8498f };
		// ????????? ?????? ????????? 
		const Vector3 TriggerBoxSize = { 3.66f, 2.062f, 4.594f };
		// ????????? ?????? ?????????????????? ????????? ?? 
		const bool ImmediatelyEnable = true;
		// ???????????? ????????? ???????????? ?????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag);
	};
}

std::weak_ptr<Trigger> Hotel_S01::TriggerInFrontOfHotelBattle()
{
	auto  _EndTrigger = AddGameObject<Trigger>().lock();
	if (_EndTrigger)
	{
		// ????????? ????????? ????????? ??????. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		// ????????? ????????? ?????? ????????????  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.06028f, 0.01f, 10.42297f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.40446f, 0.01f, 10.4338f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.430736f, 0.01f, 8.29934f });
		
		// ????????? ?????? .. . 
		const Vector3 TriggerLocation{ -3.7564f , -0.003732f , 9.8498f };
		// ????????? ?????? ????????? 
		const Vector3 TriggerBoxSize = { 10.f, 10.f, 10.f };
		// ????????? ?????? ?????? ???????????? ???????????? ????????? 
		const bool ImmediatelyEnable = false;
		// ????????? ????????? ??????????????? ???????????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// ?????? ?????? ????????? . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*????????? ?????? ??????????????? ( ????????? ???????????? ????????? ) */this]()
		{
			//... ????????? ?????? ??????????????? . 

		};

		// ????????? ?????? ?????? ???????????? ????????? . 
		const std::function<void()> WaveEndEvent =
			[this/*????????? ?????? ??????????????? (????????? ???????????? ?????????) */]()
		{
			//... ????????? ?????? ??????????????? . 
			_MainCamera.lock()->Set_PlayerCamMode(MainCamera::CAM_MODE_WAVE_END);
			// ???????????? ???????????? ????????? ?????? ?????????????????? ....
			// ... ?????? !! 
			//
			_DecreaseHotel01_Volume = false;
			_DecreaseBattle2_Volume = true;
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			}
		};

		_EndTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	}

	auto _StartTrigger = AddGameObject<Trigger>().lock();
	if (_StartTrigger)
	{
		// ????????? ????????? ????????? ??????. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		// ????????? ????????? ?????? ????????????  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.06028f, 0.01f, 10.42297f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.40446f, 0.01f, 10.4338f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.430736f, 0.01f, 8.29934f });


		// ????????? ?????? .. . 
		const Vector3 TriggerLocation{ -3.7564f, -0.003732f, 9.8498f };
		// ????????? ?????? ????????? 
		const Vector3 TriggerBoxSize = { 10.f, 10.f, 10.f };
		// ????????? ?????? ?????? ???????????? ???????????? ????????? 
		const bool ImmediatelyEnable = false;
		// ????????? ????????? ??????????????? ???????????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// ?????? ?????? ????????? . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*????????? ?????? ??????????????? ( ????????? ???????????? ????????? ) */]()
		{
			//... ????????? ?????? ??????????????? . 
			_DecreaseHotel01_Volume = true;
			SoundSystem::GetInstance()->Play("Battle2", _Battle2_Volume, false);
			SoundSystem::GetInstance()->Play("BattleStart1", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart2", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart4", 1.f, true);
			SoundSystem::GetInstance()->Play("Em100Spawn", 1.f, true);

			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}

			for (uint32 i = 4u; i < 5u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->SetActive(false);
				}
			}
			for (uint32 i = 5u; i < 7u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->SetActive(true);
					m_vecQliphothBlock[i].lock()->PlayStart();
				}
			}
		};

		// ????????? ?????? ?????? ???????????? ????????? . 
		const std::function<void()> WaveEndEvent =
			[this/*????????? ?????? ??????????????? (????????? ???????????? ?????????) */,
			_EndTrigger]()
		{
			//... ????????? ?????? ??????????????? . 
			_EndTrigger->TriggerEnable();
		};

		_StartTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	}

	return _StartTrigger;
}

void Hotel_S01::Trigger4st()
{
	// ?????? ?????? ????????? 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[this, _FadeOut = AddGameObject<FadeOut>().lock()]()
		{
			auto SpPanel = _BtlPanel.lock();
			if (SpPanel)
			{
				SpPanel->SetRedOrbActive(false);
				SpPanel->SetGlobalActive(false);
			}

			if (_FadeOut)
			{
				_FadeOut->PlayStart(0u, 
					[SpPanel]()
					{ 
						SpPanel->SetNullBlackActive(true);
						SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S02)); 
					});
			}
		};
		
		// ????????? ??????
		const Vector3 TriggerLocation { -3.171700f, 0.011680f, 12.167461f };
		// ???????????? ????????? 
		const Vector3 BoxSize{ 6.f,5.f,1.f };
		// ????????? ?????? ?????????????????? ????????? ?? 
		const bool ImmediatelyEnable = true;
		// ???????????? ????????? ???????????? ?????? 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}
}

void Hotel_S01::LateInit()
{
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({ -9.5f, -0.23f, -5.13f });
		SpPlayer->SetAngle(-90.f);
	}

	ApplyShopUpgradeDesc();

	if (auto SpMainCamera = _MainCamera.lock();
		SpMainCamera)
	{
		 SpMainCamera->SetAngle({-6.8f,-90.f,0.f});
		 SpMainCamera->SetStartPos();
	}

	Renderer::GetInstance()->LateSceneInit();

	BgmPlay();

	_LateInit = true;

	g_bOptRender = false;
	g_bFixedDeltaTime = true;
}