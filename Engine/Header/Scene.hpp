#ifndef __SCENE_HPP__
#define __SCENE_HPP__
BEGIN(ENGINE)
template<typename TYPE>
inline std::weak_ptr<TYPE> Scene::AddGameObject()
{
	GameObject* pInstance = TYPE::Create();

	if (nullptr == pInstance)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to AddGameObject. ' pInstance' is nullptr."));
		return std::weak_ptr<TYPE>();
	}

	std::shared_ptr<GameObject> pGameObject(pInstance, Deleter<Object>());

	pGameObject->SetScene(this);
	pGameObject->SetGameObject(pGameObject);

	if (FAILED(pGameObject->Ready()))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to AddGameObject. Failed to Ready GameObject."));
		pGameObject.reset();
		return std::weak_ptr<TYPE>();
	}

	//오브젝트 풀에 보관
	m_Pool[POOL_DYNAMIC].emplace_back(pGameObject);
	//
	m_Loop[ACTIVE][LOOP_AWAKE].emplace_back(pGameObject);

	return std::static_pointer_cast<TYPE>(pGameObject);
}
END
#endif // !__SCENE_HPP__
