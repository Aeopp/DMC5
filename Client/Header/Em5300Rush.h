#ifndef Em5300Rush_h__
#define Em5300Rush_h__

#pragma once
#include "Monster.h"

class Em5300;
class Em5300Rush final : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Em5300Rush() = default;
	virtual ~Em5300Rush() = default;
	// Monster을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Em5300Rush* Create();
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
	virtual void RenderReady() override;
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
public:
	void RenderInit();



public:
	weak_ptr<Em5300>					m_pEm5300;
	weak_ptr<ENGINE::SkeletonMesh>	    m_pEm5300Mesh;
	weak_ptr<ENGINE::Transform>			m_pEm5300Trans;

	optional<Matrix>					  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;

	weak_ptr<SphereCollider>			m_pCollider;
};

#endif // Em5300Rush_h__
