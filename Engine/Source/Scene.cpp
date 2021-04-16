#include "Scene.h"
#include "GameObject.h"
#include "Renderer.h"

USING(ENGINE)

Scene::Scene()
{
}

void Scene::Free()
{
	for (UINT i = 0; i < ACTIVE_END; ++i)
		for (UINT j = 0; j < LOOP_END; ++j)
			m_Loop[i][j].clear();

	for (UINT i = 0; i < POOL_END; ++i)
		m_Pool[i].clear();
	Object::Free();
}

HRESULT Scene::LoadScene()
{
	return S_OK;
}

HRESULT Scene::Awake()
{
	Destroy();

	if (0 == m_Loop[ACTIVE][LOOP_AWAKE].size())
		return S_OK;

	for (auto pGameObject : m_Loop[ACTIVE][LOOP_AWAKE])
	{
		if (pGameObject.expired())
			continue;

		if (FAILED(pGameObject.lock()->Awake()))
		{
			PRINT_LOG(TEXT("Warning"), TEXT("Failed to Awake GameObject."));
			return E_FAIL;
		}

		pGameObject.lock()->SetLoopIdx((UINT)LOOP_START);

		if (pGameObject.lock()->IsActive())
			m_Loop[ACTIVE][LOOP_START].emplace_back(pGameObject);
		else
			m_Loop[INACTIVE][LOOP_START].emplace_back(pGameObject);
	}

	m_Loop[ACTIVE][LOOP_AWAKE].clear();
	return S_OK;
}

HRESULT Scene::Start()
{
	if (0 == m_Loop[ACTIVE][LOOP_START].size())
		return S_OK;

	for (m_LoopIter = m_Loop[ACTIVE][LOOP_START].begin(); m_LoopIter != m_Loop[ACTIVE][LOOP_START].end(); ++m_LoopIter)
	{
		if ((*m_LoopIter).expired())
			continue;

		m_Loop[ACTIVE][LOOP_UPDATE].emplace_back(*m_LoopIter);
		(*m_LoopIter).lock()->SetLoopIdx((UINT)LOOP_UPDATE);
		if (FAILED((*m_LoopIter).lock()->Start()))
		{
			PRINT_LOG(TEXT("Warning"), TEXT("Failed to Start GameObject."));
			return E_FAIL;
		}
	}

	m_Loop[ACTIVE][LOOP_START].clear();
	return S_OK;
}

HRESULT Scene::Update(const float _fDeltaTime)
{
	for (m_LoopIter = m_Loop[ACTIVE][LOOP_UPDATE].begin(); m_LoopIter != m_Loop[ACTIVE][LOOP_UPDATE].end(); )
	{
		if ((*m_LoopIter).expired())
		{
			m_LoopIter = m_Loop[ACTIVE][LOOP_UPDATE].erase(m_LoopIter);
			continue;
		}
		(*m_LoopIter).lock()->Update(_fDeltaTime);
		++m_LoopIter;
	}
	return S_OK;
}

HRESULT Scene::LateUpdate(const float _fDeltaTime)
{
	for (m_LoopIter = m_Loop[ACTIVE][LOOP_UPDATE].begin(); m_LoopIter != m_Loop[ACTIVE][LOOP_UPDATE].end(); )
	{
		if ((*m_LoopIter).expired())
		{
			m_LoopIter = m_Loop[ACTIVE][LOOP_UPDATE].erase(m_LoopIter);
			continue;
		}
		(*m_LoopIter).lock()->LateUpdate(_fDeltaTime);

		++m_LoopIter;
	}
	return S_OK;
}

void Scene::EditUpdate()
{
	for (m_LoopIter = m_Loop[ACTIVE][LOOP_UPDATE].begin(); m_LoopIter != m_Loop[ACTIVE][LOOP_UPDATE].end(); )
	{
		if ((*m_LoopIter).expired())
		{
			m_LoopIter = m_Loop[ACTIVE][LOOP_UPDATE].erase(m_LoopIter);
			continue;
		}
		(*m_LoopIter).lock()->Editor();

		++m_LoopIter;
	}
}

void Scene::PushToRenderSystem()
{
	for (auto& pGameObject : m_Loop[ACTIVE][LOOP_UPDATE])
	{
		Renderer::GetInstance()->Push(pGameObject);
	}
}

void Scene::Destroy()
{
	if (0 == m_Pool[OBJECTPOOLTYPE::POOL_DESTROY].size())
		return;

	m_Pool[OBJECTPOOLTYPE::POOL_DESTROY].clear();
}

void Scene::TransferStatic(std::weak_ptr<Scene> const _pDestScene)
{
	if (_pDestScene.expired() || 0 == m_Pool[POOL_STATIC].size())
		return;
	_pDestScene.lock()->m_Pool[POOL_STATIC].splice(m_Pool[POOL_STATIC].end(), m_Pool[POOL_STATIC]);
}

std::weak_ptr<GameObject> Scene::FindGameObjectWithTag(const UINT& _nTag)
{	
	for (UINT i = 0; i < LOOPSTATE::LOOP_END; ++i)
	{
		for (auto iter = m_Loop[ACTIVE][i].begin(); iter != m_Loop[ACTIVE][i].end(); ++iter)
		{
			if (false == (*iter).expired() && _nTag == (*iter).lock()->m_nTag)
				return (*iter);
		}
	}
	return std::weak_ptr<GameObject>();
}

std::list<std::weak_ptr<GameObject>> Scene::FindGameObjectsWithTag(const UINT& _nTag)
{
	std::list<std::weak_ptr<GameObject>> listFind;

	for (UINT i = 0; i < LOOPSTATE::LOOP_END; ++i)
	{
		for (auto iter = m_Loop[ACTIVE][i].begin(); iter != m_Loop[ACTIVE][i].end(); ++iter)
		{
			if (false == (*iter).expired() && _nTag == (*iter).lock()->m_nTag)
				listFind.push_back((*iter));
		}
	}
	return listFind;
}

void Scene::Destroy(std::weak_ptr<GameObject> const _pGameObject)
{
	if (_pGameObject.expired() || _pGameObject.lock()->m_bDestroy)
		return;

	UINT nPoolIdx = (_pGameObject.lock()->IsStatic() ? (UINT)OBJECTPOOLTYPE::POOL_STATIC : (UINT)OBJECTPOOLTYPE::POOL_DYNAMIC);

	for (auto iterFind = m_Pool[nPoolIdx].begin(); iterFind != m_Pool[nPoolIdx].end(); ++iterFind)
	{
		if (_pGameObject.lock() == (*iterFind))
		{
			m_Pool[OBJECTPOOLTYPE::POOL_DESTROY].emplace_back(*iterFind);
			(*iterFind)->m_bDestroy = true;
			m_Pool[nPoolIdx].erase(iterFind);
			return;
		}
	}
}

void Scene::SetActive(std::weak_ptr<GameObject> const _pGameObject, const bool _bActive)
{
	if (_pGameObject.expired())
		return;

	UINT nSrcIdx	= (_bActive ? 1 : 0);
	UINT nDestIdx	= (_bActive ? 0 : 1);

	LOOPPOOL::iterator iterFind;

	UINT nLoopIdx = _pGameObject.lock()->GetLoopIdx();

	for (iterFind = m_Loop[nSrcIdx][nLoopIdx].begin(); iterFind != m_Loop[nSrcIdx][nLoopIdx].end(); ++iterFind)
	{
		if (_pGameObject.lock() == (*iterFind).lock())
		{
			//nSrcIdx에서 찾은 객체를 nDestIdx의 컨테이너로 전달.
			m_Loop[nDestIdx][nLoopIdx].emplace_back(*iterFind);
			
			//활성화 상태인 객체를 비활성화 상태로 변경하는 경우.
			//만약 객체가 현재 Loop 진행 중인 경우라면 m_LoopIter를 이전 노드로 옮긴 뒤 목록에서 제거한다.
			if ((false == _bActive) && (_pGameObject.lock() == (*m_LoopIter).lock()) && m_LoopIter == iterFind)
				--m_LoopIter;
			//nSrcIdx에서 제거
			m_Loop[nSrcIdx][nLoopIdx].erase(iterFind);
			return;
		}
	}
}

void Scene::SetStatic(std::weak_ptr<GameObject> const _pGameObject, const bool _bStatic)
{
	if (_pGameObject.expired())
		return;

	UINT nSrcIdx = (_bStatic ? 1 : 0);
	UINT nDestIdx = (_bStatic ? 0 : 1);

	for (auto iterFind = m_Pool[nSrcIdx].begin(); iterFind != m_Pool[nSrcIdx].end(); ++iterFind)
	{
		if (_pGameObject.lock() == (*iterFind))
		{
			m_Pool[nDestIdx].push_back(*iterFind);
			m_Pool[nSrcIdx].erase(iterFind);
			return;
		}
	}
}
