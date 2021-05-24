#include "stdafx.h"
#include "..\Header\Hotel_S03.h"
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
#include "Monster.h"
#include "Trigger.h"
#include "FadeOut.h"
#include "BreakableObject.h"
#include "Em100.h"
#include "Em0000.h"
#include "CollObject.h"
#include "SoundSystem.h"
#include "AnimationUpGround.h"
#include "Smoke.h"
#include "MakaiButterfly.h"
#include "QliphothBlock.h"

#include <iostream>
#include <fstream>
#include "NeroFSM.h"
using namespace std;

Hotel_S03::Hotel_S03()
{

}

void Hotel_S03::Free()
{
	Scene::Free();
}

Hotel_S03* Hotel_S03::Create()
{
	Hotel_S03* pInstance = new Hotel_S03;
	return pInstance;
}


HRESULT Hotel_S03::LoadScene()
{
	// Load Start
	SoundSystem::GetInstance()->ClearSound();
	SoundSystem::GetInstance()->Play("Hotel03", _Hotel03_Volume, false);
	m_fLoadingProgress = 0.01f;

#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera

	//if (auto SpCamera = AddGameObject<Camera>().lock();
	//		SpCamera)
	//{
	//	SpCamera->GetComponent<Transform>().lock()->SetPosition(
	//		Vector3{ -1.77158f, 1.36541f, 23.73719f }
	//	);
	//}

	_MainCamera = AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	//LoadObjects("../../Data/Stage3_Map.json");
	//LoadObjects("../../Data/Stage3_AniObject.json", true);
	//LoadCollObjects("../../Data/Stage3_Object.json");
	//LoadBreakablebjects("../../Data/Stage3_BreakableObject.json");

	auto Map = AddGameObject<TempMap>().lock();
	Map->LoadMap(3);

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger

	RenderDataSetUp(false);
	TriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect

	// Stage3 길막
	m_vecQliphothBlock.reserve(7);

	// 0: 보스 가는길
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.003f);
		ptr.lock()->SetRotation(Vector3(0.f, 264.7f, 0.f));
		ptr.lock()->SetPosition(Vector3(-0.555f, 0.35f, 30.835f));
		ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	// 1 ~ 2: 구덩이
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.015f);
		ptr.lock()->SetRotation(Vector3(0.f, 180.f, 0.f));
		ptr.lock()->SetPosition(Vector3(0.21f, 0.9f, 31.3f));
		ptr.lock()->SetActive(false);
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.015f);
		ptr.lock()->SetRotation(Vector3(0.f, 0.f, 0.f));
		ptr.lock()->SetPosition(Vector3(0.21f, 0.9f, 33.75f));
		ptr.lock()->SetActive(false);
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	if (auto Sp = AddGameObject<MakaiButterfly>().lock(); Sp)
	{
		Sp->SetPosition({ -4.366f, 0.55f, 34.1f });
		Sp->PlayStart();
	}

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

HRESULT Hotel_S03::Awake()
{
	Scene::Awake();
	return S_OK;
}

HRESULT Hotel_S03::Start()
{
	Scene::Start();
	return S_OK;
}

HRESULT Hotel_S03::Update(const float _fDeltaTime)
{
	if (!_LateInit)
		LateInit();

	Scene::Update(_fDeltaTime);

	/* ---------- 치트 ---------- */
	if (Input::GetKeyDown(DIK_NUMPAD8))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S03));
	}
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S04));
	}
	/* -------------------------- */
	if (_DecreaseHotel03_Volume)
		_Hotel03_Volume = FMath::Lerp(_Hotel03_Volume, 0.f, _fDeltaTime);
	else
		_Hotel03_Volume = FMath::Lerp(_Hotel03_Volume, 0.12f, _fDeltaTime * 0.5f);

	if (_DecreaseBattle1_Volume)
	{
		_Battle1_Volume = FMath::Lerp(_Battle1_Volume, 0.f, _fDeltaTime);
		SoundSystem::GetInstance()->Play("Battle3", _Battle1_Volume, false);
	}

	SoundSystem::GetInstance()->Play("Hotel03", _Hotel03_Volume, false);
	return S_OK;
}

HRESULT Hotel_S03::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void Hotel_S03::LoadObjects(const std::filesystem::path& path, const bool _bAni)
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

void Hotel_S03::LoadCollObjects(const std::filesystem::path& path)
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

void Hotel_S03::LoadBreakablebjects(const std::filesystem::path& path)
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

void Hotel_S03::RenderDataSetUp(const  bool bTest)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	if (bTest)
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	}
	else
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Hotel_S03.json");
	}

	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
	_Renderer->ao = 0.5f;
	_Renderer->SkyIntencity = 0.005f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f,0.f };
	_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f };
	_Renderer->SoftParticleDepthScale = 0.7f;
	_Renderer->SkyRotationSpeed = 1.5f;
	_Renderer->StarScale = 4.f;
	_Renderer->StarFactor = 0.9f;
};

void Hotel_S03::BgmPlay()
{
	// SoundSystem::GetInstance()->Play("Maple", 10.f, false, true);
};

void Hotel_S03::TriggerSetUp()
{
	TriggerUpGround();
	TriggerBeforeShop(TriggerShop(TriggerHole())); // ㅋㅋ
	TriggerNextScene();
};

void Hotel_S03::TriggerUpGround()
{
	auto _BattleTrigger = AddGameObject<Trigger>().lock();
	if (_BattleTrigger)
	{
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em100>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.653f, 0.182f, 26.911f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.207f, 0.182f, 26.821f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.511f, 0.182f, 27.558f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.246f, 0.205f, 28.158f });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -3.158700f, 0.565950f, 27.829210f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 100.f, 100.f, 100.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this]()
		{
			// + 카메라 연출?????

			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this]()
		{
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			}
		};

		_BattleTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	}

	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		auto _Smoke0 = AddGameObject<Smoke>();
		if (auto Sp = _Smoke0.lock(); Sp)
			Sp->SetActive(false);
		auto _Smoke1 = AddGameObject<Smoke>();
		if (auto Sp = _Smoke1.lock(); Sp)
			Sp->SetActive(false);

		auto _AnimationUpground = AddGameObject<AnimationUpGround>();
		const std::function<void()> _CallBack =
			[_AnimationUpground, _Smoke0, _Smoke1, _BattleTrigger,this]()
		{
			// 여기서 UpGround 로직 처리하세요 ... 
			_AnimationUpground.lock()->ContinueAnimation();
			_Player.lock()->GetFsm().lock()->ChangeState(NeroFSM::WINDPRESSURE);
			// 땅이 솟아오름 !! .. 
			SoundSystem::GetInstance()->Play("UpGround1", 0.6f, false);
			SoundSystem::GetInstance()->Play("UpGround2", 0.6f, false);
			//
			if (auto Sp = _Smoke0.lock(); Sp)
			{
				Sp->SetActive(true);
				Sp->SetPosition({ -2.4f, 0.1f, 27.8f });
				Sp->SetScale(0.008f);
				Sp->SetRotation({ 0.f, 344.681f, 0.f });
				Sp->SetVariationIdx(Smoke::VARIATION::SMOKE_1);
				Sp->PlayStart(7.3f);
			}
			if (auto Sp = _Smoke1.lock(); Sp)
			{
				Sp->SetActive(true);
				Sp->SetPosition({ -2.15f, 0.12f, 28.6f });
				Sp->SetScale(0.008f);
				Sp->SetRotation({ 0.f, 306.383f, 0.f });
				Sp->SetVariationIdx(Smoke::VARIATION::SMOKE_1);
				Sp->PlayStart(7.5f);
			}

			//
			_BattleTrigger->TriggerEnable();
		};
		
		// 트리거 위치
		const Vector3 TriggerLocation{ -3.158700f, 0.565950f, 27.829210f };
		const Vector3 TriggerRotation{ 0.f,0.f,0.f };

		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 1.705f, 1.0630f, 0.827000f };
		// 트리거 정보 등록하자마자 활성화 할까요 ?  
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag,
			TriggerRotation);
	}
}

void Hotel_S03::TriggerBeforeShop(const std::weak_ptr<class Trigger>& _NextTrigger)
{
	auto _BattleTrigger = AddGameObject<Trigger>().lock();
	if (_BattleTrigger)
	{
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.514f, 0.185f, 34.686f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.170f, 0.185f, 34.701f });

		// 트리거 위치
		const Vector3 TriggerLocation{ -0.23f, 0.0637f, 34.569f };
		const Vector3 TriggerRotation{ -451.22879f, 13.12937f, 0.00000f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize{ 1.354f, 1.873f, 1.608f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this]()
		{
			SoundSystem::GetInstance()->Play("BattleStart4", 1.f, true);
			SoundSystem::GetInstance()->Play("Em100Spawn", 0.7f, true);
			SoundSystem::GetInstance()->Play("Em100Spawn2", 0.6f, true);
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this, _NextTrigger]()
		{
			_NextTrigger.lock()->TriggerEnable();

			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			}
		};

		_BattleTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	}
};

std::weak_ptr<Trigger> Hotel_S03::TriggerShop(const std::weak_ptr<class Trigger>& _NextTrigger)
{
	auto _Trigger = AddGameObject<Trigger>().lock();
	if (_Trigger)
	{
		const std::function<void()> _CallBack =
			[this, _NextTrigger]()
		{
			// 임시
			_Player.lock()->BuyCbsLong();
			_Player.lock()->BuyCbsMiddle();
			_Player.lock()->BuyUpgradedOverture();
			//

			_NextTrigger.lock()->TriggerEnable();
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -2.406498f, 0.299f, 34.539822f };
		const Vector3 TriggerRotation{ -451.22879f, 13.12937f, 0.00000f };

		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 2.354f, 4.262f, 1.8820f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = false;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag,
			TriggerRotation);
	}

	return _Trigger;
};

std::weak_ptr<Trigger> Hotel_S03::TriggerHole()
{
	auto _SecondTrigger = AddGameObject<Trigger>().lock();
	if (_SecondTrigger)
	{
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.464f, -0.275f, 32.011f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.071f, -0.3f, 33.407f });

		const Vector3 TriggerLocation{ -0.23f, 0.0637f, 32.f };
		const Vector3 TriggerBoxSize{ 100.f, 100.f, 100.f };
		const bool ImmediatelyEnable = false;
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this]()
		{
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this]()
		{
			_DecreaseHotel03_Volume = false;
			_DecreaseBattle1_Volume = true;

			// 여기서 카메라 거미줄 비치기.
			_MainCamera.lock()->Set_PlayerCamMode(MainCamera::CAM_MODE_WAVE_END);

			// 여기서 거미줄 없애기. << 알아서 밖에서 지워주길 바람 - hscho
			m_vecQliphothBlock[0].lock()->Reset();
			m_vecQliphothBlock[1].lock()->Reset();
			m_vecQliphothBlock[2].lock()->Reset();


			// 여기서 하늘 왜곡 풀어주기 .
			Renderer::GetInstance()->SkyDistortionEnd();

			//
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

	auto _StartTrigger = AddGameObject<Trigger>().lock();
	if (_StartTrigger)
	{
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.464f, -0.275f, 32.011f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.028f, -0.339f, 31.884f });

		const Vector3 TriggerLocation{ -0.23f, 0.0637f, 32.f };
		const Vector3 TriggerBoxSize { 6.259999f, 1.f, 1.414996f };
		const bool ImmediatelyEnable = false;
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this]()
		{
			_DecreaseHotel03_Volume = true;
			SoundSystem::GetInstance()->Play("Battle3", _Battle1_Volume, false);
			SoundSystem::GetInstance()->Play("BattleStart1", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart2", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart4", 1.f, true);
			// 스카이 왜곡 시작 ...
			Renderer::GetInstance()->SkyDistortionStart();

			// 구덩이 길막
			m_vecQliphothBlock[1].lock()->SetActive(true);
			m_vecQliphothBlock[1].lock()->PlayStart();
			m_vecQliphothBlock[2].lock()->SetActive(true);
			m_vecQliphothBlock[2].lock()->PlayStart();

			//
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[_SecondTrigger]()
		{
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
	}

	return _StartTrigger;
};

void Hotel_S03::TriggerNextScene()
{
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
				_FadeOut->PlayStart(2u,
					[SpPanel]()
					{
						SpPanel->SetNullBlackActive(true);
						SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S04));
					});
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -4.610800f, 0.765100f, 36.625561f };
		const Vector3 TriggerRotation{ -451.22879f, 13.12937f, 0.00000f };

		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 4.f,1.f,1.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag,
			TriggerRotation);
	}
};

void Hotel_S03::LateInit()
{
	//SoundSystem::GetInstance()->ClearSound();


	// + 플레이어 초기 위치 잡기 등
	if (_Player.expired() == false)
	{
		_Player.lock()->GetComponent<Transform>().lock()->SetPosition
		({ -1.77158f, 1.36541f, 23.86f });
		_Player.lock()->SetAngle(180.f);
	}

	if (auto SpMainCamera = _MainCamera.lock();
		SpMainCamera)
	{
		SpMainCamera->SetAngle({ -5.f,170.f,0.f });
		SpMainCamera->SetStartPos();
	}

	Renderer::GetInstance()->LateSceneInit();

	BgmPlay();

	_LateInit = true;
}