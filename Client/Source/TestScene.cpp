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
#include "WingSword1st.h"
#include "WingSword2nd.h"
#include "WingSword3rd.h"
#include "WingSword4th.h"
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
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"
#include "CbsTrail.h"
#include "ElectricOccur.h"

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
	m_fLoadingProgress = 0.01f;	// 로딩 시작

	//AddGameObject<Camera>();
	AddGameObject<MainCamera>();

	m_fLoadingProgress = 0.2f;

	_Player = AddGameObject<Nero>();

	m_fLoadingProgress = 0.4f;

	AddGameObject<BtlPanel>();

	//AddGameObject<Em0000>();
	//AddGameObject<Em1000>();
	//AddGameObject<Em5300>();
	//AddGameObject<Em5000>();
	
	//AddGameObject<CircleWave>();
	//AddGameObject<AirHike>();
	//AddGameObject<FireCircle>();
	//AddGameObject<IceAge>();
	//AddGameObject<CbsTrail>();
	//AddGameObject<ElectricOccur>();


	// Wave 1st
	//{
	//	weak_ptr<Em100> _Em100 = AddGameObject<Em100>();
	//	_Em100.lock()->SetActive(false);
	//	_Em100.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ -0.8f, 0.02f, -0.7f });
	//	Wavefirst.push_back(static_pointer_cast<GameObject>(_Em100.lock()));

	//	_Em100 = AddGameObject<Em100>();
	//	_Em100.lock()->SetActive(false);
	//	_Em100.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ 0.7f, 0.02f, -1.f });
	//	Wavefirst.push_back(static_pointer_cast<GameObject>(_Em100.lock()));

	//	weak_ptr<Em0000> pEm0000 = AddGameObject<Em0000>();
	//	pEm0000.lock()->SetActive(false);
	//	pEm0000.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ 1.f, 0.02f, -1.f });
	//	Wavefirst.push_back(static_pointer_cast<GameObject>(pEm0000.lock()));

	//	pEm0000 = AddGameObject<Em0000>();
	//	pEm0000.lock()->SetActive(false);
	//	pEm0000.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ 0.5f, 0.02f,1.f });
	//	Wavefirst.push_back(static_pointer_cast<GameObject>(pEm0000.lock()));
	//}

	//// Wave 2nd
	//{
	//	weak_ptr<Em100> _Em100 = AddGameObject<Em100>();
	//	_Em100.lock()->SetActive(false);
	//	_Em100.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ -2.85553,0.02f,2.24367f });
	//	Wavesecond.push_back(static_pointer_cast<GameObject>(_Em100.lock()));

	//	_Em100 = AddGameObject<Em100>();
	//	_Em100.lock()->SetActive(false);
	//	_Em100.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ -3.74279f ,0.02f,5.37266f });
	//	Wavesecond.push_back(static_pointer_cast<GameObject>(_Em100.lock()));

	//	weak_ptr<Em0000> pEm0000 = AddGameObject<Em0000>();
	//	pEm0000.lock()->SetActive(false);
	//	pEm0000.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ -1.64173f,0.02f,2.73873f });
	//	Wavesecond.push_back(static_pointer_cast<GameObject>(pEm0000.lock()));

	//	pEm0000 = AddGameObject<Em0000>();
	//	pEm0000.lock()->SetActive(false);
	//	pEm0000.lock()->GetComponent<Transform>().lock()->SetPosition(Vector3{ -2.25858f,0.02f,5.93767f });
	//	Wavesecond.push_back(static_pointer_cast<GameObject>(pEm0000.lock()));
	//}

	LoadMap();
	AddGameObject<TempMap>();

	m_fLoadingProgress = 0.6f;

	RenderDataSetUp();

	m_fLoadingProgress = 0.8f;

	//// Stage2 안개
	//if (auto pSmoke = AddGameObject<Smoke>().lock();
	//	pSmoke)
	//{
	//	pSmoke->SetScale(0.3f);
	//	pSmoke->SetRotation(Vector3(0.f, 274.03f, 0.f));
	//	pSmoke->SetPosition(Vector3(-10.f, -4.f, 30.f));
	//	pSmoke->PlayStart(10.f);
	//}
	//if (auto pSmoke = AddGameObject<Smoke>().lock();
	//	pSmoke)
	//{
	//	pSmoke->SetScale(0.3f);
	//	pSmoke->SetRotation(Vector3(0.f, 0.f, 0.f));
	//	pSmoke->SetPosition(Vector3(30.f, -4.f, 10.f));
	//	pSmoke->PlayStart(10.f);
	//}
	//if (auto pSmoke = AddGameObject<Smoke>().lock();
	//	pSmoke)
	//{
	//	pSmoke->SetScale(0.3f);
	//	pSmoke->SetRotation(Vector3(0.f, 91.343f, 0.f));
	//	pSmoke->SetPosition(Vector3(10.f, -4.f, -25.f));
	//	pSmoke->PlayStart(10.f);
	//}
	//if (auto pSmoke = AddGameObject<Smoke>().lock();
	//	pSmoke)
	//{
	//	pSmoke->SetScale(0.3f);
	//	pSmoke->SetRotation(Vector3(0.f, 150.448f, 0.f));
	//	pSmoke->SetPosition(Vector3(-20.f, -4.f, -30.f));
	//	pSmoke->PlayStart(10.f);
	//}

	//// Stage2 길막
	//m_vecQliphothBlock.reserve(4);
	//if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
	//	!ptr.expired())
	//{
	//	ptr.lock()->SetScale(0.015f);
	//	ptr.lock()->SetRotation(Vector3(0.f, 262.286f, 0.f));
	//	ptr.lock()->SetPosition(Vector3(-5.429f, 0.286f, -5.05f));
	//	ptr.lock()->PlayStart();
	//	m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	//}
	//if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
	//	!ptr.expired())
	//{
	//	ptr.lock()->SetScale(0.009f);
	//	ptr.lock()->SetRotation(Vector3(0.f, 210.909f, 0.f));
	//	ptr.lock()->SetPosition(Vector3(-0.303f, 0.505f, -2.475f));
	//	ptr.lock()->SetActive(false);
	//	m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	//}
	//if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
	//	!ptr.expired())
	//{
	//	ptr.lock()->SetScale(0.025f);
	//	ptr.lock()->SetRotation(Vector3(0.f, 339.429f, 0.f));
	//	ptr.lock()->SetPosition(Vector3(-0.857f, 1.143f, 0.f));
	//	ptr.lock()->SetActive(false);
	//	m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	//}
	//if (weak_ptr<Effect> ptr = AddGameObject<QliphothBlock>().lock();
	//	!ptr.expired())
	//{
	//	ptr.lock()->SetScale(0.016f);
	//	ptr.lock()->SetRotation(Vector3(0.f, 25.714f, 0.f));
	//	ptr.lock()->SetPosition(Vector3(1.429f, 1.429f, 0.f));
	//	ptr.lock()->SetActive(false);
	//	m_vecQliphothBlock.push_back(static_pointer_cast<Effect>(ptr.lock()));
	//}

	// DOOMSDAY
	if (auto pFont = AddGameObject<Font>().lock();
		pFont)
	{
		pFont->SetText("D 17, Until Dooms Day",
			Font::TEX_ID::DMC5_BLACK_GRAD,
			Vector2(245.f, 130.f),
			Vector2(0.6f, 0.6f),
			Vector3(1.f, 1.f, 1.f),
			true);

		pFont->SetRenderFlag(true);
	}

	m_fLoadingProgress = 1.f;	// 로딩 완료

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
	/*auto _RefParticles = ParticleSystem::GetInstance()->PlayableParticles("Ice", 3.f);
	for (auto& _PlayInstance : _RefParticles)
	{
		_PlayInstance->LifeTime = 3.f;
		_PlayInstance->Scale = { GScale,GScale,GScale };
		_PlayInstance->CurveControlPoints = {};
		_PlayInstance->CurveControlRotationPoints = {};
	}*/



	//cout << "SceneUpdate" << endl;


	// 여기서 임시로 트리거 처리 ???
	//if (
	//	(FMath::Length
	//	(WavefirstTriggerPos - _Player.lock()->GetComponent<Transform>().lock()->GetPosition())
	//		< 0.1f) &&
	//	!bfirst)
	//{
	//	for (auto& spawn_entity : Wavefirst)
	//	{
	//		bfirst = true;
	//		spawn_entity.lock()->SetActive(true);
	//	}

	//	for (int i = 1; i < 4; ++i)
	//	{
	//		if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
	//		{
	//			m_vecQliphothBlock[i].lock()->SetActive(true);
	//			m_vecQliphothBlock[i].lock()->PlayStart();
	//		}
	//	}
	//}


	//if ((FMath::Length
	//(WavesecondTriggerPos - _Player.lock()->GetComponent<Transform>().lock()->GetPosition())
	//	< 0.1f) &&
	//	!bsecond)
	//{
	//	for (auto& spawn_entity : Wavesecond)
	//	{
	//		bsecond = true;
	//		spawn_entity.lock()->SetActive(true);
	//	}
	//}


	//if (bfirst && m_vecQliphothBlock[1].lock()->IsPlaying())
	//{
	//	int count = Wavefirst.size();
	//	for (auto& target : Wavefirst)
	//	{
	//		if (target.expired())
	//		{
	//			--count;
	//		}
	//		if (count <= 0)
	//		{
	//			for (int i = 1; i < 4; ++i)
	//			{
	//				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
	//				{
	//					m_vecQliphothBlock[i].lock()->Reset();
	//					// + render false 된 후에 SetActive(false) 해야
	//				}
	//			}
	//		}
	//	}
	//	/*std::all_of(std::begin(Wavefirst), std::end(Wavefirst),
	//		[]
	//	(const weak_ptr<GameObject>& target)
	//		return target.experiod();
	//		{});*/
	//}


	//if (Input::GetKeyDown(DIK_NUMPAD2))
	//{
	//	for (int i = 1; i < 4; ++i)
	//	{
	//		if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
	//		{
	//			m_vecQliphothBlock[i].lock()->SetActive(true);
	//			m_vecQliphothBlock[i].lock()->PlayStart();
	//		}
	//	}
	//}
	// 
	//if (Input::GetKeyDown(DIK_NUMPAD8))
	//{
	//	for (int i = 1; i < 4; ++i)
	//	{
	//		if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
	//		{
	//			m_vecQliphothBlock[i].lock()->Reset();
	//			// + render false 된 후에 SetActive(false) 해야
	//		}
	//	}
	//}

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
	//std::ifstream inputStream{ "../../Data/Hotel.json" };

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

void TestScene::RenderDataSetUp()
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	 /*_Renderer->LightLoad("..\\..\\Resource\\LightData\\Mission02.json");*/
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
