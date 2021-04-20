#include "CollisionCallback.h"
#include "Collider.h"
#include "GameObject.h"
#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif // _DEBUG

USING(ENGINE)

void CollisionCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
	PX_UNUSED(constraints);
	PX_UNUSED(count);
}

void CollisionCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	PX_UNUSED(actors);
	PX_UNUSED(count);
}

void CollisionCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
	PX_UNUSED(actors);
	PX_UNUSED(count);
}

void CollisionCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	LPPXUSERDATA pUserData[2] = {};
	std::weak_ptr<Collider> pCollider[2] = {};
	for (UINT i = 0; i < nbPairs; ++i)
	{
		pUserData[0] = (LPPXUSERDATA)pairHeader.actors[0]->userData;
		pUserData[1] = (LPPXUSERDATA)pairHeader.actors[1]->userData;

		//Collider 컴포넌트가 해제되어 pUserData가 nullptr인 경우의 예외처리.
		if (nullptr == pUserData[0] || nullptr == pUserData[1])
			continue;

		pCollider[0] = pUserData[0]->pCollider;
		pCollider[1] = pUserData[1]->pCollider;

		if (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND == pairs[i].events)
		{
			if(false == pCollider[0].expired())
				pCollider[0].lock()->GetGameObject().lock()->OnCollisionEnter(pCollider[1].lock()->GetGameObject());
			if (false == pCollider[1].expired())
				pCollider[1].lock()->GetGameObject().lock()->OnCollisionEnter(pCollider[0].lock()->GetGameObject());
		}
		else if (physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS == pairs[i].events)
		{
			if (false == pCollider[0].expired())
				pCollider[0].lock()->GetGameObject().lock()->OnCollisionStay(pCollider[1].lock()->GetGameObject());
			if (false == pCollider[1].expired())
				pCollider[1].lock()->GetGameObject().lock()->OnCollisionStay(pCollider[0].lock()->GetGameObject());
		}
		else if (physx::PxPairFlag::eNOTIFY_TOUCH_LOST == pairs[i].events)
		{
			if (false == pCollider[0].expired())
				pCollider[0].lock()->GetGameObject().lock()->OnCollisionExit(pCollider[1].lock()->GetGameObject());
			if (false == pCollider[1].expired())
				pCollider[1].lock()->GetGameObject().lock()->OnCollisionExit(pCollider[0].lock()->GetGameObject());
		}
	}
}

void CollisionCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	LPPXUSERDATA pUserData[2] = {};
	std::weak_ptr<Collider> pCollider[2] = {};

	for (UINT i = 0; i < count; ++i)
	{
		pUserData[0] = (LPPXUSERDATA)pairs->otherActor->userData;
		pUserData[1] = (LPPXUSERDATA)pairs->triggerActor->userData;

		//Collider 컴포넌트가 해제되어 pUserData가 nullptr인 경우의 예외처리.
		if (nullptr == pUserData[0] || nullptr == pUserData[1])
			continue;

		pCollider[0] = pUserData[0]->pCollider;
		pCollider[1] = pUserData[1]->pCollider;

		if (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND == pairs[i].status)
		{
			if (false == pCollider[0].expired())
				pCollider[0].lock()->GetGameObject().lock()->OnTriggerEnter(pCollider[1].lock()->GetGameObject());
			if (false == pCollider[1].expired())
				pCollider[1].lock()->GetGameObject().lock()->OnTriggerEnter(pCollider[0].lock()->GetGameObject());
		}
		else if (physx::PxPairFlag::eNOTIFY_TOUCH_LOST == pairs[i].status)
		{
			if (false == pCollider[0].expired())
				pCollider[0].lock()->GetGameObject().lock()->OnTriggerExit(pCollider[1].lock()->GetGameObject());
			if (false == pCollider[1].expired())
				pCollider[1].lock()->GetGameObject().lock()->OnTriggerExit(pCollider[0].lock()->GetGameObject());
		}
	}
}

void CollisionCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
}
