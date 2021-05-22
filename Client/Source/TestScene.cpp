#include "stdafx.h"
#include "..\Header\TestScene.h"
#include "Scene.h"
#include "TestObject.h"
#include "ShaderTester.h"
#include "TempMap.h"
#include "PreLoader.h"
#include "TestAnimationObject.h"
#include "Camera.h"
#include "Nero.h"
#include "RedQueen.h"
#include "Em100.h"
#include "Em0000.h"
#include "Em0000_Weapon.h"
#include "Em5000.h"
#include "Em1000.h"
#include "Em5300.h"
#include "Em200.h"
#include "Car.h"
#include "OvertureHand.h"
#include "Glint.h"
#include "Liquid.h"
#include "WhirlWind.h"
#include "AppearGroundMonster.h"
#include "Smoke.h"
#include "QliphothBlock.h"
#include "BtlPanel.h"
#include "MainCamera.h"
#include "Renderer.h"
#include "MapObject.h"
#include "CircleWave.h"
#include "AirHike.h"
#include "FireCircle.h"
#include "IceAge.h"
#include "JudgementSword.h"
#include "CbsTrail.h"
#include "ElectricOccur.h"
#include "Thunderbolt.h"
#include "ElectricVortex.h"
#include "ThunderboltSecond.h"
#include "ElectricBranch.h"
#include "AirHike.h"
#include "CbsMidTrail.h"
#include "BlitzAttack.h"
#include "Trigger.h"
#include "SecretVision.h"
#include "MakaiButterfly.h"
#include "CbsLongTrail.h"
#include "Smoke.h"
#include "NhDoor.h"
#include "ShopPanel.h"
#include "StoneDebris.h"
#include "TimeSystem.h"
#include "ShockWave.h"
#include "Trail.h"
#include "Change.h"
#include "SpriteEffect.h"
#include "SoundSystem.h"
#include "ShockWave.h"
#include "ArtemisMissile.h"
#include "LensFlare.h"

#include <iostream>
#include <fstream>
using namespace std;

TestScene::TestScene()
{
	pPlane = nullptr;
}

void TestScene::Free()
{
	Scene::Free();
}

TestScene* TestScene::Create()
{
	TestScene* pInstance = new TestScene;
	return pInstance;
};


HRESULT TestScene::LoadScene()
{
	// Load Start

	m_fLoadingProgress = 0.01f;

	AddGameObject<Change>();
	AddGameObject<ShockWave>();

	for (int32 i = 0; i < 16; ++i)
	{
		AddGameObject<ArtemisMissile>();
	}

	AddGameObject<LensFlare>();

	for (int i = 0; i < 6; ++i)
	{
		if (auto _SpriteEffect = AddGameObject<SpriteEffect>().lock();
			_SpriteEffect)
		{
			_SpriteEffect->InitializeFromOption(i);
		}
	}
#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera

	_Camera = AddGameObject<Camera>();
	
	/*_MainCamera = AddGameObject<MainCamera>();
	_Player     = AddGameObject<Nero>();*/
#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

	//AddGameObject<Em0000>();
	//AddGameObject<Em1000>();
	//AddGameObject<Em5300>();
	//AddGameObject<Em5000>();

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map

	LoadMap();
	auto Map = AddGameObject<TempMap>().lock();
	Map->LoadMap(1);

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger
	RenderDataSetUp(false);
	//TriggerSetUp();
	//MonsterWaveTriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect
	//if (auto Sp = AddGameObject<StoneDebris>().lock();
	//	Sp)
	//{
	//	Sp->SetLoop(true);
	//	Sp->PlayStart();
	//}

	AddGameObject<MakaiButterfly>();

#pragma endregion

	m_fLoadingProgress = 0.8f;

#pragma region UI

	AddGameObject<BtlPanel>();

	//AddGameObject<BtlPanel>().lock()->SetActive(false);
	//AddGameObject<ShopPanel>();

#pragma endregion

	m_fLoadingProgress = 0.9f;

#pragma region Misc

	// DOOMSDAY
	if (auto pFont = AddGameObject<Font>().lock();
		pFont)
	{
		pFont->SetText("D 5, Until Dooms Day",
			Font::TEX_ID::DMC5_BLACK_GRAD,
			Vector2(505.f, 40.f),
			Vector2(0.6f, 0.6f),
			Vector3(1.f, 1.f, 1.f),
			true);

		pFont->SetRenderFlag(true);
	}

#pragma endregion

	// Load Complete
	m_fLoadingProgress = 1.f;

	if (_Camera.expired() == false)
	{
		SoundSystem::GetInstance()->SetDisanceDecrease(0.1f, 3.f, _Camera.lock()->GetComponent<Transform>());
	};

	return S_OK;
}

HRESULT TestScene::Awake()
{
	Scene::Awake();

	//if (nullptr != pPlane)
	//	return S_OK;

	//pPlane = PxCreatePlane(*Physics::GetPxPhysics(), PxPlane(0.f, 1.f, 0.f, 0.f), *Physics::GetDefaultMaterial());
	//Physics::AddActor(UniqueID, *pPlane);

	return S_OK;
}

HRESULT TestScene::Start()
{
	Scene::Start();
	return S_OK;
}

HRESULT TestScene::Update(const float _fDeltaTime)
{
	Scene::Update(_fDeltaTime);

	//if (auto SpPlayer = _Player.lock();
	//	SpPlayer)
	//{
	//	SpPlayer->GetComponent<Transform>().lock()->SetPosition(Vector3{ 0.f,0.2f,0.f });
	//}

	//cout << "SceneUpdate" << endl;

	/*auto _RefParticles = ParticleSystem::GetInstance()->PlayableParticles("Ice", 3.f);
	for (auto& _PlayInstance : _RefParticles)
	{
		_PlayInstance->LifeTime = 3.f;
		_PlayInstance->Scale = { GScale,GScale,GScale };
		_PlayInstance->CurveControlPoints = {};
		_PlayInstance->CurveControlRotationPoints = {};
	}*/

	return S_OK;
}

HRESULT TestScene::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
};

void TestScene::LoadMap()
{
	// std::ifstream inputStream{ "../../Data/Hotel.json" };
	std::ifstream inputStream{ "..\\..\\Data\\Stage1_Map.json" };

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
			vScale.x = scale[0].GetDouble();
			vScale.y = scale[1].GetDouble();
			vScale.z = scale[2].GetDouble();

			D3DXVECTOR3 vRotation;
			auto rotation = iterObject->FindMember("Rotation")->value.GetArray();
			vRotation.x = rotation[0].GetDouble();
			vRotation.y = rotation[1].GetDouble();
			vRotation.z = rotation[2].GetDouble();

			D3DXVECTOR3 vPosition;
			auto position = iterObject->FindMember("Position")->value.GetArray();
			vPosition.x = position[0].GetDouble();
			vPosition.y = position[1].GetDouble();
			vPosition.z = position[2].GetDouble();

			pMapObject.lock()->SetUp(sFullPath, vScale, vRotation, vPosition);
		}
	}
};

void TestScene::RenderDataSetUp(const bool bTestLight)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	
	if (bTestLight)
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
		_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
		_Renderer->ao = 0.0005f;
		_Renderer->SkyIntencity = 0.005f;
		_Renderer->SkysphereScale = 0.078f;
		_Renderer->SkysphereRot = { 0.f,0.f,0.f };
		_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f };
		_Renderer->SoftParticleDepthScale = 0.7f;
		_Renderer->SkyRotationSpeed = 1.5f;
		_Renderer->StarScale = 4.f;
		_Renderer->StarFactor = 0.9f;
	}
	else
	{
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
	

	Renderer::GetInstance()->LateSceneInit();
}

void TestScene::TriggerSetUp()
{
	// 이건 일반 트리거 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[/*변수 캡쳐 (되도록 포인터로 ) */this]()
		{
			// 로직 .. 
			vector<Vector3> _LostTimes;
			_LostTimes.emplace_back(Vector3{ 3.f,1.f,0.5f });
			TimeSystem::GetInstance()->LostTime(_LostTimes);
			_MainCamera.lock()->Set_TriggerCam(MainCamera::STAGE1_WAVE1, Vector3(0.338f, 1.037f, 0.524f), 3.f);
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -0.66720f,0.01168f,-2.18399f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 1.f,1.f,1.f };
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


void TestScene::MonsterWaveTriggerSetUp()
{
	// 트리거 생성 !! 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
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
			lock()->SetPosition({ -0.93355f, 0.02f, -1.60137f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ 0.88708f, 0.02f, -0.92085f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.75695f, 0.02f, -0.34596f });

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -0.54699f, 0.02f, -2.37278f });

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -0.66720f,0.01168f,-2.18399f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 1.f,1.f,1.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */this]()
		{
			//... 여기서 로직 처리하세요 . 

		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */this]()
		{
			//... 여기서 로직 처리하세요 . 
			_MainCamera.lock()->Set_PlayerCamMode(MainCamera::CAM_MODE_WAVE_END);
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
};

