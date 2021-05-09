#ifndef JudgementSword_h__
#define JudgementSword_h__

#include "GameObject.h"
class JudgementSword :
    public GameObject
{
private:
    // GameObject을(를) 통해 상속됨
    virtual void Free() override;
    virtual HRESULT Ready() override;
    virtual HRESULT Awake() override;
    virtual HRESULT Start() override;
    virtual UINT Update(const float _fDeltaTime) override;
    virtual UINT LateUpdate(const float _fDeltaTime) override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
};



#endif // JudgementSword_h__