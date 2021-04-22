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
#include "BtlPanel.h"
#include "MainCamera.h"
#include "Renderer.h"

#include <iostream>
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
	//AddGameObject<MainCamera>();
	AddGameObject<Camera>();
	
	//AddGameObject<Nero>();
	AddGameObject<Em100>();
	//AddGameObject<Car>();
	
	Renderer::GetInstance()->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	//AddGameObject<TempMap>();
	Renderer::GetInstance()->exposure = 10.0f;
	//AddGameObject<ShaderTester>();
	
	/*AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();
	AddGameObject<TestAnimationObject>();*/

	AddGameObject<BtlPanel>();

	//AddGameObject<Glint>();
	//AddGameObject<OvertureHand>();

	//AddGameObject<DashImpact>();
	//AddGameObject<Blood>();

	return S_OK;
}

HRESULT TestScene::Awake()
{
	Scene::Awake();

	if (nullptr != pPlane)
		return S_OK;

	pPlane = PxCreatePlane(*Physics::GetPxPhysics(), PxPlane(0.f, 1.f, 0.f, 0.f) , *Physics::GetDefaultMaterial());

	Physics::AddActor(UniqueID, *pPlane);
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
