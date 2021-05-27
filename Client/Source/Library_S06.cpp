#include "stdafx.h"
#include "..\Header\Library_S06.h"
#include "LoadingScene.h"
#include "PreLoader.h"
#include "Trigger.h"
#include "TempMap.h"
#include "Nero.h"
#include "BtlPanel.h"
#include "Camera.h"
#include "MainCamera.h"
#include "Renderer.h"
#include "MapObject.h"
#include "Monster.h"
#include "SoundSystem.h"
#include "Em5300.h"
#include "FinalReady.h"
#include "NuClear.h"
#include "Energism.h"
#include "ShopPanel.h"
#include "Renderer.h"
#include "EnergismReady.h"

#include <iostream>
#include <fstream>
using namespace std;

Library_S06::Library_S06()
{

}

void Library_S06::Free()
{
	Scene::Free();
}

Library_S06* Library_S06::Create()
{
	Library_S06* pInstance = new Library_S06;
	return pInstance;
}

HRESULT Library_S06::LoadScene()
{
	// Load Start

	m_fLoadingProgress = 0.01f;

#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera

	/*if (auto SpCamera = AddGameObject<Camera>().lock();
		SpCamera)
	{
		SpCamera->GetComponent<Transform>().lock()->SetPosition(Vector3{
			-38.744f, -0.388f, 30.861f
			});
	}*/

	 /*AddGameObject<FinalReady>();
	 AddGameObject<Energism>();
	 AddGameObject<NuClear>();
	 AddGameObject<EnergismReady>();*/
	 
	AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster
	m_pBoss = AddGameObject<Em5300>();
	m_pBoss.lock()->GetComponent<Transform>().lock()->SetPosition({ -38.744f, -0.388f, 30.861f });
#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage6_Map.json");

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

HRESULT Library_S06::Awake()
{
	Scene::Awake();
	return S_OK;
}

HRESULT Library_S06::Start()
{
	Scene::Start();
	return S_OK;
}

HRESULT Library_S06::Update(const float _fDeltaTime)
{
	if (!_LateInit)
		LateInit();

	Scene::Update(_fDeltaTime);

	/* ---------- 치트 ---------- */
	if (Input::GetKeyDown(DIK_NUMPAD8))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S06));
	}
	//if (Input::GetKeyDown(DIK_NUMPAD9))
	//{
	//	SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S06));
	//}
	/* -------------------------- */

	return S_OK;
}

HRESULT Library_S06::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void Library_S06::LoadObjects(const std::filesystem::path& path)
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

void Library_S06::BgmPlay()
{
	// SoundSystem::GetInstance()->Play("Maple", 10.f, false, true);
};

void Library_S06::RenderDataSetUp(const bool bTest)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();

	if (bTest)
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	}
	else
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Library_S06_PtDir.json");
	}

	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission03;
	_Renderer->ao = 0.00025f;
	_Renderer->SkyIntencity = 0.0005f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f,0.f };
	_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f };
	_Renderer->SoftParticleDepthScale = 0.7f;
	_Renderer->SkyRotationSpeed = 1.5f;
	_Renderer->StarScale = 4.f;
	_Renderer->StarFactor = 0.9f;
	Renderer::GetInstance()->SkyOriginColor = Vector4{ 1.f,1.f,1.f,1.f };

	_Renderer->SkyDistortionStart(20.f, 0.110972f);
	_Renderer->SkyDistortionIntencity = 100.f;
	_Renderer->DistortionColor = Vector4{0.f,187.f/255.f,1.f,1.f};
}

void Library_S06::ApplyShopUpgradeDesc()
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

		// UpgradeDesc._RebellionUpgradeCount 1이면 리벨리온 산거임 ㅇㅇ
	}

	BtlPanel::SetExGaugeLevel(UpgradeDesc._ExgaugeUpUpgradeCount);
	BtlPanel::SetTDTGaugeLevel(UpgradeDesc._PurpleOrbUpgradeCount);
}

void Library_S06::TriggerSetUp()
{
	if (auto SpBoss = m_pBoss.lock();
		SpBoss)
	{
		SpBoss->SetTrigger(TriggerUlte());
	}
}

std::weak_ptr<Trigger> Library_S06::TriggerUlte()
{
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[this]()
		{
			this->m_pBoss.lock()->Set_Ulte();
		};
		
		const Vector3 TriggerLocation{ -37.148f, -0.5f, 30.902f };
		const Vector3 BoxSize{ 0.5f, 0.5f, 0.5f };
		const bool ImmediatelyEnable = false;
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Monster5300;
		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
		return _Trigger;
	};

	return {};
}

void Library_S06::LateInit()
{
	SoundSystem::GetInstance()->ClearSound();

	
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({ -33.711f, -0.994f, 30.884f });
	}

	ApplyShopUpgradeDesc();

	Renderer::GetInstance()->LateSceneInit();

	Renderer::GetInstance()->SkyDistortionStart(20.f, 0.110972f);
	Renderer::GetInstance()->SkyDistortionIntencity = 100.f;
	Renderer::GetInstance()->DistortionColor =
		Vector4{ 0.f,187.f / 255.f,1.f,1.f };


	BgmPlay();

	_LateInit = true;
};
