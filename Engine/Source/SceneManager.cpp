#include "SceneManager.h"
#include "Scene.h"
#include "SceneSystem.h"

USING(ENGINE)

std::weak_ptr<SceneSystem> SceneManager::m_pSceneSystem = SceneSystem::GetInstance();

HRESULT SceneManager::LoadScene(Scene* const _pScene, const bool _bSceneActivation)
{
	if (nullptr == m_pSceneSystem.lock())
		return E_FAIL;

	return m_pSceneSystem.lock()->LoadScene(_pScene, _bSceneActivation);
}

bool SceneManager::IsLoaded()
{
	if (nullptr == m_pSceneSystem.lock())
		return false;

	return m_pSceneSystem.lock()->IsLoaded();
}

void SceneManager::ActivateScene()
{
	if (nullptr == m_pSceneSystem.lock())
		return;

	m_pSceneSystem.lock()->ActivateScene();
}
