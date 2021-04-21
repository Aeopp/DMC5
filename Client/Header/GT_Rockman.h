#ifndef GT_Rockman_h__
#define GT_Rockman_h__

#include "Gauntlet.h"
class GT_Rockman :
    public Gauntlet
{
private:
    explicit GT_Rockman();
    virtual ~GT_Rockman() = default;
    // Gauntlet을(를) 통해 상속됨
    virtual void Free() override;
public:
    static GT_Rockman* Create();
    virtual HRESULT Ready() override;
    virtual HRESULT Awake() override;
    virtual HRESULT Start() override;
    virtual UINT Update(const float _fDeltaTime) override;
    virtual UINT LateUpdate(const float _fDeltaTime) override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	virtual std::string GetName() override;
	virtual void Editor()override;
    virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
    virtual void OnTriggerExit(std::weak_ptr<GameObject> _pOther) override;
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



#endif // GT_Rockman_h__