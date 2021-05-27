#include "stdafx.h"
#include "..\Header\LoadingScene.h"
#include "Hotel_S01.h"
#include "Hotel_S02.h"
#include "Hotel_S03.h"
#include "Hotel_S04.h"
#include "Library_S05.h"
#include "Library_S06.h"
#include "EndingScene.h"
#include "TestScene.h"
#include "LoadingPanel.h"
#include "SoundSystem.h"


void LoadingScene::Free()
{
	Scene::Free();
}

LoadingScene* LoadingScene::Create(SCENE_ID NextSceneID)
{
	LoadingScene* pInstance = new LoadingScene;
	pInstance->_NextSceneID = NextSceneID;
	return pInstance;
}


HRESULT LoadingScene::LoadScene()
{
	m_fLoadingProgress = 0.01f;	// 로딩 시작

	_LoadingPanel = AddGameObject<LoadingPanel>();

	m_fLoadingProgress = 1.f;	// 로딩 완료

	return S_OK;
}

HRESULT LoadingScene::Awake()
{
	Scene::Awake();

	return S_OK;
}

HRESULT LoadingScene::Start()
{
	Scene::Start();

	return S_OK;
}

HRESULT LoadingScene::Update(const float _fDeltaTime)
{
	Scene::Update(_fDeltaTime);

	if (!_LoadNextScene)
	{
		// 다음 씬 로드
		switch (_NextSceneID)
		{
		case HOTEL_S01:
			SceneManager::LoadScene(Hotel_S01::Create(), false);
			break;
		case HOTEL_S02:
			SceneManager::LoadScene(Hotel_S02::Create(), false);
			break;
		case HOTEL_S03:
			SceneManager::LoadScene(Hotel_S03::Create(), false);
			break;
		case HOTEL_S04:
			SceneManager::LoadScene(Hotel_S04::Create(), false);
			break;
		case LIBRARY_S05:
			SceneManager::LoadScene(Library_S05::Create(), false);
			break;
		case LIBRARY_S06:
			SceneManager::LoadScene(Library_S06::Create(), false);
			break;
		case ENDING:
			SceneManager::LoadScene(EndingScene::Create(), false);
			_LoadingPanel.lock()->SetShowLoadingProgress(false);
			break;
		default:
			SceneManager::LoadScene(TestScene::Create(), false);
			break;
		}

		_LoadNextScene = true;
	}

	_CheckLoadingTick += _fDeltaTime;

	if (0.5f < _CheckLoadingTick)
	{
		if (auto SpPanel = _LoadingPanel.lock();
			SpPanel)
		{
			if (SceneManager::IsLoaded())
			{
				SoundSystem::GetInstance()->Play("LoadingComplete", 1.f, false);

				SpPanel->SetLoadingProgress(1.f);

				if (SpPanel->IsReadyToNextScene())
					SceneManager::ActivateScene();
			}
			else
			{
				SpPanel->SetLoadingProgress(m_fLoadingProgress);
			}
		}

		_CheckLoadingTick = 0.f;
	}

	return S_OK;
}

HRESULT LoadingScene::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);

	return S_OK;
}
