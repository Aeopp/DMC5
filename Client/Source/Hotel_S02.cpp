#include "stdafx.h"
#include "..\Header\Hotel_S02.h"
#include "LoadingScene.h"
#include "PreLoader.h"
#include "TempMap.h"
#include "Nero.h"
#include "BtlPanel.h"
#include "Camera.h"
#include "MainCamera.h"
#include "Renderer.h"
#include "MapObject.h"
#include "SecretVision.h"
#include "NhDoor.h"
#include "MakaiButterfly.h"
#include "Em100.h"
#include "Trigger.h"
#include "FadeOut.h"
#include "CollObject.h"
#include "BreakableObject.h"
#include "HotelBrokenFloor.h"
#include "HotelAnimationWall.h"
#include "SoundSystem.h"
#include "NeroFSM.h"
#include "Smoke.h"
#include "QliphothBlock.h"
#include "TimeSystem.h"
#include "Em0000.h"
#include "ShopPanel.h"

#include <iostream>
#include <fstream>
using namespace std;

Hotel_S02::Hotel_S02()
{

}

void Hotel_S02::Free()
{
	for (auto& Element : _MakaiButterflyVec)
		Destroy(Element);
	_MakaiButterflyVec.clear();
	_MakaiButterflyVec.shrink_to_fit();

	for (auto& Element : m_vecQliphothBlock)
		Destroy(Element);
	m_vecQliphothBlock.clear();
	m_vecQliphothBlock.shrink_to_fit();

	Scene::Free();
}

Hotel_S02* Hotel_S02::Create()
{
	Hotel_S02* pInstance = new Hotel_S02;
	return pInstance;
}

HRESULT Hotel_S02::LoadScene()
{
	// Load Start
	SoundSystem::GetInstance()->ClearSound();
	SoundSystem::GetInstance()->Stop("Hotel01");
	SoundSystem::GetInstance()->Play("Hotel02", _Hotel02_Volume, false);
	
	m_fLoadingProgress = 0.01f;

#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera

	//if (auto SpCamera = AddGameObject<Camera>().lock();
	//	SpCamera)
	//{
	//	SpCamera->GetComponent<Transform>().lock()->SetPosition(Vector3{ -3.808f, 0.296f, 11.846f });
	//}
	
	_MainCamera = AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage2_Map.json");
	LoadCollObjects("../../Data/Stage2_Object.json");
	LoadBreakablebjects("../../Data/Stage2_BreakableObject.json");

	AddGameObject<HotelBrokenFloor>();

	auto Map = AddGameObject<TempMap>().lock();
	Map->LoadMap(2);

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger

	RenderDataSetUp(false);
	TriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect

	// Stage2 길막
	m_vecQliphothBlock.reserve(3);

	// 0: StartPoint 
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.016f);
		ptr.lock()->SetRotation(Vector3(0.f, 0.f, 0.f));
		ptr.lock()->SetPosition(Vector3(-3.631f, 0.4f, 11.43f));
		ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	// 1 ~ 2: 마지막 방
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.002f);
		ptr.lock()->SetRotation(Vector3(0.f, 180.f, 0.f));
		ptr.lock()->SetPosition(Vector3(-3.3f, 1.47f, 21.96f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.003f);
		ptr.lock()->SetRotation(Vector3(0.f, 0.f, 0.f));
		ptr.lock()->SetPosition(Vector3(-1.718f, 1.52f, 23.42f));
		ptr.lock()->SetActive(false);
		//ptr.lock()->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	}

	//
	_SecretVision = AddGameObject<SecretVision>();
	AddGameObject<NhDoor>();

	// 
	size_t ButterflyCnt = 5u;
	_MakaiButterflyVec.reserve(ButterflyCnt);
	for (size_t i = 0u ; i < ButterflyCnt; ++i)
		_MakaiButterflyVec.push_back(AddGameObject<MakaiButterfly>());

#pragma endregion

	m_fLoadingProgress = 0.8f;

#pragma region UI

	_BtlPanel = AddGameObject<BtlPanel>();

	_ShopPanel = AddGameObject<ShopPanel>();
	if (auto Sp = _ShopPanel.lock(); Sp)
	{
		Sp->ResetCmd();
		Sp->SetActive(false);
	}

#pragma endregion

	m_fLoadingProgress = 0.9f;

#pragma region Misc

#pragma endregion

	// Load Complete
	m_fLoadingProgress = 1.f;

	return S_OK;
}

HRESULT Hotel_S02::Awake()
{
	Scene::Awake();
	return S_OK;
}

HRESULT Hotel_S02::Start()
{
	Scene::Start();
	return S_OK;
}

HRESULT Hotel_S02::Update(const float _fDeltaTime)
{
	if (!_LateInit)
		LateInit();

	Scene::Update(_fDeltaTime);

	CheckShopAvailable();

	/* ---------- 치트 ---------- */
	if (Input::GetKeyDown(DIK_NUMPAD8))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S02));
	}
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S03));
	}
	/* -------------------------- */
	
	if (_DecreaseHotel02_Volume)
		_Hotel02_Volume = FMath::Lerp(_Hotel02_Volume, 0.f, _fDeltaTime);
	else
		_Hotel02_Volume = FMath::Lerp(_Hotel02_Volume, 0.12f, _fDeltaTime * 0.5f);

	if (_DecreaseBattle1_Volume)
	{
		_Battle1_Volume = FMath::Lerp(_Battle1_Volume, 0.f, _fDeltaTime);
		SoundSystem::GetInstance()->Play("Battle1", _Battle1_Volume, false);
	}

	SoundSystem::GetInstance()->Play("Hotel02", _Hotel02_Volume, false);

	return S_OK;
}

HRESULT Hotel_S02::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void Hotel_S02::LoadObjects(const std::filesystem::path& path)
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

void Hotel_S02::LoadCollObjects(const std::filesystem::path& path)
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

void Hotel_S02::LoadBreakablebjects(const std::filesystem::path& path)
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

void Hotel_S02::RenderDataSetUp(const bool bTest)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	if (bTest)
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	}
	else
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Hotel_S02.json");
	}
	
	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
	_Renderer->ao = 0.01f;
	_Renderer->SkyIntencity = 0.005f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f,0.f };
	_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f };
	_Renderer->SoftParticleDepthScale = 0.7f;
	_Renderer->SkyRotationSpeed = 1.5f;
	_Renderer->StarScale = 4.f;
	_Renderer->StarFactor = 0.9f;
}

void Hotel_S02::TriggerSetUp()
{
	TriggerFirstButterFlyMeetCamera(TriggerFirstButterFlyMeet());
	TriggerPuzzleStart();
	TriggerWallSmash();
	TriggerLastRoomBattle(TriggerNextScene());
}

void Hotel_S02::TriggerWallSmash()
{
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		auto _AnimationWall = AddGameObject<HotelAnimationWall>();
		auto _Smoke0 = AddGameObject<Smoke>();
		if (auto Sp = _Smoke0.lock(); Sp)
			Sp->SetActive(false);
		auto _Smoke1 = AddGameObject<Smoke>();
		if (auto Sp = _Smoke1.lock(); Sp)
			Sp->SetActive(false);

		const std::function<void()> _CallBack =
			[this, _AnimationWall, _Smoke0, _Smoke1]()
		{
			// 여기서 성큰이 벽을 박살내며 등장 !!
			_AnimationWall.lock()->ContinueAnimation();
			SoundSystem::GetInstance()->Play("Explosion1", 0.7f, false);
			SoundSystem::GetInstance()->Play("Stone2", 0.7f, false);
			//
			if (auto Sp = _Player.lock(); Sp)
				Sp->GetFsm().lock()->ChangeState(NeroFSM::CUTSCENE_WINDPRESSURE);

			//
			if (auto Sp = _Smoke0.lock(); Sp)
			{
				Sp->SetActive(true);
				Sp->SetPosition({ -2.15f, 1.48f, 20.85f });
				Sp->SetScale(0.008f);
				Sp->SetRotation({0.f, 344.681f, 0.f});
				Sp->SetVariationIdx(Smoke::VARIATION::SMOKE_1);
				Sp->PlayStart(9.6f);
			}
			if (auto Sp = _Smoke1.lock(); Sp)
			{
				Sp->SetActive(true);
				Sp->SetPosition({ -2.15f, 1.48f, 20.78f });
				Sp->SetScale(0.008f);
				Sp->SetRotation({ 0.f, 344.681f, 0.f });
				Sp->SetVariationIdx(Smoke::VARIATION::SMOKE_1);
				Sp->PlayStart(9.4f);
			}

			//
			for (auto& Element : _MakaiButterflyVec)
				Element.lock()->SetActive(false);
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.108f, 1.564f, 19.806705f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 1.f, 0.5f, 0.1f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}
}

void Hotel_S02::TriggerFirstButterFlyMeetCamera(const std::weak_ptr<Trigger>& _NextTrigger)
{
	// 이건 일반 트리거 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[_NextTrigger, this]()
		{
			//vector<Vector3> _LostTimes;
			//_LostTimes.emplace_back(Vector3(2.f, 1.f, 0.3f));
			//TimeSystem::GetInstance()->LostTime(_LostTimes);
			Renderer::GetInstance()->FadeOutStart(0.5f);
			if (!_MainCamera.expired())
			{
				_MainCamera.lock()->Set_Trigger(_NextTrigger);
				_MainCamera.lock()->SetFadeSceneInfo(0.5f);
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.786f, 0.592f, 12.665f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 2.f, 1.f, 1.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(
			_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	};
}

std::weak_ptr<Trigger> Hotel_S02::TriggerFirstButterFlyMeet()
{
	auto _Trigger = AddGameObject<Trigger>().lock();
	if (_Trigger)
	{
		const std::function<void()> _CallBack =
			[this]()
		{
			// 여기서 퍼즐용 나방과 첫 조우 연출 !

			if (auto SpObject = _MakaiButterflyVec[0].lock();
				SpObject)
			{
				SpObject->SetActive(true);
				SpObject->SetRotation({ 0.f, 90.f, 0.f });
				SpObject->SetPosition({ -3.808f, 0.886f, 14.175f });
				SpObject->PlayStart();

				if (!_MainCamera.expired())
				{
					_MainCamera.lock()->Set_TriggerCam(MainCamera::STAGE2_BUTTERFLY1, { -3.808f, 0.886f, 14.175f }, 2.f);
					_MainCamera.lock()->Set_At_Transform(SpObject->GetComponent<Transform>(), MainCamera::AT_TRIGGER);
				}
			}
		};
		
		// 트리거 위치
		const Vector3 TriggerLocation{ -3.786f, 0.592f, 12.665f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 2.f, 1.f, 1.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = false;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(
			_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}

	return _Trigger;
}

void Hotel_S02::TriggerPuzzleStart()
{
	auto _SecondTrigger = AddGameObject<Trigger>().lock();
	if (_SecondTrigger)
	{
		const std::function<void()> _CallBack =
			[this]()
		{
			if (auto SpObject = _MakaiButterflyVec[1].lock();
				SpObject)
			{
				SpObject->SetActive(true);
				SpObject->SetRotation({ 0.f, 270.f, 0.f });
				SpObject->SetPosition({ -3.163f, 1.555f, 15.013f });
				SpObject->PlayStart();
			}

			for (uint32 i = 2u; i < 5u; ++i)
			{
				if (auto SpObject = _MakaiButterflyVec[i].lock();
					SpObject)
				{
					SpObject->SetActive(true);
					SpObject->PlayStart();
				}
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.3f, 1.565f, 16.13f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 0.5f, 0.5f, 0.1f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = false;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_SecondTrigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}

	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[this, _SecondTrigger]()
		{
			_SecretVision.lock()->PuzzleStart();

			_SecondTrigger->TriggerEnable();
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.3f, 1.565f, 17.f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 0.5f, 0.5f, 0.1f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}
};

void Hotel_S02::TriggerLastRoomBattle(const std::weak_ptr<Trigger>& _NextSceneTrigger)
{
	// 트리거 생성 !! 
	std::shared_ptr<Trigger> _BattleTrigger{};
	if (_BattleTrigger = AddGameObject<Trigger>().lock();
		_BattleTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			//AddGameObject<Em0000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.313f, 1.367f, 23.006f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.176f, 1.367f, 22.574f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.126f, 1.367f, 22.957f });

		//MonsterWave[3].lock()->GetComponent<Transform>().
		//	lock()->SetPosition({ -3.180f, 1.367f, 22.462f });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -2.573f, 1.820050f, 22.48850f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 2.524000f, 1.f, 0.894000f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this]()
		{
			//
			_DecreaseHotel02_Volume = true;
			SoundSystem::GetInstance()->Play("Battle1", _Battle1_Volume, false);
			SoundSystem::GetInstance()->Play("BattleStart1", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart2", 1.f, true);
			SoundSystem::GetInstance()->Play("BattleStart4", 1.f, true);
			SoundSystem::GetInstance()->Play("Em100Spawn", 1.f, true);

			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			};

			//
			auto PtLightDecrease = [](std::weak_ptr<FLight> TargetLight)
			{
				if (auto SpTargetLight = TargetLight.lock();
					SpTargetLight)
				{
					SpTargetLight->lightFlux *= 0.3f;
				}
			};

			PtLightDecrease(Renderer::GetInstance()->RefPointLights(15));
			PtLightDecrease(Renderer::GetInstance()->RefPointLights(16));

			//
			for (uint32 i = 1u; i < 3u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->SetActive(true);
					m_vecQliphothBlock[i].lock()->PlayStart();
				}
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this, _NextSceneTrigger]()
		{ 
			//if (_MainCamera.expired() == false)
			//{
			//	_MainCamera.lock()->Set_PlayerCamMode(MainCamera::CAM_MODE_WAVE_END);
			//}
			_DecreaseHotel02_Volume = false;
			_DecreaseBattle1_Volume = true;
			//
			for (uint32 i = 1u; i < 3u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->Reset();
				}
			}

			//
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				//Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			}

			//
			if (_NextSceneTrigger.expired() == false)
			{
				_NextSceneTrigger.lock()->TriggerEnable();
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
	};
};


std::weak_ptr<Trigger> Hotel_S02::TriggerNextScene()
{
	auto _Trigger = AddGameObject<Trigger>().lock();
	if (_Trigger)
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
				_FadeOut->PlayStart(1u,
					[SpPanel]()
					{
						SpPanel->SetNullBlackActive(true);
						SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S03));
					});
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -1.516f, 1.751f, 23.954f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 4.f, 1.f, 1.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = false;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}

	return _Trigger;
}

void Hotel_S02::BgmPlay()
{
	// SoundSystem::GetInstance()->Play("Maple", 10.f, false, true);
}

void Hotel_S02::ApplyShopUpgradeDesc()
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

	if (auto SpBtlPanel = _BtlPanel.lock();
		SpBtlPanel)
	{
		SpBtlPanel->SetExGaugeLevel(UpgradeDesc._ExgaugeUpUpgradeCount);
	}
}

void Hotel_S02::CheckShopAvailable()
{
	if (_IsShopAvailable && Input::GetKeyDown(DIK_P))
	{
		if (auto Sp = _ShopPanel.lock(); Sp)
		{
			if (!Sp->IsActive())
			{
				Sp->SetActive(true);
				_BtlPanel.lock()->SetActive(false);
			}
			else
			{
				ApplyShopUpgradeDesc();
				Sp->ResetCmd();
				Sp->SetActive(false);
				_BtlPanel.lock()->SetActive(true);
			}
		}
	}
}

void Hotel_S02::LateInit()
{
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({ -3.63097f, 0.077f, 11.75365f });
		SpPlayer->SetAngle(180.f);
	}

	ApplyShopUpgradeDesc();

	if (auto SpMainCamera = _MainCamera.lock();
		SpMainCamera)
	{
		SpMainCamera->SetLerp(false);
		SpMainCamera->SetAngle({ 0.f,180.f,0.f });
		SpMainCamera->SetDistance(0.33f);
		SpMainCamera->SetStartPos();
	}

	//
	if (auto SpObject = _MakaiButterflyVec[0].lock();
		SpObject)
	{
		// 첫번째 조우 나비
		SpObject->SetVariationIdx(MakaiButterfly::VARIATION::MOVE_FORWARD);
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[1].lock();
		SpObject)
	{
		// 두번째 조우 나비
		SpObject->SetVariationIdx(MakaiButterfly::VARIATION::MOVE_FORWARD);
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[2].lock();
		SpObject)
	{
		// 퍼즐용 나비
		SpObject->SetPosition({ -4319.212f * GScale, 340.248f * GScale, 16593.594f * GScale });
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[3].lock();
		SpObject)
	{
		// 퍼즐용 나비
		SpObject->SetPosition({ -804.781f * GScale, 967.678f * GScale, 16376.986f * GScale });
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[4].lock();
		SpObject)
	{
		// 퍼즐용 나비
		SpObject->SetPosition({ -4696.414f * GScale, 1553.205f * GScale, 15569.233f * GScale });
		SpObject->SetActive(false);
	}

	MakaiButterfly::ResetTotalCnt();

	Renderer::GetInstance()->LateSceneInit();

	BgmPlay();

	_LateInit = true;
}