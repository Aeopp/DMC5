#ifndef __COLLIDER_H__
#define __COLLIDER_H__
#include "Component.h"
#include "PxPhysicsAPI.h"
BEGIN(ENGINE)
USING(physx)
class Collider abstract : public Component
{
protected:
	PxShape*		m_pShape;
	PxMaterial*		m_pMaterial;
	PxRigidActor*	m_pRigidActor;

	PXUSERDATA		m_UserData;

	bool			m_bRigid;
	bool			m_bTrigger;
protected:
	explicit Collider(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~Collider() = default;
	// Component을(를) 통해 상속됨
	virtual void Free() PURE;
protected:
	HRESULT ReadyCollider();
public:
	void ReadySimulate();
private:
	void CreateRigidActor();
public:
	bool IsRigid();
	void SetRigid(const bool _bRigid);

	bool IsTrigger();
	void SetTrigger(const bool _bTrigger);
};
END
#endif // !__COLLIDER_H__