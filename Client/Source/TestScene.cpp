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
#include "Em1000.h"
#include "Car.h"
#include "OvertureHand.h"
#include "Glint.h"
#include "Liquid.h"
#include "AppearGroundMonster.h"
#include "Smoke.h"
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

	AddGameObject<MainCamera>();
	AddGameObject<Nero>();
	AddGameObject<BtlPanel>();
	/*AddGameObject<Em100>();
	AddGameObject<Em0000>();*/
	//AddGameObject<Car>();

	
	for (UINT i = 0; i < 3; ++i)
	{
		weak_ptr<Em100> pEm100 = AddGameObject<Em100>();
		pEm100.lock()->SetActive(false);
		m_vecEm100.push_back(static_pointer_cast<GameObject>(pEm100.lock()));
	}

	LoadMap();
	AddGameObject<TempMap>();

	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.015f);
		ptr->SetRotation(Vector3(0.f, 262.286f, 0.f));
		ptr->SetPosition(Vector3(-5.429f, 0.286f, -5.05f));
		ptr->PlayStart();
	}
	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.009f);
		ptr->SetRotation(Vector3(0.f, 210.909f, 0.f));
		ptr->SetPosition(Vector3(-0.303f, 0.505f, -2.475f));
		//ptr->SetActive(false);
		//ptr->PlayStart();
	}
	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.025f);
		ptr->SetRotation(Vector3(0.f, 339.429f, 0.f));
		ptr->SetPosition(Vector3(-0.857f, 1.143f, 0.f));
		//ptr->SetActive(false);
		//ptr->PlayStart();
	}
	if (auto ptr = AddGameObject<QliphothBlock>().lock();
		ptr)
	{
		ptr->SetScale(0.016f);
		ptr->SetRotation(Vector3(0.f, 25.714f, 0.f));
		ptr->SetPosition(Vector3(1.429f, 1.429f, 0.f));
		//ptr->SetActive(false);
		//ptr->PlayStart();
	}

	/*AddGameObject<Glint>();
	AddGameObject<OvertureHand>();
	AddGameObject<Liquid>();
	AddGameObject<QliphothBlock>();
	AddGameObject<AppearGroundMonster>();*/

	// 수정필요
	//AddGameObject<DashImpact>();
	
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	_Renderer->LightLoad("..\\..\\Resource\\LightData\\Mission02.json");
	// _Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
	_Renderer->ao = 0.0005; 
	_Renderer->SkyIntencity = 0.005f;
	_Renderer->SkysphereScale = 0.078f;
	_Renderer->SkysphereRot = { 0.f,0.f,0.f }; 
	_Renderer->SkysphereLoc = { 0.f,-2.3f,0.f  }; 
	_Renderer->SoftParticleDepthScale = 0.7f;
	_Renderer->SkyRotationSpeed = 1.5f;

	// Stage2 안개
	if (auto pSmoke = AddGameObject<Smoke>().lock();
		pSmoke)
	{
		pSmoke->SetScale(0.3f);
		pSmoke->SetRotation(Vector3(0.f, 274.03f, 0.f));
		pSmoke->SetPosition(Vector3(-10.f, -4.f, 30.f));
		pSmoke->PlayStart(10.f);
	}
	if (auto pSmoke = AddGameObject<Smoke>().lock();
		pSmoke)
	{
		pSmoke->SetScale(0.3f);
		pSmoke->SetRotation(Vector3(0.f, 0.f, 0.f));
		pSmoke->SetPosition(Vector3(30.f, -4.f, 10.f));
		pSmoke->PlayStart(10.f);
	}
	if (auto pSmoke = AddGameObject<Smoke>().lock();
		pSmoke)
	{
		pSmoke->SetScale(0.3f);
		pSmoke->SetRotation(Vector3(0.f, 91.343f, 0.f));
		pSmoke->SetPosition(Vector3(10.f, -4.f, -25.f));
		pSmoke->PlayStart(10.f);
	}
	if (auto pSmoke = AddGameObject<Smoke>().lock();
		pSmoke)
	{
		pSmoke->SetScale(0.3f);
		pSmoke->SetRotation(Vector3(0.f, 150.448f, 0.f));
		pSmoke->SetPosition(Vector3(-20.f, -4.f, -30.f));
		pSmoke->PlayStart(10.f);
	}

	return S_OK;
}

HRESULT TestScene::Awake()
{
	Scene::Awake();

	//if (nullptr != pPlane)
	//	return S_OK;

	/*pPlane = PxCreatePlane(*Physics::GetPxPhysics(), PxPlane(0.f, 1.f, 0.f, 0.f) , *Physics::GetDefaultMaterial());

	Physics::AddActor(UniqueID, *pPlane);*/

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

	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		for (UINT i = 0; i < m_vecEm100.size(); ++i)
		{
			if (m_vecEm100[i].expired())
				continue;
			if (m_vecEm100[i].lock()->IsActive())
				continue;
			
			m_vecEm100[i].lock()->SetActive(true);
			break;
		}
	}

	if (Input::GetKeyDown(DIK_NUMPAD1))
	{
		list<weak_ptr<QliphothBlock>> listQliphoth = FindGameObjectsWithType<QliphothBlock>();
		for (auto& obj : listQliphoth)
		{
			//obj.lock()->SetActive(true);
			// + 몬스터도 이때 중앙에 소환
			obj.lock()->PlayStart();
		}

		//std::static_pointer_cast<QliphothBlock>(FindGameObjectsWithTag(Eff_QliphothBlock).lock())->PlayStart();
	}
	if (Input::GetKeyDown(DIK_NUMPAD2))
	{
		list<weak_ptr<QliphothBlock>> listQliphoth = FindGameObjectsWithType<QliphothBlock>();
		for (auto& obj : listQliphoth)
		{
			//obj.lock()->SetActive(false);
			// + 몬스터도 이때 중앙에 소환
			obj.lock()->Reset();
		}
		//std::static_pointer_cast<QliphothBlock>(FindGameObjectsWithTag(Eff_QliphothBlock).lock())->PlayStart();
	}

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
