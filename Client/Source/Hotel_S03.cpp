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

#include <iostream>
#include <fstream>
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
	//		Vector3{ -1.77158f, 1.36541f, 23.73719 }
	//	);
	//}

	AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage3_Map.json");
	LoadObjects("../../Data/Stage3_AniObject.json", true);
	LoadCollObjects("../../Data/Stage3_Object.json");
	LoadBreakablebjects("../../Data/Stage3_BreakableObject.json");

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
	// 상점 터치다운 -> 배틀 트리거 켜기
	TriggerShop(TriggerBattleStart());
	TriggerNextScene();
};

void Hotel_S03::TriggerUpGround()
{
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[]()
		{
			// 여기서 UpGround 로직 처리하세요 ... 

			// 땅이 솟아오름 !! .. 
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.158700f ,0.565950f,27.829210f};
		const Vector3 TriggerRotation{ 0.f ,0.f,0.f };

		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 1.705f,1.0630f,0.827000f};
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
		const Vector3 TriggerLocation{
			-4.610800f, 1.59400f, 36.717659f };
		const Vector3 TriggerRotation{ -451.22879f, 13.12937f, 0.00000f };
		;
		// -4.55710 1.59400 37.21000
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

void Hotel_S03::TriggerShop(const std::weak_ptr<class Trigger>& _BattleTrigger)
{
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[_BattleTrigger]()
		{
			_BattleTrigger.lock()->TriggerEnable();
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -2.406498f,0.299f,34.539822f };
		const Vector3 TriggerRotation{ -451.22879f, 13.12937f, 0.00000f };
		;
		// -4.55710 1.59400 37.21000
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 2.354f, 4.262f, 1.8820f };
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
	};
};

std::weak_ptr<Trigger> Hotel_S03::TriggerBattleStart()
{
	auto _ThirdTrigger = AddGameObject<Trigger>().lock();
	if (_ThirdTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.791f , 0.239, 28.937f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.164f, 0.293f, 29.090f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.517f, 0.256f, 26.847f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.586, 0.337, 27.124 });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation
			{ -2.792f, 0.409f, 27.183f};
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = 
				{ 2.805f,1.063000f,1.419001f};

		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			Renderer::GetInstance()->SkyDistortionEnd();
			//... 여기서 로직 처리하세요 . 
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */
			]()
		{
				// 여기서 거미줄 없애기.
				// 여기서 카메라 거미줄 비치기.
				// 여기서 하늘 왜곡 풀어주기 .
				
		};

		_ThirdTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	}

	auto _SecondTrigger = AddGameObject<Trigger>().lock();
	if (_SecondTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.791f , 0.239, 28.937f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.164f, 0.293f, 29.090f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.517f, 0.256f, 26.847f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.586, 0.337, 27.124 });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation
				{ -1.144f, 0.734000f ,26.896f};
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize
						{ 6.259999f,1.f,1.414996f};
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */ ,
			_ThirdTrigger]()
		{
			//... 여기서 로직 처리하세요 . 
			_ThirdTrigger->TriggerEnable();
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

	// 트리거 생성 !! 
	std::shared_ptr<Trigger> _StartTrigger{};
	if (_StartTrigger = AddGameObject<Trigger>().lock();
		_StartTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.791f , 0.239f, 28.937f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.164f, 0.293f, 29.090f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.517f, 0.256f, 26.847f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.586, 0.337, 27.124 });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation
			{ 0.198f, 0.662000f,30.821f};
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 1.705f,1.f,0.827000f};
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			// 스카이 왜곡 시작 ...
			Renderer::GetInstance()->SkyDistortionStart();
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */,
			_SecondTrigger]()
		{
			//... 여기서 로직 처리하세요 . 
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

void Hotel_S03::TriggerFirstBattle()
{
	auto _SecondTrigger = AddGameObject<Trigger>().lock();
	if (_SecondTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.791f , 0.239, 28.937f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.164f, 0.293f, 29.090f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.517f, 0.256f, 26.847f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.586, 0.337, 27.124 });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -1.77158f, 1.36541f, 23.73719f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 100.f,100.f,100.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			//... 여기서 로직 처리하세요 . 
			// 여기서 카메라 연출 하세요 .

			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			};
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

	// 트리거 생성 !! 
	std::shared_ptr<Trigger> _StartTrigger{};
	if (_StartTrigger = AddGameObject<Trigger>().lock();
		_StartTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.791f , 0.239f, 28.937f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.164f, 0.293f, 29.090f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.517f, 0.256f, 26.847f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.586, 0.337, 27.124 });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -0.463250f, 0.734000f ,27.396049f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 1.f,1.f,1.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */,
			_SecondTrigger]()
		{
			//... 여기서 로직 처리하세요 . 
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
};

void Hotel_S03::TriggerBattleInFrontShop()
{
	auto _SecondTrigger = AddGameObject<Trigger>().lock();
	if (_SecondTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.331f, 0.232f, 34.746f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.325f, 0.213f, 34.183f});

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.156f, 0.206f, 34.152f});

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.197f, 0.188f, 34.674f});

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ 0,0,0  };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 100.f , 100.f, 100.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			//... 여기서 로직 처리하세요 . 
			// 여기서 카메라 연출 하세요 .
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			};
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

	// 트리거 생성 !! 
	std::shared_ptr<Trigger> _StartTrigger{};
	if (_StartTrigger = AddGameObject<Trigger>().lock();
		_StartTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em100>(),
			AddGameObject<Em100>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.331f, 0.232f, 34.746f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.325f, 0.213f, 34.183f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.156f, 0.206f, 34.152f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.197f, 0.188f, 34.674f });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation
				{  -2.406498f,0.299f,34.539822f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = 
			{ 2.354f, 4.262f, 1.8820f};
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */,
			_SecondTrigger]()
		{
			//... 여기서 로직 처리하세요 . 
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
};

void Hotel_S03::LateInit()
{
	// + 플레이어 초기 위치 잡기 등
	if (_Player.expired() == false)
	{
		_Player.lock()->GetComponent<Transform>().lock()->SetPosition
		({ -1.77158f, 1.36541f, 23.73719f });
	}

	Renderer::GetInstance()->LateSceneInit();

	BgmPlay();

	_LateInit = true;
}