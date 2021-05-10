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
	m_fLoadingProgress = 0.01f;	// 로딩 시작

	_TitlePanel = AddGameObject<TitlePanel>();

	m_fLoadingProgress = 1.f;	// 로딩 완료

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
		SceneManager::LoadScene(TestScene::Create(), false);

		_LoadNextScene = true;
	}

	_CheckLoadingTick += _fDeltaTime;
	
	if (0.5f < _CheckLoadingTick)
	{
		if (auto SpLTitlePanel = _TitlePanel.lock();
			SpLTitlePanel)
		{
			if (SceneManager::IsLoaded())
			{
				SpLTitlePanel->SetLoadingProgress(1.f);

				if (SpLTitlePanel->IsReadyToNextScene())
					SceneManager::ActivateScene();
			}
			else
			{
				SpLTitlePanel->SetLoadingProgress(m_fLoadingProgress);
			}
		}

		_CheckLoadingTick = 0.f;
	}

	return S_OK;
}

HRESULT TitleScene::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);

	return S_OK;
}