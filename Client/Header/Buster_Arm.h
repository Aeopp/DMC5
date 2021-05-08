#ifndef Buster_Arm_h__
#define Buster_Arm_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class Buster_Arm : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Buster_Arm();
	virtual ~Buster_Arm() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Buster_Arm* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
	// ������ �Լ� ... 
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
public:
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
public:
	virtual std::string GetName() override;
	float Get_PlayingTime();
	float Get_PlayingAccTime();
public:
	// RenderInterface��(��) ���� ��ӵ�
	virtual void RenderReady() override;

	virtual void Editor()override;

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<Nero>					  m_pNero;
	std::weak_ptr<SphereCollider> m_pCollider;

	bool								  m_bIsRender;
	bool								  m_bHitOnce_Em5000 = true;

	std::shared_ptr<ENGINE::Texture>		m_NRMRTex{};
	std::shared_ptr<ENGINE::Texture>		m_ATOSTex{};
	std::shared_ptr<ENGINE::Texture>		m_GradationTex{};

	float	m_fAccTime = 0.f;

};



#endif // Buster_Arm_h__