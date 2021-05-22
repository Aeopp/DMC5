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

#include <iostream>
#include <fstream>
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
	//	
	//}

	_Camera = AddGameObject<MainCamera>();
	_Camera.lock()->GetComponent<Transform>().lock()->SetPosition({ -5.218f, -1.5f, 43.326f });
	
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster
	auto _pMonster = AddGameObject<Em5000>();
	_pMonster.lock()->GetComponent<Transform>().lock()->SetPosition({ -5.629f, -1.529f, 47.67f });
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
	if (Input::GetKeyDown(DIK_NUMPAD8))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S04));
	}
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::LIBRARY_S05));
	}
	if (Input::GetKeyDown(DIK_NUMPAD1))
	{
		_Camera.lock()->Set_At_Transform(
			FindGameObjectWithTag(GAMEOBJECTTAG::Monster5000).lock()->GetComponent<Transform>(),
			MainCamera::AT_BOSS1);
		_Camera.lock()->SetDistance(1.1f);
	}
	/* -------------------------- */

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
};

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
	_Renderer->ao = 0.5f;
	_Renderer->SkyIntencity = 0.005f;
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
	TriggerMeetingWithGoliath();
}

void Hotel_S04::TriggerMeetingWithGoliath()
{
	// 여기서 왜곡 계수 조절해야함 !! 

	{
		constexpr float NoiseWrap = 2.020390f;
		constexpr float TimeCorr = 0.009006f;
		Renderer::GetInstance()->SkyDistortionStart(
			NoiseWrap,
			TimeCorr);
	}
}

void Hotel_S04::LateInit()
{
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({-5.218f, -1.5f, 43.326f });
	}
	/*_Camera.lock()->Set_At_Transform(
		FindGameObjectWithTag(GAMEOBJECTTAG::Monster5000).lock()->GetComponent<Transform>() ,
		MainCamera::AT_BOSS1);*/
	

	Renderer::GetInstance()->LateSceneInit();
	
	_LateInit = true;
	BgmPlay();
}