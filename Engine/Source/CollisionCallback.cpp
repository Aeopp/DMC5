#include "CollisionCallback.h"
#include "Collider.h"
#include "GameObject.h"
#include "PhysicsSystem.h"

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

		PxContactPairPoint* contactPointBuffer = new PxContactPairPoint[16];

		UINT nCount = pairs[i].extractContacts(contactPointBuffer, 16);
		if (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND == pairs[i].events || physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS == pairs[i].events)
		{
			if (((LPPXUSERDATA)(pairs[i].shapes[0]->getActor()->userData))->bIsGround == false)
			{
				for (UINT i = 0; i < nCount; ++i)
				{
					if (contactPointBuffer[i].normal.y >= 0.9f)
					{
						((LPPXUSERDATA)(pairs[i].shapes[0]->getActor()->userData))->bIsGround = true;
						break;
					}
				}
			}
		}
		else
		{
			if (((LPPXUSERDATA)(pairs[i].shapes[0]->getActor()->userData))->bIsGround == true)
			{
				if(0 == nCount)
					((LPPXUSERDATA)(pairs[i].shapes[0]->getActor()->userData))->bIsGround = false;

				for (UINT i = 0; i < nCount; ++i)
				{
					if (contactPointBuffer[i].normal.y >= 0.9f)
					{
						((LPPXUSERDATA)(pairs[i].shapes[0]->getActor()->userData))->bIsGround = false;
						break;
					}
				}
			}
		}

		delete[] contactPointBuffer;

		pCollider[0] = pUserData[0]->pCollider;
		pCollider[1] = pUserData[1]->pCollider;
		
		PhysicsSystem::COLLISIONPAIR tCollisionPair;

		tCollisionPair.bTrigger = false;


		if (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND == pairs[i].events)
		{
			if (false == pCollider[0].expired() && false == pCollider[1].expired())
			{
				tCollisionPair.ePairFlag = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
				tCollisionPair.pFirst = pCollider[0].lock()->GetGameObject();
				tCollisionPair.pSecond = pCollider[1].lock()->GetGameObject();
				PhysicsSystem::GetInstance()->AddCallbackPair(tCollisionPair);
			}

		}
		else if (physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS == pairs[i].events)
		{
			if (false == pCollider[0].expired() && false == pCollider[1].expired())
			{
				tCollisionPair.ePairFlag = physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
				tCollisionPair.pFirst = pCollider[0].lock()->GetGameObject();
				tCollisionPair.pSecond = pCollider[1].lock()->GetGameObject();
				PhysicsSystem::GetInstance()->AddCallbackPair(tCollisionPair);
			}
		}
		else if (physx::PxPairFlag::eNOTIFY_TOUCH_LOST == pairs[i].events)
		{
			if (false == pCollider[0].expired() && false == pCollider[1].expired())
			{
				tCollisionPair.ePairFlag = physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
				tCollisionPair.pFirst = pCollider[0].lock()->GetGameObject();
				tCollisionPair.pSecond = pCollider[1].lock()->GetGameObject();
				PhysicsSystem::GetInstance()->AddCallbackPair(tCollisionPair);
			}
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

		PhysicsSystem::COLLISIONPAIR tCollisionPair;

		tCollisionPair.bTrigger = true;

		if (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND == pairs[i].status)
		{
			if (false == pCollider[0].expired() && false == pCollider[1].expired())
			{
				tCollisionPair.ePairFlag = physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
				tCollisionPair.pFirst = pCollider[0].lock()->GetGameObject();
				tCollisionPair.pSecond = pCollider[1].lock()->GetGameObject();
				PhysicsSystem::GetInstance()->AddCallbackPair(tCollisionPair);
			}
		}
		else if (physx::PxPairFlag::eNOTIFY_TOUCH_LOST == pairs[i].status)
		{
			if (false == pCollider[0].expired() && false == pCollider[1].expired())
			{
				tCollisionPair.ePairFlag = physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
				tCollisionPair.pFirst = pCollider[0].lock()->GetGameObject();
				tCollisionPair.pSecond = pCollider[1].lock()->GetGameObject();
				PhysicsSystem::GetInstance()->AddCallbackPair(tCollisionPair);
			}
		}
	}
}

void CollisionCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
}
