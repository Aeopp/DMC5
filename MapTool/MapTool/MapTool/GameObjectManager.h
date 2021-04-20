#ifndef __GAMEOBJECT_MANAGER_H__
#define __GAMEOBJECT_MANAGER_H__
#include "Physics.h"
class GameObject;
class GameObjectManager
{
public:
	static GameObjectManager Instance;
public:
	GameObject* m_pSelected;
	std::unordered_map<std::string, std::vector<GameObject*>> m_Container;
	int m_nCurrItem;
public:
	GameObjectManager();
	~GameObjectManager();
public:
	void Update();
	void Render();
public:
	void ShowMenuBar();
	void ShowGameObjectList();
public:
	void AddGameObject(std::string _sMeshID, PxTriangleMesh* _pTriangleMesh);
	//
	void AddGameObject(GameObject* _pGameObject);
	void DeleteGameObject();

public:
	void Save();
	void Load();
};

#endif // !__GAMEOBJECT_MANAGER_H__