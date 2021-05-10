#include "stdafx.h"
#include "..\Header\TitleScene.h"
#include "Scene.h"
#include "TitlePanel.h"

#include "TestScene.h"


void TitleScene::Free()
{
	Scene::Free();
}

TitleScene* TitleScene::Create()
{
	TitleScene* pInstance = new TitleScene;
	return pInstance;
}


HRESULT TitleScene::LoadScene()
{
	_TitlePanel = AddGameObject<TitlePanel>();

	return S_OK;
}

HRESULT TitleScene::Awake()
{
	Scene::Awake();

	return S_OK;
}

HRESULT TitleScene::Start()
{
	Scene::Start();

	return S_OK;
}

HRESULT TitleScene::Update(const float _fDeltaTime)
{
	Scene::Update(_fDeltaTime);

	if (!_LoadNextScene)
	{
		// 다음 씬 로드
		//SceneManager::LoadScene(TestScene::Create(), false);

		_LoadNextScene = true;
	}
	else if (SceneManager::IsLoaded())
	{
		//if (auto SpLogoPanel = _LogoPanel.lock();
		//	SpLogoPanel) 
		//{
		//	SpLogoPanel->SetLoadingFinishedFlag(true);

		//	if (SpLogoPanel->IsReadyToNextScene())
		//		SceneManager::ActivateScene();
		//}
	}

	return S_OK;
}

HRESULT TitleScene::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);

	return S_OK;
}