#ifndef __PHYSICS_H__
#define __PHYSICS_H__
#include "PxPhysicsAPI.h"
using namespace physx;
class GameObject;
class Physics
{
public:
	static Physics Instance;
public:
	PxFoundation*			m_pFoundation;
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;
	PxPhysics*				m_pPhysics;
	PxDefaultCpuDispatcher* m_pDispatcher;
	PxMaterial*				m_pDefaultMaterial;

	//PhyX Visual Debugger
	PxPvd* m_pPVD;
	//
	PxScene* m_pScene;

	PxCooking* m_pCooking;
public:
	Physics();
	~Physics();
public:
	static HRESULT Ready();

public:
	static GameObject* RayCast();
};

#endif // !__PHYSICS_H__
