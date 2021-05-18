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
#include "NhDoor.h"
#include "MakaiButterfly.h"
#include "Em100.h"
#include "Trigger.h"
#include "FadeOut.h"
#include "CollObject.h"
#include "BreakableObject.h"
#include "HotelBrokenFloor.h"
#include "HotelAnimationWall.h"
#include "SoundSystem.h"
#include <iostream>
#include <fstream>
#include "NeroFSM.h"
using namespace std;

Hotel_S02::Hotel_S02()
{

}

void Hotel_S02::Free()
{
	for (auto& Element : _MakaiButterflyVec)
		Destroy(Element);
	_MakaiButterflyVec.clear();
	_MakaiButterflyVec.shrink_to_fit();

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

	//if (auto SpCamera = AddGameObject<Camera>().lock();
	//	SpCamera)
	//{
	//	SpCamera->GetComponent<Transform>().lock()->SetPosition(Vector3{ -3.808f, 0.296f, 11.846f });
	//}
	
	AddGameObject<MainCamera>();
	_Player = AddGameObject<Nero>();

#pragma endregion

	m_fLoadingProgress = 0.2f;

#pragma region Monster

#pragma endregion

	m_fLoadingProgress = 0.4f;

#pragma region Map & Objects

	LoadObjects("../../Data/Stage2_Map.json");
	LoadCollObjects("../../Data/Stage2_Object.json");
	LoadBreakablebjects("../../Data/Stage2_BreakableObject.json");

	AddGameObject<HotelBrokenFloor>();
	auto Map = AddGameObject<TempMap>().lock();
	Map->LoadMap(2);

#pragma endregion

	m_fLoadingProgress = 0.6f;

#pragma region RenderData & Trigger

	RenderDataSetUp(false);
	TriggerSetUp();

#pragma endregion

	m_fLoadingProgress = 0.7f;

#pragma region Effect

	_SecretVision = AddGameObject<SecretVision>();
	AddGameObject<NhDoor>();

	// 
	size_t ButterflyCnt = 5u;
	_MakaiButterflyVec.reserve(ButterflyCnt);
	for (size_t i = 0u ; i < ButterflyCnt; ++i)
		_MakaiButterflyVec.push_back(AddGameObject<MakaiButterfly>().lock());

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

	/* ---------- 치트 ---------- */
	if (Input::GetKeyDown(DIK_NUMPAD8))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S02));
	}
	if (Input::GetKeyDown(DIK_NUMPAD9))
	{
		SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S03));
	}
	/* -------------------------- */

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

void Hotel_S02::LoadCollObjects(const std::filesystem::path& path)
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

void Hotel_S02::LoadBreakablebjects(const std::filesystem::path& path)
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

void Hotel_S02::RenderDataSetUp(const bool bTest)
{
	// 렌더러 씬 맵 특성에 맞춘 세팅
	auto _Renderer = Renderer::GetInstance();
	if (bTest)
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Light.json");
	}
	else
	{
		_Renderer->LightLoad("..\\..\\Resource\\LightData\\Hotel_S02.json");
	}
	
	_Renderer->CurSkysphereTex = _Renderer->SkyTexMission02Sunset;
	_Renderer->ao = 0.01f;
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
	TriggerFirstButterFlyMeet();
	TriggerPuzzleStart();
	TriggerWallSmash();
	TriggerNextScene();
}

void Hotel_S02::TriggerWallSmash()
{
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		auto _AnimationWall = AddGameObject<HotelAnimationWall>();
		const std::function<void()> _CallBack =
			[this/*변수 캡쳐*/, _AnimationWall]()
		{
			// 여기서 성큰이 벽을 박살내며 등장 !!
			_AnimationWall.lock()->ContinueAnimation();
			_Player.lock()->GetFsm().lock()->ChangeState(NeroFSM::WINDPRESSURE);
			//
			for (auto& Element : _MakaiButterflyVec)
				Element.lock()->SetActive(false);
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.108f, 1.564f, 20.465f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 1.f, 0.5f, 0.1f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}
}

void Hotel_S02::TriggerFirstButterFlyMeet()
{
	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[this/*변수 캡쳐*/]()
		{
			// 여기서 퍼즐용 나방과 첫 조우 연출 !

			if (auto SpObject = _MakaiButterflyVec[0].lock();
				SpObject)
			{
				SpObject->SetActive(true);
				SpObject->SetRotation({ 0.f, 90.f, 0.f });
				SpObject->SetPosition({ -3.314f, 0.886f, 14.175f });
				SpObject->PlayStart();
			}
		};
		
		// 트리거 위치
		const Vector3 TriggerLocation{ -3.786f, 0.592f, 12.665f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 2.f, 1.f, 1.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}
}

void Hotel_S02::TriggerPuzzleStart()
{
	auto _SecondTrigger = AddGameObject<Trigger>().lock();
	if (_SecondTrigger)
	{
		const std::function<void()> _CallBack =
			[this/*변수 캡쳐*/]()
		{
			if (auto SpObject = _MakaiButterflyVec[1].lock();
				SpObject)
			{
				SpObject->SetActive(true);
				SpObject->SetRotation({ 0.f, 270.f, 0.f });
				SpObject->SetPosition({ -3.163f, 1.555f, 15.013f });
				SpObject->PlayStart();
			}

			for (uint32 i = 2u; i < 5u; ++i)
			{
				if (auto SpObject = _MakaiButterflyVec[i].lock();
					SpObject)
				{
					SpObject->SetActive(true);
					SpObject->PlayStart();
				}
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.3f, 1.565f, 16.13f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 0.5f, 0.5f, 0.1f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = false;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_SecondTrigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}

	if (auto _Trigger = AddGameObject<Trigger>().lock();
		_Trigger)
	{
		const std::function<void()> _CallBack =
			[this, _SecondTrigger/*변수 캡쳐*/]()
		{
			_SecretVision.lock()->PuzzleStart();

			_SecondTrigger->TriggerEnable();
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -3.3f, 1.565f, 17.f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 0.5f, 0.5f, 0.1f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}
}

void Hotel_S02::TriggerNextScene()
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
				_FadeOut->PlayStart(1u,
					[SpPanel]()
					{
						SpPanel->SetNullBlackActive(true);
						SceneManager::LoadScene(LoadingScene::Create(SCENE_ID::HOTEL_S03));
					});
			}
		};

		// 트리거 위치
		const Vector3 TriggerLocation{ -1.516f, 1.751f, 23.954f };
		// 콜라이더 사이즈 
		const Vector3 BoxSize{ 4.f,1.f,1.f };
		// 트리거 정보 등록하자마자 활성화 ?? 
		const bool ImmediatelyEnable = true;
		// 트리거가 검사할 오브젝트 태그 
		const GAMEOBJECTTAG TargetTag = GAMEOBJECTTAG::Player;

		_Trigger->EventRegist(_CallBack,
			TriggerLocation,
			BoxSize,
			ImmediatelyEnable,
			TargetTag);
	}
}

void Hotel_S02::BgmPlay()
{
	SoundSystem::GetInstance()->Play("Maple", 10.f, false, true);
}

void Hotel_S02::LateInit()
{
	// + 플레이어 초기 위치 잡기 등
	if (auto SpPlayer = _Player.lock();
		SpPlayer)
	{
		SpPlayer->GetComponent<Transform>().lock()->SetPosition({ -3.63097f, 0.4f, 11.70365f });
		SpPlayer->GetComponent<Transform>().lock()->SetRotation({ 0.f, 180.f, 0.f });
	}

	//
	if (auto SpObject = _MakaiButterflyVec[0].lock();
		SpObject)
	{
		// 첫번째 조우 나비
		SpObject->SetVariationIdx(MakaiButterfly::VARIATION::MOVE_FORWARD);
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[1].lock();
		SpObject)
	{
		// 두번째 조우 나비
		SpObject->SetVariationIdx(MakaiButterfly::VARIATION::MOVE_FORWARD);
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[2].lock();
		SpObject)
	{
		// 퍼즐용 나비
		SpObject->SetPosition({ -4319.212f * GScale, 470.248f * GScale, 16593.594f * GScale });
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[3].lock();
		SpObject)
	{
		// 퍼즐용 나비
		SpObject->SetPosition({ -804.781f * GScale, 1097.678f * GScale, 16376.986f * GScale });
		SpObject->SetActive(false);
	}
	if (auto SpObject = _MakaiButterflyVec[4].lock();
		SpObject)
	{
		// 퍼즐용 나비
		SpObject->SetPosition({ -4696.414f * GScale, 1683.205f * GScale, 15569.233f * GScale });
		SpObject->SetActive(false);
	}

	MakaiButterfly::ResetTotalCnt();

	Renderer::GetInstance()->LateSceneInit();
	BgmPlay();
	_LateInit = true;
}
