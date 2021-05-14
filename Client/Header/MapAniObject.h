#ifndef __MAP_ANI_OBJECT_H__
#define __MAP_ANI_OBJECT_H__
#include "GameObject.h"
#include "RenderInterface.h"
class MapAniObject final : public GameObject, public RenderInterface
{
private:
	explicit MapAniObject();
	virtual ~MapAniObject() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;


private:
	virtual std::string GetName() override;
public:
	static MapAniObject* Create();
public:
	virtual HRESULT Ready()								override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual UINT	Update(const float _fDeltaTime)		override;
	virtual UINT	LateUpdate(const float _fDeltaTime) override;
	virtual void	OnEnable()							override;
	virtual void	OnDisable()							override;
	virtual void    Editor()							override;

public:
	virtual void    RenderReady()                       override;
public:
	void RenderInit();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);
	void RenderEmissive(const DrawInfo& _Info);	
public:
	void SetUp(const TSTRING _sMesh, const D3DXVECTOR3& _vScale, const D3DXVECTOR3& _vRotation, const D3DXVECTOR3 _vPosition);
private:
	shared_ptr<SkeletonMesh>	m_pMesh;
	float EmissivePower = 0.05f;
	bool  bEmissive = false;
	bool  m_bFlag6 = false;
};
#endif // !__MAP_ANI_OBJECT_H__