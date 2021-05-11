#include "stdafx.h"
#include "..\Header\Hotel_S01.h"
#include "TempMap.h"
#include "Nero.h"
#include "BtlPanel.h"
#include "MainCamera.h"
#include "Renderer.h"
#include "MapObject.h"

#include <iostream>
#include <fstream>
using namespace std;


Hotel_S01::Hotel_S01()
{

}

void Hotel_S01::Free()
{
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

#pragma region Player & Camera

	AddGameObject<MainCamera>();

	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & RenderData

	//LoadMap();
	AddGameObject<TempMap>();
	
	RenderDataSetUp();

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region UI & Effect

	AddGameObject<BtlPanel>();

#pragma endregion

	m_fLoadingProgress = 0.8f;

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
	Scene::Update(_fDeltaTime);
	return S_OK;
}

HRESULT Hotel_S01::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void Hotel_S01::LoadMap()
{
	std::ifstream inputStream{ "../../Data/Hotel.json" };
	//std::ifstream inputStream{ "../../Data/Mission02.json" };

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
}

void Hotel_S01::RenderDataSetUp()
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