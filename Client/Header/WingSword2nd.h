#ifndef WingSword2nd_h__
#define WingSword2nd_h__

#include "WingSword.h"
class WingSword2nd :
    public WingSword
{
private:
	explicit WingSword2nd();
	virtual ~WingSword2nd() = default;

	// Gauntlet��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static WingSword2nd* Create();
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
	// RenderInterface��(��) ���� ��ӵ�
	virtual void RenderReady() override;
	// ������ �Լ� ...
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
};



#endif // WingSword2nd_h__