#ifndef __BreakableObject__H_
#define __BreakableObject__H_
#include "GameObject.h"
#include "RenderInterface.h"

class BreakableObject : public ENGINE::GameObject ,
				   public ENGINE::RenderInterface				
{
private:
	explicit BreakableObject() = default;
	virtual ~BreakableObject() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static BreakableObject* Create();
public:
	virtual void    RenderReady()                          override;
public:
	void RenderInit();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;

public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);


	void SetUp(const TSTRING _sMesh, const D3DXVECTOR3& _vScale, const D3DXVECTOR3& _vRotation, const D3DXVECTOR3 _vPosition);
	void StoneInit();
	void StoneDebrisPlayStart();

private:
	std::shared_ptr<ENGINE::StaticMesh> m_pStaticMesh;
	std::weak_ptr<CapsuleCollider> m_pCollider;

	vector<weak_ptr<class StoneDebris>>	m_pStoneDebrisVec;
	bool	m_bStoneDebrisPlayStart = false;
	int		m_iStoneCount = 0;


	float	m_fStoneTime = 0.f;
	bool	m_bDestroyObject = false;

};
#endif //__BreakableObject__H_