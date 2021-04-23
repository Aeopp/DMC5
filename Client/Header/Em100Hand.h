#ifndef Em100Hand_h__
#define Em100Hand_h__

#pragma once
#include "Monster.h"

class Em100;
class Em100Hand final : public Unit,
						public ENGINE::RenderInterface
{
private:
	explicit Em100Hand() = default;
	virtual ~Em100Hand() = default;
	// Monster을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Em100Hand* Create();
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
	weak_ptr<Em100>						  m_pEm100;
	weak_ptr<ENGINE::SkeletonMesh>		  m_pEm100Mesh;
	weak_ptr<ENGINE::Transform>			  m_pEm100Trans;

	optional<Matrix>					  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;
		
	bool								  m_bLeft;
	 
	weak_ptr<SphereCollider>		 	  m_pCollider;

	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
};

#endif // Em100Hand_h__
