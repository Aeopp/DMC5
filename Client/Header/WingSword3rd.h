#ifndef WingSword3rd_h__
#define WingSword3rd_h__

#include "WingSword.h"
class WingSword3rd :
    public WingSword
{
private:
	explicit WingSword3rd();
	virtual ~WingSword3rd() = default;

	// Gauntlet을(를) 통해 상속됨
	virtual void Free() override;
public:
	static WingSword3rd* Create();
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
	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
	// 렌더링 함수 ...
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
};



#endif // WingSword3rd_h__