#include "Scene.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Input.h"

USING(ENGINE)

float Scene::m_fLoadingProgress = 0.f;

Scene::Scene()
	: m_bOnLoop(false)
{
	m_fLoadingProgress = 0.f;
}

void Scene::Free()
{
	for (UINT i = 0; i < ACTIVE_END; ++i)
		for (UINT j = 0; j < LOOP_END; ++j)
			m_Loop[i][j].clear();

	for (UINT i = 0; i < POOL_END; ++i)
	{
		for (auto& rGameObject : m_Pool[i])
			rGameObject.reset();
		m_Pool[i].clear();
	}

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

	m_bOnLoop = false;

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
	m_bOnLoop = true;
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
	m_bOnLoop = false;
	m_Loop[ACTIVE][LOOP_START].clear();
	return S_OK;
}

HRESULT Scene::Update(const float _fDeltaTime)
{
	m_bOnLoop = true;
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
	m_bOnLoop = false;

	return S_OK;
}



HRESULT Scene::LateUpdate(const float _fDeltaTime)
{
	m_bOnLoop = true;
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
	m_bOnLoop = false;
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
		if (auto _Element = (*m_LoopIter).lock();
			_Element)
		{
			if (_Element->m_bEditable)
			{
				_Element->Editor();
			}
		}

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

	OBJECTPOOL listTemp = m_Pool[OBJECTPOOLTYPE::POOL_DESTROY];	// ?????? refcnt?? 1 ????
	m_Pool[OBJECTPOOLTYPE::POOL_DESTROY].clear();				// refcnt?? 0?? ??
	listTemp.clear();											// ???? ???????? Free ????
}

void Scene::TransferStatic(std::weak_ptr<Scene> const _pDestScene)
{
	if (_pDestScene.expired() || 0 == m_Pool[POOL_STATIC].size())
		return;
	for (auto iter = m_Pool[POOL_STATIC].begin(); iter != m_Pool[POOL_STATIC].end(); ++iter)
	{
		_pDestScene.lock()->m_Pool[POOL_STATIC].push_back(*iter);

		ACTIVESTATE eActiveState = (*iter)->IsActive() ? ACTIVESTATE::ACTIVE : ACTIVESTATE::INACTIVE;

		_pDestScene.lock()->m_Loop[eActiveState][LOOP_UPDATE].push_back(*iter);
	}
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

std::list<std::weak_ptr<GameObject>> Scene::FindGameObjectsWithLayer(const UINT& _nLayer)
{
	std::list<std::weak_ptr<GameObject>> listFind;

	for (UINT i = 0; i < LOOPSTATE::LOOP_END; ++i)
	{
		for (auto iter = m_Loop[ACTIVE][i].begin(); iter != m_Loop[ACTIVE][i].end(); ++iter)
		{
			if (false == (*iter).expired() && _nLayer == (*iter).lock()->m_nLayer)
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
			//nSrcIdx???? ???? ?????? nDestIdx?? ?????????? ????.
			m_Loop[nDestIdx][nLoopIdx].emplace_back(*iterFind);
			
			//?????? ?????? ?????? ???????? ?????? ???????? ????.
			//???? ?????? ???? Loop ???? ???? ???????? m_LoopIter?? ???? ?????? ???? ?? ???????? ????????ssssssssssssssssw.
			if (m_bOnLoop && 0 != nLoopIdx)
			if ((false == _bActive) && (_pGameObject.lock() == (*m_LoopIter).lock()) && m_LoopIter == iterFind)
				--m_LoopIter;
			//nSrcIdx???? ????
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
