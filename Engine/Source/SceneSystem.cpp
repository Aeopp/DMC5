#include "SceneSystem.h"
#include "Scene.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(SceneSystem)

SceneSystem::SceneSystem()
	: m_pCurrentScene(nullptr)
	, m_pNextScene(nullptr)
	, m_hThread(NULL)
	, m_bSceneActivation(false)
	, m_eLoadingState(SceneSystem::LOADINGSTATE::NONE)
{
}

void SceneSystem::Free()
{
	if (NULL != m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
	}
	m_pNextScene.reset();
	m_pCurrentScene.reset();
	Object::Free();
}

HRESULT SceneSystem::ReadySceneSystem()
{
	return S_OK;
}

HRESULT SceneSystem::UpdateSceneSystem(const float _fDeltaTime)
{
	//로딩 체크
	CheckThread();
	//씬 전환
	SwitchScene();

	if (nullptr == m_pCurrentScene)
		return S_OK;

	m_pCurrentScene->Awake();
	m_pCurrentScene->Start();
	m_pCurrentScene->Update(_fDeltaTime);
	m_pCurrentScene->LateUpdate(_fDeltaTime);
	m_pCurrentScene->PushToRenderSystem();
	return S_OK;
}

void SceneSystem::EditUpdateSceneSystem()
{
	if (nullptr == m_pCurrentScene)
		return;

	m_pCurrentScene->EditUpdate();
}

HRESULT SceneSystem::LoadScene(Scene* const _pScene, const bool _bSceneActivation)
{
	if (nullptr == _pScene)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to LoadScene. Parameter '_pScene' is nullptr."));
		return E_FAIL;
	}

	std::shared_ptr<Scene> pTemporary;
	pTemporary.reset(_pScene, Deleter<Object>());

	m_bSceneActivation = _bSceneActivation;

	//로딩할 씬이 현재 씬과 같은 경우
	if (pTemporary == m_pCurrentScene)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to LoadScene. Parameter '_pScene' is same with current scene."));
		return E_FAIL;
	}

	//이미 다른 씬 로딩이 진행중인 경우
	if (NULL != m_hThread || LOADINGSTATE::NONE != m_eLoadingState)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("m_hThread is not NULL."));
		return E_FAIL;
	}

	//로딩 쓰레드 생성
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadingThread, this, 0, NULL);

	//로딩 쓰레드 생성에 실패한 경우
	if (INVALID_HANDLE_VALUE == m_hThread)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to _beginthreadex."));
		m_hThread = NULL;
		return E_FAIL;
	}

	m_pNextScene = pTemporary;

	m_eLoadingState = LOADINGSTATE::LOADING;
	return S_OK;
}

bool SceneSystem::IsLoaded()
{
	return (LOADINGSTATE::FINISH == m_eLoadingState);
}

void SceneSystem::ActivateScene()
{
	m_bSceneActivation = true;
}

void SceneSystem::CheckThread()
{
	if (NULL == m_hThread)
		return;

	DWORD dwExitCode = 0;
	GetExitCodeThread(m_hThread, &dwExitCode);

	//로딩 쓰레드가 진행 중
	if (STILL_ACTIVE == dwExitCode)
		return;

	//쓰레드 핸들 반환 및 값 NULL 초기화.
	CloseHandle(m_hThread);
	m_hThread = NULL;

	m_eLoadingState = LOADINGSTATE::FINISH;
}

void SceneSystem::SwitchScene()
{
	//로딩이 진행
	if (LOADINGSTATE::FINISH != m_eLoadingState)
		return;
	if (false == m_bSceneActivation)
		return;

	//현재 씬의 Static Object를 다음 씬에 전달.
	if (nullptr != m_pCurrentScene)
		m_pCurrentScene->TransferStatic(m_pNextScene);

	//현재 씬 리셋
	m_pCurrentScene.reset();
	//씬 전환
	m_pCurrentScene = m_pNextScene;
	//
	m_pNextScene.reset();
	//
	m_eLoadingState = LOADINGSTATE::NONE;
}

unsigned int SceneSystem::LoadingThread(void* _pArg)
{
	SceneSystem* pSceneSystem = (SceneSystem*)_pArg;
	
	pSceneSystem->m_pNextScene->LoadScene();

	return 0;
}
