#ifndef GT_Overture_h__
#define GT_Overture_h__

#include "Gauntlet.h"

class GT_Overture : public Gauntlet
{
private:
	explicit GT_Overture();
	virtual ~GT_Overture() = default;

	// Gauntlet��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static GT_Overture* Create();
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
	virtual std::string GetName() override;
	virtual void Editor()override;
public:
	//�浹
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
	virtual void OnTriggerExit(std::weak_ptr<GameObject> _pOther) override;
public:
	// RenderInterface��(��) ���� ��ӵ�
	virtual void RenderReady() override;
	// ������ �Լ� ...
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

private:
	std::weak_ptr<SphereCollider> m_pCollider;
};

#endif // GT_Overture_h__
