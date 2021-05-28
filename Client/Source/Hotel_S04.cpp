#include "stdafx.h"
#include "..\Header\Hotel_S04.h"
#include "LoadingScene.h"
#include "PreLoader.h"
#include "TempMap.h"
#include "Nero.h"
#include "BtlPanel.h"
#include "Camera.h"
#include "MainCamera.h"
#include "Renderer.h"
#include "MapObject.h"
#include "Monster.h"
#include "SoundSystem.h"
#include "Em5000.h"
#include "FadeOut.h"
#include "Trigger.h"
#include "ShopPanel.h"
#include "TimeSystem.h"

#include <iostream>
#include <fstream>
#include "NoCameraColl.h"
using namespace std;

Hotel_S04::Hotel_S04()
{

}

void Hotel_S04::Free()
{
	Scene::Free();
}

Hotel_S04* Hotel_S04::Create()
{
	Hotel_S04* pInstance = new Hotel_S04;
	return pInstance;
};

HRESULT Hotel_S04::LoadScene()
{
	// Load Start
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

	AddGameObject<NoCameraColl>();

	_Camera = AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

	m_pBoss = AddGameObject<Em5000>();
	m_pBoss.lock()->GetComponent<Transform>().lock()->SetPosition({ -5.629f, -1.529f, 47.67f });

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage4_Map.json");

	auto Map = AddGameObject<TempMap>().lock();
	Map->LoadMap(4);

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger

	RenderDataSetUp(false);
	TriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect

	if (_FadeOut = AddGameObject<FadeOut>();
		!_FadeOut.expired())
	{
		_FadeOut.lock()->SetActive(false);
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

HRESULT Hotel_S04::Awake()
{
	Scene::Awake();
	return S_OK;
}

HRESULT Hotel_S04::Start()
{
	Scene::Start();
	return S_OK;
}

HRESULT Hotel_S04::Update(const float _fDeltaTime)
{
	if (!_LateInit)
		LateInit();

	Scene::Update(_fDeltaTime);

	/* ---------- 치트 ---------- */
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
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S04));
	}
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		if (auto SpFadeOut = _FadeOut.lock(); SpFadeOut)
		{
			if (auto SpPanel = _BtlPanel.lock(); SpPanel)
			{
				SpFadeOut->SetActive(true);

				SpFadeOut->PlayStart(3u,
					[SpPanel]()
					{
						SpPanel->SetNullBlackActive(true);
						SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S05));
					}
				);
			}
		}
		else
		{
			SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S05));
		}
	}
	/* -------------------------- */

	if (!_IsBossDead && m_pBoss.lock()->Get_BattleInfo().iHp <= 0)
	{
		if (auto SpPanel = _BtlPanel.lock(); SpPanel)
		{
			SpPanel->SetBossGaugeActive(false);
			SpPanel->SetRedOrbActive(false);
			SpPanel->SetGlobalActive(false);
			SpPanel->ResetRankScore();
		}

		_IsBossDead = true;
	}

	return S_OK;
}

HRESULT Hotel_S04::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void Hotel_S04::LoadObjects(const std::filesystem::path& path)
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
};

void Hotel_S04::BgmPlay()
{
	// SoundSystem::GetInstance()->Play("Maple", 10.f, false, true);
}

void Hotel_S04::ApplyShopUpgradeDesc()
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

void Hotel_S04::RenderDataSetUp(const bool bTest)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();

	if (bTest)
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	}
	else
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Hotel_S04.json");
	}

	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
	_Renderer->ao = 0.03f;
	_Renderer->SkyIntencity = 0.025f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f,0.f };
	_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f };
	_Renderer->SoftParticleDepthScale = 0.7f;
	_Renderer->SkyRotationSpeed = 1.5f;
	_Renderer->StarScale = 4.f;
	_Renderer->StarFactor = 0.9f;
}

void Hotel_S04::TriggerSetUp()
{
	TriggerMeetingWithGoliath(TriggerCutScene());
};

std::weak_ptr<Trigger> Hotel_S04::TriggerCutScene()
{
	auto _Trigger = AddGameObject<Trigger>().lock();
	if (_Trigger)
	{
		const std::function<void()> _CallBack =
			[this]()
		{
			// 골리앗연출씬
			Renderer::GetInstance()->SkyOriginColor = Vector4{ 246.f / 255.f,10.f / 255.f,10.f / 255.f,1.f };
			SoundSystem::GetInstance()->ClearSound();
			SoundSystem::GetInstance()->Play("Stage1_Boss", 0.12f, true, {}, 0);
			m_pBoss.lock()->Set_Howling();

			Vector3 CamAt = m_pBoss.lock()->GetMonsterBoneWorldPos("Neck");
			Vector3 CamEye = m_pBoss.lock()->GetComponent<Transform>().lock()->GetPosition() +
				m_pBoss.lock()->GetComponent<Transform>().lock()->GetLook() * -1.4f;
			CamEye.y += 0.3f;
			_Camera.lock()->Set_TriggerCam(MainCamera::STAGE4_BOSS_CUTSCENE, CamAt, 4.5f);
			_Camera.lock()->SetEye(CamEye);
			_Camera.lock()->SetShakeInfo(3.5f, 10.f);

			vector<Vector3> _LostTimes;
			_LostTimes.emplace_back(Vector3{ 7.f,1.f,0.3f });
			TimeSystem::GetInstance()->LostTime(_LostTimes);
			//_Camera.lock()->Set_TriggerCam(MainCamera::STAGE4_BOSS_CUTSCENE,)

			// 다른 후처리가 묻히니 스카이 왜곡을 약하게 .... 

			// 로직 작성 .... 

			//
			if (auto Sp = _BtlPanel.lock(); Sp)
			{
				Sp->SetGlobalActive(true, true);
				Sp->SetBossGaugeActive(true);
				Sp->SetAppearGoliath(true);
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -4.379f, -6.142f, 40.870f };
		const Vector3 TriggerRotation{ 0.f, 0.f, 0.f };

		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 10.f,10.f,5.f };
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
}

void Hotel_S04::TriggerMeetingWithGoliath(const std::weak_ptr<Trigger>& _CamTrigger)
{
	// 여기서 왜곡 계수 조절해야함 !! 
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[_CamTrigger,this]()
		{
			Renderer::GetInstance()->FadeOutStart(1.f);
			_Camera.lock()->SetFadeSceneInfo(0.8f);
			_Camera.lock()->Set_Trigger(_CamTrigger);
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -4.379f, -6.142f, 40.870f };
		const Vector3 TriggerRotation{ 0.f, 0.f, 0.f };

		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 10.f,10.f,5.f };
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

void Hotel_S04::LateInit()
{
	SoundSystem::GetInstance()->ClearSound();
	SoundSystem::GetInstance()->Play("Wind1", 0.5f, false, {}, 35000);
	
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({ -4.205f, 0.79473f, 36.998f });
		SpPlayer->SetAngle(180.f);
	}

	ApplyShopUpgradeDesc();

	if (auto SpMainCamera = _Camera.lock();
		SpMainCamera)
	{
		SpMainCamera->SetAngle({ -5.f,170.f,0.f });
		SpMainCamera->SetStartPos();
	}
	
	Renderer::GetInstance()->LateSceneInit();
	
	BgmPlay();

	_LateInit = true;
}