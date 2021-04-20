#include "Component.h"
#include "GameObject.h"

USING(ENGINE)

Component::Component(std::weak_ptr<GameObject> const _pGameObject)
	: m_bActive(true)
	, m_pGameObject(_pGameObject)
{
	m_pThis.reset(this, DummyDeleter<Object>());
}

void Component::Free()
{
	Object::Free();
}

void Component::Editor()
{
	Object::Editor();
}

std::weak_ptr<GameObject> Component::GetGameObject()
{
	return m_pGameObject;
}

bool Component::IsActive()
{
	return m_bActive;
}

void Component::SetActive(const bool _bActive)
{
	m_bActive = _bActive;
}
