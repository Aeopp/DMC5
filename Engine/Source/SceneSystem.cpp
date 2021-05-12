#include "SceneSystem.h"
#include "Scene.h"
#include "ResourceSystem.h"
#include "PhysicsSystem.h"

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
	//�ε� üũ
	CheckThread();
	//�� ��ȯ
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

	//�ε��� ���� ���� ���� ���� ���
	if (pTemporary == m_pCurrentScene)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to LoadScene. Parameter '_pScene' is same with current scene."));
		return E_FAIL;
	}

	//�̹� �ٸ� �� �ε��� �������� ���
	if (NULL != m_hThread || LOADINGSTATE::NONE != m_eLoadingState)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("m_hThread is not NULL."));
		return E_FAIL;
	}

	if(FAILED(PhysicsSystem::GetInstance()->CreateScene(pTemporary->UniqueID)))
		return E_FAIL;
	
	//�ε� ������ ����
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, LoadingThread, this, 0, NULL);

	//�ε� ������ ������ ������ ���
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

	//�ε� �����尡 ���� ��
	if (STILL_ACTIVE == dwExitCode)
		return;

	//������ �ڵ� ��ȯ �� �� NULL �ʱ�ȭ.
	CloseHandle(m_hThread);
	m_hThread = NULL;

	m_eLoadingState = LOADINGSTATE::FINISH;
}

void SceneSystem::SwitchScene()
{
	//�ε��� ����
	if (LOADINGSTATE::FINISH != m_eLoadingState)
		return;
	if (false == m_bSceneActivation)
		return;

	//���� ���� Static Object�� ���� ���� ����.
	if (nullptr != m_pCurrentScene)
		m_pCurrentScene->TransferStatic(m_pNextScene);

	//���� �� ����
	m_pCurrentScene.reset();
	//�� ��ȯ
	m_pCurrentScene = m_pNextScene;
	PhysicsSystem::GetInstance()->ChangeScene(m_pCurrentScene->UniqueID);
	//
	m_pNextScene.reset();
	//
	m_eLoadingState = LOADINGSTATE::NONE;
	//
	ResourceSystem::GetInstance()->Clear();
}

unsigned int SceneSystem::LoadingThread(void* _pArg)
{
	SceneSystem* pSceneSystem = (SceneSystem*)_pArg;
	
	pSceneSystem->m_pNextScene->LoadScene();

	return 0;
}
