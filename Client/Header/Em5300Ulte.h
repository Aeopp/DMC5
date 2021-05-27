#ifndef Em5300Ulte_h__
#define Em5300Ulte_h__

#pragma once
#include "Monster.h"

class Em5300;
class Em5300Ulte final : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Em5300Ulte() = default;
	virtual ~Em5300Ulte() = default;
	// Monster을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Em5300Ulte* Create();
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

	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);

public:
	void Ulte(const float _fDeltaTime);
	void Set_Ulte(bool _fUlte) { m_bUlteStart = _fUlte; }
	bool Get_UlteEnd() { return m_bUlteEnd; }
	float Get_UlteSize() { return m_fUlteSize; }

public:
	std::shared_ptr<ENGINE::StaticMesh>   m_pMesh;

	weak_ptr<Em5300>					m_pEm5300;
	weak_ptr<ENGINE::SkeletonMesh>	    m_pEm5300Mesh;
	weak_ptr<ENGINE::Transform>			m_pEm5300Trans;

	optional<Matrix>					  m_ParentBone;
	Matrix*								  m_pParentBone;
	Matrix								  m_ParentWorld;
	Matrix								  m_Result;

	weak_ptr<SphereCollider>			m_pCollider;
	weak_ptr<class NuClear>					m_pNuclear;

	bool	m_bUlteStart = false;
	bool    m_bUlte = false;

	float	m_fUlteSize = 0.f;

	//이거 가지고 터트리면 됨 다 떨어지면 True
	bool	m_bUlteEnd = false;

	bool	m_bOne[2] = { false, false };
};

#endif // Em5300Ulte_h__
