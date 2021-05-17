#ifndef __CollObject__H_
#define __CollObject__H_
#include "GameObject.h"
#include "RenderInterface.h"

class CollObject : public ENGINE::GameObject ,
				   public ENGINE::RenderInterface				
{
private:
	explicit CollObject() = default;
	virtual ~CollObject() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static CollObject* Create();
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
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);


	void SetUp(const TSTRING _sMesh, const D3DXVECTOR3& _vScale, const D3DXVECTOR3& _vRotation, const D3DXVECTOR3 _vPosition);

private:
	std::shared_ptr<ENGINE::StaticMesh> m_pStaticMesh;
	std::weak_ptr<MeshCollider> m_pCollider;
};
#endif //__CollObject__H_