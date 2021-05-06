#ifndef WingSword4th_h__
#define WingSword4th_h__

#include "WingSword.h"
class WingSword4th :
    public WingSword
{
private:
	explicit WingSword4th();
	virtual ~WingSword4th() = default;

	// Gauntlet��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static WingSword4th* Create();
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



#endif // WingSword4th_h__