#include "stdafx.h"
#include "..\Header\EndingScene.h"
#include "Scene.h"
#include "BtlPanel.h"
#include "Font.h"

void EndingScene::Free()
{
	Scene::Free();
}

EndingScene* EndingScene::Create()
{
	EndingScene* pInstance = new EndingScene;
	return pInstance;
}

HRESULT EndingScene::LoadScene()
{
	m_fLoadingProgress = 0.01f;

	if (auto Sp = AddGameObject<BtlPanel>().lock(); Sp)
	{
		Sp->SetRedOrbActive(false);
		Sp->SetNullBlackActive(true);
	}

	Font0 = AddGameObject<Font>();
	if (auto SpFont = Font0.lock(); SpFont)
	{
		SpFont->SetText("THANK YOU FOR WATCHING.",
			Font::TEX_ID::DMC5_BLACK_GRAD,
			Vector2(290.f, 240.f),
			Vector2(1.4f, 1.4f));

		SpFont->SetRenderFlag(false);
	}

	Font1 = AddGameObject<Font>();
	if (auto SpFont = Font1.lock(); SpFont)
	{
		SpFont->SetText("TEAM JUSIN 111A",
			Font::TEX_ID::DMC5_BLACK_GRAD,
			Vector2(470.f, 420.f),
			Vector2(1.1f, 1.1f));

		SpFont->SetRenderFlag(false);
	}

	Sleep(3000);

	m_fLoadingProgress = 1.f;

	return S_OK;
}

HRESULT EndingScene::Awake()
{
	Scene::Awake();

	return S_OK;
}

HRESULT EndingScene::Start()
{
	Scene::Start();

	return S_OK;
}

HRESULT EndingScene::Update(const float _fDeltaTime)
{
	Scene::Update(_fDeltaTime);

	_Tick += _fDeltaTime;

	if (7.f < _Tick)
		Font1.lock()->SetRenderFlag(false, Font::FADE_ID::DISOLVE_TORIGHT);
	else if (6.f < _Tick)
		Font0.lock()->SetRenderFlag(false, Font::FADE_ID::DISOLVE_TORIGHT);
	else if (2.5f < _Tick)
		Font1.lock()->SetRenderFlag(true, Font::FADE_ID::DISOLVE_TORIGHT);
	else if (1.5f < _Tick)
		Font0.lock()->SetRenderFlag(true, Font::FADE_ID::DISOLVE_TORIGHT);

	return S_OK;
}

HRESULT EndingScene::LateUpdate(const float _fDeltaTime)
{
	Scene::LateUpdate(_fDeltaTime);

	return S_OK;
}
