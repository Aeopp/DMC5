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
#include <iostream>
#include <fstream>
using namespace std;

Hotel_S02::Hotel_S02()
{

}

void Hotel_S02::Free()
{
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
	m_fLoadingProgress = 0.01f;

#pragma region PreLoad

	PreLoader::PreLoadResources();

#pragma endregion

	m_fLoadingProgress = 0.1f;

#pragma region Player & Camera
	AddGameObject<SecretVision>();

	AddGameObject<Camera>();
	/*AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();*/

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage2_Map.json");
	LoadObjects("../../Data/Stage2_Object.json");

	AddGameObject<TempMap>();

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger

	RenderDataSetUp();
	TriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect


#pragma endregion

	m_fLoadingProgress = 0.8f;

#pragma region UI

	AddGameObject<BtlPanel>();

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

	// 테스트용 ////////////////////////
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S03));
	}
	////////////////////////////////////

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

void Hotel_S02::RenderDataSetUp()
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	//_Renderer->LightLoad("..\\..\\Resource\\LightData\\Mission02.json");
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

void Hotel_S02::TriggerSetUp()
{

}

void Hotel_S02::LateInit()
{
	// + 플레이어 초기 위치 잡기 등
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({ -3.63097f, 1.f, 11.70365f });
	}


	_LateInit = true;
}