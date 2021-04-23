#include "GameObject.h"
#include "Scene.h"
#include "Renderer.h"
#include "Transform.h"
#include "Collider.h"

USING(ENGINE)

GameObject::GameObject()
	: m_bActive(true)
	, m_pScene(nullptr)
	, m_nLoopIdx(0)
	, m_bStatic(false)
	, m_bDestroy(false)
	, m_bRenderRegist(false)
{
	m_pTransform = AddComponent<Transform>();
	m_pGameObject = std::static_pointer_cast<GameObject>(m_pThis);
}

void GameObject::Free()
{
	for (auto& rPair : m_Components)
		rPair.second.reset();

	m_Components.clear();
	Object::Free();
}

HRESULT GameObject::Ready()
{
	return S_OK;
}

HRESULT GameObject::Awake()
{
	return S_OK;
}

HRESULT GameObject::Start()
{
	return S_OK;
}

UINT GameObject::Update(const float _fDeltaTime)
{
	m_pTransform.lock()->UpdateTransform();
	return 0;
}

UINT GameObject::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

std::weak_ptr<GameObject> GameObject::FindGameObjectWithTag(const UINT& _nTag)
{
	return m_pScene->FindGameObjectWithTag(_nTag);
}

std::list<std::weak_ptr<GameObject>> GameObject::FindGameObjectsWithTag(const UINT& _nTag)
{
	return m_pScene->FindGameObjectsWithTag(_nTag);
}

std::list<std::weak_ptr<GameObject>> GameObject::FindGameObjectsWithLayer(const UINT& _nLayer)
{
	return m_pScene->FindGameObjectsWithLayer(_nLayer);
}

void GameObject::Destroy(std::weak_ptr<GameObject> const _pGameObject)
{
	m_pScene->Destroy(_pGameObject);
}

bool GameObject::IsRenderEnable()
{
	return m_bRenderRegist; 
}

bool GameObject::IsActive()
{
	return m_bActive;
}

void GameObject::SetRenderEnable(const bool bActiveRender)&
{
	m_bRenderRegist = bActiveRender;
}

void GameObject::SetActive(const bool _bActive)
{
	if (_bActive == m_bActive)
		return;

	m_bActive = _bActive;

	if (_bActive)
		OnEnable();
	else
		OnDisable();

	m_pScene->SetActive(m_pGameObject, _bActive);
}

bool GameObject::IsStatic()
{
	return m_bStatic;
}

void GameObject::SetStatic(const bool _bStatic)
{
	if (_bStatic == m_bStatic)
		return;
	m_bStatic = _bStatic;
}

void GameObject::PushEditEntity(Object* _EditObject)
{
	m_pEditObjects.push_back(_EditObject);
}

void GameObject::SetScene(Scene* const _pScene)
{
	m_pScene = _pScene;
}

UINT GameObject::GetLoopIdx()
{
	return m_nLoopIdx;
}

void GameObject::SetLoopIdx(const UINT _nLoopIdx)
{
	m_nLoopIdx = _nLoopIdx;
}

UINT GameObject::GetSceneID()
{
	if (nullptr == m_pScene)
		return 0;

	return m_pScene->UniqueID;
}

void GameObject::DrawCollider(const DrawInfo& _Info)
{
	for (auto& rPair : m_Components)
	{
		if (nullptr != std::dynamic_pointer_cast<Collider>(rPair.second))
		{
			std::static_pointer_cast<Collider>(rPair.second)->DrawCollider(_Info);
			return;
		}
	}
}

void GameObject::Editor()
{
	Object::Editor();

	if (bEdit)
	{
		ImGui::Begin(EditName.c_str());
		for (auto& _EditObj : m_pEditObjects)
		{
			_EditObj->Editor();
		}
		ImGui::End();
	}
}

void GameObject::OnEnable()
{
}

void GameObject::OnDisable()
{
}

void GameObject::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
}

void GameObject::OnCollisionStay(std::weak_ptr<GameObject> _pOther)
{
}

void GameObject::OnCollisionExit(std::weak_ptr<GameObject> _pOther)
{
}

void GameObject::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void GameObject::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}
