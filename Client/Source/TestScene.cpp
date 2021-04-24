#include "stdafx.h"
#include "..\Header\TestScene.h"
#include "Scene.h"
#include "TestObject.h"
#include "ShaderTester.h"
#include "TempMap.h"
#include "TestAnimationObject.h"
#include "Camera.h"
#include "Nero.h"
#include "RedQueen.h"
#include "Em100.h"
#include "Em101.h"
#include "Em0000.h"
#include "Em0000_Weapon.h"
#include "Em5000.h"
#include "Car.h"
#include "OvertureHand.h"
#include "Glint.h"
#include "Liquid.h"
#include "AppearGroundMonster.h"
#include "Dust.h"
#include "QliphothBlock.h"
#include "BtlPanel.h"
#include "MainCamera.h"
#include "Renderer.h"
#include "MapObject.h"


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
}

HRESULT TestScene::LoadScene()
{
	//AddGameObject<Camera>();

	AddGameObject<Nero>();
	AddGameObject<BtlPanel>();
	AddGameObject<Em100>();

	//AddGameObject<Car>();

	
	//LoadMap();
	//AddGameObject<TempMap>();

	
	

	//AddGameObject<Glint>();
	//AddGameObject<OvertureHand>();
	//AddGameObject<Liquid>();
	//AddGameObject<AppearGroundMonster>();
	//AddGameObject<Dust>();
	//AddGameObject<QliphothBlock>();

	// 수정필요
	//AddGameObject<DashImpact>();



	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
	_Renderer->ao = 0.01f; 
	_Renderer->SkyIntencity = 0.111f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f ,0.f }; 
	_Renderer->SkysphereLoc = { 0.f,-4.762f,0.f  }; 

	return S_OK;
}

HRESULT TestScene::Awake()
{
	Scene::Awake();

	if (nullptr != pPlane)
		return S_OK;

	pPlane = PxCreatePlane(*Physics::GetPxPhysics(), PxPlane(0.f, 1.f, 0.f, 0.f) , *Physics::GetDefaultMaterial());

	Physics::AddActor(UniqueID, *pPlane);

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
	//cout << "SceneUpdate" << endl;

	return S_OK;
}

HRESULT TestScene::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);
	return S_OK;
}

void TestScene::LoadMap()
{
	std::ifstream inputStream{ "../../Data/Stage2.json" };

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
