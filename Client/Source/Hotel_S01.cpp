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
	m_fLoadingProgress = 0.01f;

#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera

	//AddGameObject<Camera>();

	AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage1_Map.json");
	LoadObjects("../../Data/Stage1_Object.json");
	LoadObjects("../../Data/Stage1_AniObject.json", true);

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

	// Stage2 길막
	m_vecQliphothBlock.reserve(7);

	// 0: 시작지점 뒤
	if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
		!ptr.expired())
	{
		ptr.lock()->SetScale(0.016f);
		ptr.lock()->SetRotation(Vector3(0.f, 262.286f, 0.f));
		ptr.lock()->SetPosition(Vector3(-5.7f, 0.286f, -5.05f));
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

	// 테스트용 ////////////////////////
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		Renderer::GetInstance()->CurDirLight = nullptr;
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S02));
	}
	////////////////////////////////////

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

void Hotel_S01::RenderDataSetUp(const bool bTest)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
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

void Hotel_S01::TriggerSetUp()
{
	Trigger1st();
	Trigger2nd();
	Trigger3rd();
	Trigger4st();
};

void Hotel_S01::Trigger1st()
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
		const Vector3 TriggerBoxSize = { 10.f,10.f,10.f};
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			//... 여기서 로직 처리하세요 . 
			if (auto Sp = _BtlPanel.lock(); Sp)
				Sp->ResetRankScore();

			for (uint32 i = 1u; i < 4u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->Reset();
				}
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
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 

			for (uint32 i = 1u; i < 4u; ++i)
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
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */,
			_SecondTrigger]()
		{
			//... 여기서 로직 처리하세요 . 

			_SecondTrigger->TriggerEnable();

			// 여기서 카메라 연출 하세요 .

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

}

void Hotel_S01::Trigger2nd()
{
	std::shared_ptr<Trigger> _Trigger{};

	if (_Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.19094f, 0.009f, 5.34162f});

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.99099f, 0.009f, 6.28210f});

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -2.37271f, 0.009f, 5.71682f});

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.52741f, 0.009f, 6.39310f});

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -3.1017f, 0.011680f, 5.662048f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 6.f, 3.f, 1.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 

			Renderer::GetInstance()->SkyDistortionStart();
		
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

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			if (auto Sp = _BtlPanel.lock(); Sp)
				Sp->ResetRankScore();

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

void Hotel_S01::Trigger3rd()
{
	// 트리거 생성 !! 
	auto  _EndTrigger = AddGameObject<Trigger>().lock();
	if (_EndTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.06028f, 0.01f, 10.42297f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.40446f, 0.01f, 10.4338f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.430736f, 0.01f, 8.29934f });
			
		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -5.1670f , -0.003732f , 7.915854f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 4.f,5.f,1.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			//... 여기서 로직 처리하세요 . 
			if (auto Sp = _BtlPanel.lock(); Sp)
				Sp->ResetRankScore();

			for (uint32 i = 5u; i < 7u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->Reset();
				}
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

	if (auto _StartTrigger = AddGameObject<Trigger>().lock();
		_StartTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -3.06028, 0.01f, 10.42297f });

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.40446f, 0.01f, 10.4338f });

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -4.430736f, 0.01f, 8.29934f });


		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -5.1670f , -0.003732f , 7.915854f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 4.f,5.f,1.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 로직 처리하세요 . 

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

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */,
			_EndTrigger]()
		{
			//... 여기서 로직 처리하세요 . 
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


}

void Hotel_S01::Trigger4st()
{
	// 이건 일반 트리거 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[/*변수 캡쳐*/]()
		{
			// 여기서 씬전환 !! 
			SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S02));
		};
		
		// 트리거 위치
		const Vector3 TriggerLocation{ -3.171700f, 0.011680f, 12.167461f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 6.f,5.f,1.f };
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

void Hotel_S01::LateInit()
{
	// + 플레이어 초기 위치 잡기 등
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition
											({ -4.8f, -0.2f, -5.02f });
	}

	// 맵오브젝트가 로딩된 시점 (맵 오브젝트는 정적으로 움직이지 않음) (그림자맵 굽기)
	Renderer::GetInstance()->LateSceneInit();

	_LateInit = true;
}
