#ifndef Revelion_h__
#define Revelion_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class Revelion : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Revelion();
	virtual ~Revelion() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Revelion* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther);
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual std::string GetName() override;
public:
	void	SetWeaponState(UINT _StateIndex) { m_iStateIndex = _StateIndex; }
	void	SetAttType(ATTACKTYPE _eAttDir) { m_BattleInfo.eAttackType = _eAttDir; }
public:
	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
	// 렌더링 함수 ...
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

	virtual void Editor()override;

	Matrix* Get_BoneMatrixPtr(std::string _BoneName);
	Vector3	GetRevelionBoneWorldPos(std::string _BoneName);

private:
	std::weak_ptr<Nero>					  m_pNero;
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;

	std::vector<Matrix*>			m_vecParentMat;
	Matrix*							m_pMyBoneMat;
	UINT							m_iStateIndex = 0;
};



#endif // Revelion_h__