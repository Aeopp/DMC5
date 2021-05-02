#ifndef Em200Hand_h__
#define Em200Hand_h__

#pragma once
#include "Monster.h"

class Em200;
class Em200Hand final : public Unit,
						public ENGINE::RenderInterface
{
private:
	explicit Em200Hand() = default;
	virtual ~Em200Hand() = default;
	// Monster을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Em200Hand* Create();
public:
	virtual std::string GetName() override;
public:
	virtual HRESULT Ready()								override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual UINT	Update(const float _fDeltaTime)		override;
	virtual UINT	LateUpdate(const float _fDeltaTime) override;
	virtual void	Editor()							override;
	virtual void	OnEnable()							override;
	virtual void	OnDisable()							override;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
public:
	void RenderInit();
public:
	weak_ptr<Em200>						  m_pEm200;
	weak_ptr<ENGINE::SkeletonMesh>		  m_pEm200Mesh;
	weak_ptr<ENGINE::Transform>			  m_pEm200Trans;

	optional<Matrix>					  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;
		
	bool								  m_bLeft;
	 
	weak_ptr<SphereCollider>		 	  m_pCollider;

	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
};

#endif // Em200Hand_h__
