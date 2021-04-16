#include "Scene.h"
#ifndef __GAMEOBJECT_HPP__
#define __GAMEOBJECT_HPP__
BEGIN(ENGINE)
template<typename TYPE>
inline std::weak_ptr<TYPE> GameObject::AddComponent()
{
	size_t nTypeID = typeid(TYPE).hash_code();

	auto iterFind = m_Components.find(nTypeID);

	if (m_Components.end() != iterFind)
	{
		Log("Failed to AddComponent. Component is already exist.");

		PRINT_LOG(TEXT("Warning"), TEXT("Failed to AddComponent. Component is already exist."));
		return std::weak_ptr<TYPE>();
	}

	Component* pComponent = TYPE::Create(m_pGameObject);

	iterFind = (m_Components.emplace(nTypeID, std::shared_ptr<Component>(pComponent, Deleter<Object>()))).first;
	
	pComponent->SetWeakPtr(iterFind->second);

	return std::static_pointer_cast<TYPE>(iterFind->second);
}

template<typename TYPE>
inline std::weak_ptr<TYPE> GameObject::GetComponent()
{
	size_t nTypeID = typeid(TYPE).hash_code();

	auto iterFind = m_Components.find(nTypeID);

	if (m_Components.end() == iterFind)
		return std::weak_ptr<TYPE>();

	return std::static_pointer_cast<TYPE>(iterFind->second);
}

template<typename TYPE>
inline std::weak_ptr<TYPE> GameObject::AddGameObject()
{
	return m_pScene->AddGameObject<TYPE>();
}
END
#endif // !__GAMEOBJECT_HPP__

