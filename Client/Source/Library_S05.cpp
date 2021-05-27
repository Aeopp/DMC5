#include "stdafx.h"
#include "..\Header\Library_S05.h"
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
#include "BreakableObject.h"
#include "CollObject.h"
#include "Monster.h"
#include "SoundSystem.h"
#include "ShopPanel.h"
#include "Em0000.h"
#include "Em100.h"
#include "Em200.h"
#include "Em1000.h"
#include "FadeOut.h"
#include "Trigger.h"
#include "QliphothBlock.h"
#include "MakaiButterfly.h"

#include <iostream>
#include <fstream>
using namespace std;

Library_S05::Library_S05()
{

}

void Library_S05::Free()
{
	for (auto& Element : m_vecQliphothBlock)
		Destroy(Element);
	m_vecQliphothBlock.clear();
	m_vecQliphothBlock.shrink_to_fit();

	Scene::Free();
}

Library_S05* Library_S05::Create()
{
	Library_S05* pInstance = new Library_S05;
	return pInstance;
}

HRESULT Library_S05::LoadScene()
{
	// Load Start

	SoundSystem::GetInstance()->ClearSound();
	SoundSystem::GetInstance()->Play("Library_01", 0.07f, true);

	m_fLoadingProgress = 0.01f;

#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera

	//if (auto SpCamera = AddGameObject<Camera>().lock();
	//	SpCamera)
	//{
	//	SpCamera->GetComponent<Transform>().lock()->SetPosition(Vector3{
	//		-4.327f,
	//		1.449f,
	//		36.596f, 
	//		});
	//}

	AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage5_Map.json");
	LoadObjects("../../Data/Stage5_AniObject.json", true);
	LoadCollObjects("../../Data/Stage5_Object.json");
	LoadBreakablebjects("../../Data/Stage5_BreakableObject.json");

	auto Map = AddGameObject<TempMap>().lock();
	Map->LoadMap(5);

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger

	RenderDataSetUp(false);
	TriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect

	// Stage5 길막
	m_vecQliphothBlock.reserve(4);

	// 0 ~ 3: 블러드. 3번이 정면에 있는 ㅈㄴ 큰 길막
	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.003f);
		ptr->SetRotation(Vector3(0.f, 0.f, 0.f));
		ptr->SetPosition(Vector3(-17.98f, -2.655f, 37.57f));
		ptr->PlayStart();
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr));
	}
	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.003f);
		ptr->SetRotation(Vector3(0.f, 180.f, 0.f));
		ptr->SetPosition(Vector3(-13.47f, -2.87f, 34.98f));
		ptr->SetActive(false);
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr));
	}
	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.004f);
		ptr->SetRotation(Vector3(0.f, 180.f, 0.f));
		ptr->SetPosition(Vector3(-18.53f, -2.8f, 34.88f));
		ptr->SetActive(false);
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr));
	}
	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.024f);
		ptr->SetRotation(Vector3(0.f, 270.f, 0.f));
		ptr->SetPosition(Vector3(-19.514f, -3.09f, 36.3f));
		ptr->SetActive(false);
		m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr));
	}
	
	//
	if (auto Sp = AddGameObject<MakaiButterfly>().lock(); Sp)
	{
		Sp->SetPosition({ -23.619f, -1.29f, 33.56f });
		Sp->PlayStart();
	}
	if (auto Sp = AddGameObject<MakaiButterfly>().lock(); Sp)
	{
		Sp->SetPosition({ -26.967f, -1.29f, 33.56f });
		Sp->PlayStart();
	}

	//
	if (_ShopFadeOut = AddGameObject<FadeOut>();
		!_ShopFadeOut.expired())
	{
		_ShopFadeOut.lock()->SetActive(false);
	}

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

HRESULT Library_S05::Awake()
{
	Scene::Awake();
	return S_OK;
}

HRESULT Library_S05::Start()
{
	Scene::Start();
	return S_OK;
}

HRESULT Library_S05::Update(const float _fDeltaTime)
{
	if (!_LateInit)
		LateInit();

	Scene::Update(_fDeltaTime);

	CheckShopAvailable();

	/* ---------- 치트 ---------- */
	if (Input::GetKeyDown(DIK_NUMPAD8))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S05));
	}
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S06));
	}
	/* -------------------------- */

	if (OBookCaseSunkenSmash)
	{
		*OBookCaseSunkenSmash += _fDeltaTime;
		if (*OBookCaseSunkenSmash >= BookCaseDelta)
		{
			*OBookCaseSunkenSmash = 0.0f;
			if (BookCaseTrigger.empty() == false)
			{
				BookCaseTrigger.back().lock()->TriggerEnable();
				BookCaseTrigger.pop_back();
			}
		}
	}

	return S_OK;
}

HRESULT Library_S05::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void Library_S05::LoadObjects(const std::filesystem::path& path, const bool _bAni)
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
};

void Library_S05::LoadCollObjects(const std::filesystem::path& path)
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

void Library_S05::LoadBreakablebjects(const std::filesystem::path& path)
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

void Library_S05::BgmPlay()
{
	// SoundSystem::GetInstance()->Play("Maple", 10.f, false, true);
}

void Library_S05::ApplyShopUpgradeDesc()
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

void Library_S05::CheckShopAvailable()
{
	if (_IsShopAvailable && Input::GetKeyDown(DIK_P))
	{
		if (auto SpShopPanel = _ShopPanel.lock(); SpShopPanel)
		{
			if (!SpShopPanel->IsActive())
			{
				if (auto SpFadeOut = _ShopFadeOut.lock(); SpFadeOut)
				{
					if (auto SpBtlPanel = _BtlPanel.lock(); SpBtlPanel)
					{
						SpBtlPanel->SetRedOrbActive(false);
						SpBtlPanel->SetActive(false);
					}

					SpFadeOut->SetActive(true);
					SpFadeOut->PlayStart(7u,
						[SpShopPanel, SpFadeOut]()
						{
							SpShopPanel->SetActive(true);
							SpFadeOut->SetActive(false);
						}
					);
				}
			}
			else
			{
				if (auto SpBtlPanel = _BtlPanel.lock(); SpBtlPanel)
				{
					SpBtlPanel->SetRedOrbActive(true);
					SpBtlPanel->SetActive(true);
				}

				ApplyShopUpgradeDesc();
				SpShopPanel->ResetCmd();
				SpShopPanel->SetActive(false);
			}
		}
	}
}

void Library_S05::RenderDataSetUp(const bool bTest)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();

	if (bTest)
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	}
	else
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Library_S05.json");
	}

	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission03;
	_Renderer->ao = 0.0005f;
	_Renderer->SkyIntencity = 0.005f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f,0.f };
	_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f };
	_Renderer->SoftParticleDepthScale = 0.7f;
	_Renderer->SkyRotationSpeed = 1.5f;
	_Renderer->StarScale = 4.f;
	_Renderer->StarFactor = 0.9f;
};

void Library_S05::TriggerSetUp()
{
	TriggerSewerSunken();
	TriggerBloodPrevious(TriggerBloodFirstWave(TriggerBloodSecondWave(TriggerBloodThirdWave())));


	// 버전 1: 성큰이 전부다 책상 부수며 나오는 이벤트.
	// TriggerBookCaseSunkenSmash();

	// 버전 2 : 성큰이 순차적으로 책장 부수며 나오는 이벤트 .  
	TriggerBookCaseSunkenFirstSmash();
	BookCaseTrigger.push_back(TriggerBookCaseSunkenThirdSmash());
	BookCaseTrigger.push_back(TriggerBookCaseSunkenSecondSmash());


	TriggerShop();
	TriggerNextScene();
};

void Library_S05::TriggerBloodPrevious(const std::weak_ptr<Trigger> _BloodTrigger)
{
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack = [this, _BloodTrigger]()
		{
			// 여기서 연출 하고 TriggerBlood 호출해주세요 !!
			_BloodTrigger.lock()->TriggerEnable();

			for (uint32 i = 1u; i < 4u; ++i)
			{
				m_vecQliphothBlock[i].lock()->SetActive(true);
				m_vecQliphothBlock[i].lock()->PlayStart();
			}
		};
		// 트리거 위치
		const Vector3 TriggerLocation{ -14.065550f, -3.066150f, 36.398815f };
		const Vector3 TriggerRotation{ 0.f, 0.f, 0.f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 0.5f,0.5f,3.f };
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

std::weak_ptr<Trigger> Library_S05::TriggerBloodFirstWave
									   (const std::weak_ptr<Trigger> _WaveTrigger)
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),

			AddGameObject<Em100>(),
			AddGameObject<Em100>(),

			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -19.f, -3.014f, 35.556f });
		MonsterWave[0].lock()->Set_Hp(60);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -17.929f, -3.014f, 35.528f });
		MonsterWave[1].lock()->Set_Hp(60);

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -18.979f, -3.014f, 37.077f });
		MonsterWave[2].lock()->Set_Hp(60);

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -17.980f, -3.014f, 36.903f });
		MonsterWave[3].lock()->Set_Hp(60);

		MonsterWave[4].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.626f, -3.168f, 35.443f });
		MonsterWave[4].lock()->Set_Hp(60);

		MonsterWave[5].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.134f, -3.339f, 36.172f });
		MonsterWave[5].lock()->Set_Hp(60);

		MonsterWave[6].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.291f, -3.222f, 36.921f });
		MonsterWave[6].lock()->Set_Hp(60);

		MonsterWave[7].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.930f, -3.274f, 35.774f });
		MonsterWave[7].lock()->Set_Hp(60);

		MonsterWave[8].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.604f, -3.256f, 35.654f });
		MonsterWave[8].lock()->Set_Hp(60);

		MonsterWave[9].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.308f, -3.266f, 36.834f });
		MonsterWave[9].lock()->Set_Hp(60);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -14.065550f, -3.066150f, 36.398815f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 10.f,10.f,10.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = false;
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
			_WaveTrigger]()
		{	
			// 다음 트리거 발동 !!
			_WaveTrigger.lock()->TriggerEnable();
		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);

		return SpTrigger;
	};

	return {};
}
std::weak_ptr<Trigger> Library_S05::TriggerBloodSecondWave(const std::weak_ptr<Trigger> _WaveTrigger)
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),

			AddGameObject<Em100>(),
			AddGameObject<Em100>(),

			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -19.f, -3.014f, 35.556f });
		MonsterWave[0].lock()->Set_Hp(60);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -17.929f, -3.014f, 35.528f });
		MonsterWave[1].lock()->Set_Hp(60);

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -18.979f, -3.014f, 37.077f });
		MonsterWave[2].lock()->Set_Hp(60);

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -17.980f, -3.014f, 36.903f });
		MonsterWave[3].lock()->Set_Hp(60);

		MonsterWave[4].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.626f, -3.168f, 35.443f });
		MonsterWave[4].lock()->Set_Hp(60);

		MonsterWave[5].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.134f, -3.339f, 36.172f });
		MonsterWave[5].lock()->Set_Hp(60);

		MonsterWave[6].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.291f, -3.222f, 36.921f });
		MonsterWave[6].lock()->Set_Hp(60);

		MonsterWave[7].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.930f, -3.274f, 35.774f });
		MonsterWave[7].lock()->Set_Hp(60);

		MonsterWave[8].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.604f, -3.256f, 35.654f });
		MonsterWave[8].lock()->Set_Hp(60);

		MonsterWave[9].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.308f, -3.266f, 36.834f });
		MonsterWave[9].lock()->Set_Hp(60);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -14.065550f, -3.066150f, 36.398815f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 10.f,10.f,10.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
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
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */
			, _WaveTrigger]()
		{
			// 다음 웨이브를 작동 !!
			_WaveTrigger.lock()->TriggerEnable();
		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);

		return SpTrigger;
	};

	return {};
};

std::weak_ptr<Trigger> Library_S05::TriggerBloodThirdWave()
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),
			AddGameObject<Em200>(),

			AddGameObject<Em100>(),
			AddGameObject<Em100>(),

			AddGameObject<Em0000>(),
			AddGameObject<Em0000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -19.f, -3.014f, 35.556f });
		MonsterWave[0].lock()->Set_Hp(60);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -17.929f, -3.014f, 35.528f });
		MonsterWave[1].lock()->Set_Hp(60);

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -18.979f, -3.014f, 37.077f });
		MonsterWave[2].lock()->Set_Hp(60);

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -17.980f, -3.014f, 36.903f });
		MonsterWave[3].lock()->Set_Hp(60);

		MonsterWave[4].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.626f, -3.168f, 35.443f });
		MonsterWave[4].lock()->Set_Hp(60);

		MonsterWave[5].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.134f, -3.339f, 36.172f });
		MonsterWave[5].lock()->Set_Hp(60);

		MonsterWave[6].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -16.291f, -3.222f, 36.921f });
		MonsterWave[6].lock()->Set_Hp(60);

		MonsterWave[7].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.930f, -3.274f, 35.774f });
		MonsterWave[8].lock()->Set_Hp(60);

		MonsterWave[8].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.604f, -3.256f, 35.654f });
		MonsterWave[9].lock()->Set_Hp(60);

		MonsterWave[9].lock()->GetComponent<Transform>().
			lock()->SetPosition({ -14.308f, -3.266f, 36.834f });
		MonsterWave[9].lock()->Set_Hp(60);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -14.065550f, -3.066150f, 36.398815f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 10.f,10.f,10.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
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
			for (uint32 i = 1u; i < 4u; ++i)
			{
				m_vecQliphothBlock[i].lock()->Reset();
			}

			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			};
		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);

		return SpTrigger;
	};

	return {};
};

void Library_S05::TriggerBookCaseSunkenSmash()
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -21167.0f, -1968.0f, 31454.0f } * GScale);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -20507.0f, -1968.0f, 31454.0f } * GScale);

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -19817.0f, -1968.0f, 31454.0f } * GScale);

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -19817.0f, -1968.0f, 30394.0f } * GScale);

		MonsterWave[4].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -20527.0f, -1968.0f, 30394.0f } * GScale);

		MonsterWave[5].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -21177.0f, -1968.0f, 30394.0f } * GScale);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -19.6f, -1.677550f, 30.850502f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 0.5f,0.5f,1.9f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 책장 박살나는 애니메이션 재생 하면 될듯 ... ?


			//... 여기서 로직 처리하세요 . 
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}

			for (uint32 i = 1u; i < 4u; ++i)
			{
				m_vecQliphothBlock[i].lock()->SetActive(false);
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			};
		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	};
};

void Library_S05::TriggerBookCaseSunkenFirstSmash()
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -19817.0f, -1968.0f, 31454.0f } * GScale);
		MonsterWave[0].lock()->Set_Hp(60);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -19817.0f, -1968.0f, 30394.0f } * GScale);
		MonsterWave[1].lock()->Set_Hp(60);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -19.6f, -1.677550f, 30.850502f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 0.5f,0.5f,1.9f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = true;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 책장 박살나는 애니메이션 재생 하면 될듯 ... ?
			OBookCaseSunkenSmash = 0.0f;

			//... 여기서 로직 처리하세요 . 
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
			}

			for (uint32 i = 1u; i < 4u; ++i)
			{
				m_vecQliphothBlock[i].lock()->SetActive(false);
			}
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{

		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	};
};

std::weak_ptr<Trigger> Library_S05::TriggerBookCaseSunkenSecondSmash()
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -20527.0f, -1968.0f, 30394.0f } * GScale);
		MonsterWave[0].lock()->Set_Hp(60);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -20527.0f, -1968.0f, 30394.0f } * GScale);
		MonsterWave[1].lock()->Set_Hp(60);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -19.6f, -1.677550f, 30.850502f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 10.f,10.f,10.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			
		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			
		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);

		return SpTrigger;
	};

	return {};
};

std::weak_ptr<Trigger> Library_S05::TriggerBookCaseSunkenThirdSmash()
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>()
		};

		// 몬스터 위치는 미리 잡아주기  . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -21167.0f, -1968.0f, 31454.0f } * GScale);
		MonsterWave[0].lock()->Set_Hp(60);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -21177.0f, -1968.0f, 30394.0f } * GScale);
		MonsterWave[1].lock()->Set_Hp(60);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -19.6f, -1.677550f, 30.850502f };
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 10.f,10.f,10.f };
		// 트리거 정보 등록 하자마자 트리거는 활성화 
		const bool ImmediatelyEnable = false;
		// 트리거 검사할 오브젝트는 플레이어 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		// 스폰 직후 이벤트 . 
		const std::function<void()> SpawnWaveAfterEvent =
			[this/*필요한 변수 캡쳐하세요 ( 되도록 포인터로 하세요 ) */]()
		{
			//... 여기서 책장 박살나는 애니메이션 재생 하면 될듯 ... ?
			//... 여기서 로직 처리하세요 . 



		};

		// 몬스터 전부 사망 하였을때 이벤트 . 
		const std::function<void()> WaveEndEvent =
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			};
		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);

		return SpTrigger;
	};

	return {};
};

void Library_S05::TriggerSewerSunken()
{
	if (auto SpTrigger = AddGameObject<Trigger>().lock();
		SpTrigger)
	{
		// 몬스터 웨이브 배열로 등록. 
		std::vector<std::weak_ptr<Monster>> MonsterWave
		{
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>(),
			AddGameObject<Em1000>()
		};

		// 몬스터 위치는 미리 잡아주기 . 
		MonsterWave[0].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -23979.177f, -1378.879f, 33560.666f } * GScale);
		MonsterWave[0].lock()->Set_Hp(60);

		MonsterWave[1].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -25179.414f, -1378.879f, 33560.666f } * GScale);
		MonsterWave[1].lock()->Set_Hp(60);

		MonsterWave[2].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -26379.939f, -1378.879f, 33560.666f } * GScale);
		MonsterWave[2].lock()->Set_Hp(60);

		MonsterWave[3].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -27578.267f, -1378.879f, 33560.666f } * GScale);
		MonsterWave[3].lock()->Set_Hp(60);

		MonsterWave[4].lock()->GetComponent<Transform>().
			lock()->SetPosition(Vector3{ -28779.519f, -1378.879f, 33560.666f } * GScale);
		MonsterWave[4].lock()->Set_Hp(60);

		// 트리거 위치 .. . 
		const Vector3 TriggerLocation{ -23.681999f, -1.231250f, 33.628098f};
		// 트리거 박스 사이즈 
		const Vector3 TriggerBoxSize = { 0.5f,0.5f,0.5f };
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
			[this/*필요한 변수 캡쳐하세요 (되도록 포인터로 하세요) */]()
		{
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetRedOrbActive(false);
				Sp->SetGlobalActive(false);
				Sp->ResetRankScore();
			};
		};

		SpTrigger->EventRegist(
			MonsterWave,
			TriggerLocation,
			TriggerBoxSize,
			ImmediatelyEnable,
			TargetTag,
			SpawnWaveAfterEvent,
			WaveEndEvent);
	};
}

void Library_S05::TriggerShop()
{
	auto _Trigger = AddGameObject<Trigger>().lock();
	if (_Trigger)
	{
		const std::function<void()> _CallBack =
			[this]()
		{
			_IsShopAvailable = true;
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -29.511f, -0.613f, 30.34f };
		const Vector3 TriggerRotation{ 0.f, 0.f, 0.f };

		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 1.f, 1.f, 1.f };
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
}

void Library_S05::TriggerNextScene()
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
				_FadeOut->PlayStart(4u,
					[SpPanel]()
					{
						SpPanel->SetNullBlackActive(true);
						SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S06));
					});
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -33.122341f, -0.641000f, 30.992397f };
		const Vector3 TriggerRotation{ 0.f, 0.f, 0.f };

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
			TargetTag,
			TriggerRotation);
	}
};

void Library_S05::LateInit()
{
	//SoundSystem::GetInstance()->ClearSound();

	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({ -11.1f, -3.483f, 32.696f });
	}

	ApplyShopUpgradeDesc();

	Renderer::GetInstance()->LateSceneInit();

	BgmPlay();

	_LateInit = true;
}