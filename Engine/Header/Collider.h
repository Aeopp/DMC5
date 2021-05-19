#ifndef __COLLIDER_H__
#define __COLLIDER_H__
#include "Component.h"
#include "PxPhysicsAPI.h"
#include "RenderProperty.h"
BEGIN(ENGINE)
USING(physx)
class ENGINE_DLL Collider abstract : public Component
{
protected:
	PxShape *		m_pShape;
	PxMaterial*		m_pMaterial;
	PxRigidActor*	m_pRigidActor;

	PXUSERDATA		m_UserData;

	bool			m_bRigid;
	bool			m_bTrigger;

	D3DXVECTOR3		m_vCenter;

	//Option
	bool			m_bLock[6];
	bool			m_bGravity;
	bool			m_bRayCastTarget;
protected:
	explicit Collider(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~Collider() = default;
	// Component��(��) ���� ��ӵ�
	virtual void Free() PURE;
protected:
	virtual HRESULT ReadyCollider()							PURE;
public:
	virtual	HRESULT DrawCollider(const DrawInfo & _Info)	PURE;
	virtual void	Editor()								override;
public:
	virtual void	SetActive(const bool _bActive)			override;
public:
	void ReadySimulate();
private:
	void CreateRigidActor();
public:
	//RigidBody
	bool IsRigid();
	void SetRigid(const bool _bRigid);
	//Trigger
	bool IsTrigger();
	void SetTrigger(const bool _bTrigger);
	//Center
	D3DXVECTOR3 GetCenter();
	void		SetCenter(const D3DXVECTOR3 & _vCenter);

	//Lock Axis (RigidDynamic)
	bool		IsLock(const PxRigidDynamicLockFlag::Enum _eFlag);
	void		SetLockFlag(const PxRigidDynamicLockFlag::Enum _eFlag, const bool _bLock);

	bool		IsGravity();
	void		SetGravity(const bool _bActive);

	bool		IsRayCastTarget();
	void		SetRayCastTarget(const bool _bRayCastTarget);

	bool		IsGround();

	void		AddForce(const D3DXVECTOR3 _vForce);

	void		SetLinearVelocity(const D3DXVECTOR3 _vLinearVelocity = D3DXVECTOR3(0.f, 0.f, 0.f));
	float		GetContactOffset();

	void		SetContactOffset(const float _fOffset);
};
END
#endif // !__COLLIDER_H__