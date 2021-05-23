#ifndef SnatchPoint_h__
#define SnatchPoint_h__

#pragma once
#include "Monster.h"

class Em5300;
class SnatchPoint final : public Monster
{
private:
	explicit SnatchPoint() = default;
	virtual ~SnatchPoint() = default;
	// Monster을(를) 통해 상속됨
	virtual void Free() override;
public:
	static SnatchPoint* Create();
public:
	virtual std::string GetName() override;
	// Monster을(를) 통해 상속됨
	virtual void Fight(const float _fDeltaTime) override;
	virtual void State_Change(const float _fDeltaTime) override;
	virtual void Skill_CoolTime(const float _fDeltaTime) override;
	virtual void Buster(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void Snatch(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle() override;
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
	virtual void OnCollisionEnter(std::weak_ptr<GameObject> _pOther)override;
public:
	void RenderInit();

	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);

public:
	void Snatch(const float _fDeltaTime);
	void Set_SnatchPos(const int _iPos) { m_iSnatchPos = _iPos; }
	void Fly();
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

	int		m_iSnatchPos;
	Vector3 m_vStartPos;

	bool	m_bFly = false;

};

#endif // SnatchPoint_h__
