#include "stdafx.h"
#include "Application.h"
#include "LogoScene.h"

HRESULT Application::ReadyApplication(const bool bWindowed, const bool bMultiSample)
{
	if (FAILED(Engine::ReadyEngine(bWindowed, bMultiSample)))
		return E_FAIL;

	// ó�� ���� Scene �ٲٷ��� LogoScene -> Update���� �ּ� �����ϱ� BARAM - by hscho
	SceneManager::LoadScene(LogoScene::Create());

	return S_OK;
}

HRESULT Application::UpdateApplication(const float Delta)
{
	if (FAILED(Engine::UpdateEngine(Delta)))
		return E_FAIL;
	return S_OK;
}

HRESULT Application::ReleaseApplication()
{
	if (FAILED(Engine::ReleaseEngine()))
		return E_FAIL;
	return S_OK;
}