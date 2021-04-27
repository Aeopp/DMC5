#ifndef Wire_Arm_h__
#define Wire_Arm_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class Wire_Arm_Grab;
class Wire_Arm : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Wire_Arm();
	virtual ~Wire_Arm() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Wire_Arm* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	//�浹
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
	virtual void OnTriggerExit(std::weak_ptr<GameObject> _pOther) override;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
public:
	virtual std::string GetName() override;
	float Get_PlayingTime();
	float Get_PlayingAccTime();
public:
	void Set_RadianForRotX(float _fRadian) { m_fRadianForRotX = _fRadian; }
public:
	// RenderInterface��(��) ���� ��ӵ�
	virtual void RenderReady() override;
	virtual void Editor()override;

	// ������ �Լ� ..
	void RenderInit();
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<Nero>					  m_pNero;
	std::weak_ptr<Wire_Arm_Grab>		  m_pWireArmGrab;
	std::weak_ptr<SphereCollider> m_pCollider;

	bool								m_bIsRender;
	Matrix*								m_pBoneMatrix;
	Matrix								m_MyRenderMatrix;
	float								m_fRadianForRotX;
};



#endif // Wire_Arm_h__