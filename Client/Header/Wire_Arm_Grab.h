#ifndef Wire_Arm_Grab_h__
#define Wire_Arm_Grab_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class Monster;
class Wire_Arm_Grab : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Wire_Arm_Grab();
	virtual ~Wire_Arm_Grab() = default;
	// Unit을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Wire_Arm_Grab* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	//충돌
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
	virtual void OnTriggerExit(std::weak_ptr<GameObject> _pOther) override;
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	void SetGrabedMonster(std::weak_ptr<GameObject> _GrabedMonster);
public:
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
public:
	virtual std::string GetName() override;
	float Get_PlayingTime();
	float Get_PlayingAccTime();

public:
	void Set_RadianForRotX(float _fRadian) { m_fRadianForRotX = _fRadian; }
	void Set_GrabEnd(bool _bGrabEnd) { m_bGrabEnd = _bGrabEnd; }
public:
	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
	virtual void Editor()override;

	// 렌더링 함수 ..
	void RenderInit();
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<Nero>					  m_pNero;
	std::weak_ptr<SphereCollider> m_pCollider;
	std::weak_ptr<Monster>		m_pGrabedMonster;

	bool								m_bIsRender;
	bool								m_bPlayOnce = false;
	bool								m_bGrabEnd = false;
	Vector3	m_vDir;

	float								m_fRadianForRotX;
};



#endif // Wire_Arm_Grab_h__