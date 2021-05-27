#include "stdafx.h"
#include "..\Header\LogoScene.h"
#include "Scene.h"
#include "LogoPanel.h"
#include "TitleScene.h"
#include "TestScene.h"
#include "Hotel_S02.h"
#include "Hotel_S01.h"
#include "Hotel_S03.h"
#include "Hotel_S04.h"
#include "Library_S06.h"
#include "Library_S05.h"

void LogoScene::Free()
{
	Scene::Free();
}

LogoScene* LogoScene::Create()
{
	LogoScene* pInstance = new LogoScene;
	return pInstance;
}


HRESULT LogoScene::LoadScene()
{
	m_fLoadingProgress = 0.01f;	// 로딩 시작

	_LogoPanel = AddGameObject<LogoPanel>();
	_LogoPanel.lock()->QuickProgressToNextScene();	// 엔터 안치고 바로 다음씬

	m_fLoadingProgress = 1.f;	// 로딩 완료

	return S_OK;
}

HRESULT LogoScene::Awake()
{
	Scene::Awake();

	return S_OK;
}

HRESULT LogoScene::Start()
{
	Scene::Start();

	return S_OK;
}

HRESULT LogoScene::Update(const float _fDeltaTime)
{
	Scene::Update(_fDeltaTime);

	if (!_LoadNextScene)
	{
		// 다음 씬 로드
		SceneManager::LoadScene(TitleScene::Create(), false);
		//SceneManager::LoadScene(TestScene::Create(), false);
		//SceneManager::LoadScene(Hotel_S01::Create(), false);
		//SceneManager::LoadScene(Hotel_S02::Create(), false);
		

		_LoadNextScene = true;
	}
	else if (SceneManager::IsLoaded())
	{
		if (auto SpLogoPanel = _LogoPanel.lock();
			SpLogoPanel) 
		{
			SpLogoPanel->SetLoadingFinishedFlag(true);

			if (SpLogoPanel->IsReadyToNextScene())
				SceneManager::ActivateScene();
		}
	}

	return S_OK;
}

HRESULT LogoScene::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);

	return S_OK;
}
