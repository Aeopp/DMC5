#include "GameObject.h"
#include "Scene.h"
#include "Renderer.h"
#include "Transform.h"

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
}

void GameObject::Free()
{
	Object::Free();
}

std::weak_ptr<GameObject> GameObject::FindGameObjectWithTag(const UINT& _nTag)
{
	return m_pScene->FindGameObjectWithTag(_nTag);
}

std::list<std::weak_ptr<GameObject>> GameObject::FindGameObjectsWithTag(const UINT& _nTag)
{
	return m_pScene->FindGameObjectsWithTag(_nTag);
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

BT_INFO GameObject::Get_BattleInfo()
{
	return m_BattleInfo;
}

void GameObject::SetScene(Scene* const _pScene)
{
	m_pScene = _pScene;
}

void GameObject::SetGameObject(std::weak_ptr<GameObject> _pGameObject)
{
	m_pGameObject = _pGameObject;
}

UINT GameObject::GetLoopIdx()
{
	return m_nLoopIdx;
}

void GameObject::SetLoopIdx(const UINT _nLoopIdx)
{
	m_nLoopIdx = _nLoopIdx;
}

UINT GameObject::Update(const float _fDeltaTime)
{
	m_pTransform.lock()->UpdateTransform();
	return 0;
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
