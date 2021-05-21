#ifndef Em5000Hand_h__
#define Em5000Hand_h__

#pragma once
#include "Monster.h"

class Em5000;
class Em5000Hand final : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Em5000Hand() = default;
	virtual ~Em5000Hand() = default;
	// Monster을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Em5000Hand* Create();
public:
	virtual std::string GetName() override;
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void Editor() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
	virtual void RenderReady() override;
public:
	void RenderInit();



public:
	weak_ptr<Em5000>					m_pEm5000;
	weak_ptr<ENGINE::SkeletonMesh>	    m_pEm5000Mesh;
	weak_ptr<ENGINE::Transform>			m_pEm5000Trans;

	optional<Matrix>					  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;
		
	bool								  m_bLeft;

	weak_ptr<SphereCollider>			m_pCollider;
	weak_ptr<class ShockWave>			m_pWave;
	weak_ptr<class Smoke>				m_pSmoke[2];
};

#endif // Em5000Hand_h__
