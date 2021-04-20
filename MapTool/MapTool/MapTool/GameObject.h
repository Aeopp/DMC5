#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
class Transform;
#include "Physics.h"
class GameObject
{
public:
	std::string m_sMesh;
	Transform* m_pTransform;

	PxRigidActor* m_pRigidActor;
	PxShape* m_pShape;
	PxTriangleMeshGeometry m_Geometry;
	
	int m_nIndex;

	int m_nListIdx;
public:
	GameObject();
	~GameObject();

public:
	void CreateRigidActor(PxTriangleMesh* _pTriangleMesh);

	void Update();
};

#endif // !__GAMEOBJECT_H__
