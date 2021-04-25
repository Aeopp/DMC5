#ifndef __MAP_OBJECT_H__
#define __MAP_OBJECT_H__
#include "GameObject.h"
#include "RenderInterface.h"
class MapObject final : public GameObject, public RenderInterface
{
private:
	shared_ptr<StaticMesh>	m_pStaticMesh;
private:
	explicit MapObject();
	virtual ~MapObject() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static MapObject* Create();
public:
	virtual HRESULT Ready()								override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual UINT	Update(const float _fDeltaTime)		override;
	virtual UINT	LateUpdate(const float _fDeltaTime) override;
	virtual void	OnEnable()							override;
	virtual void	OnDisable()							override;

public:
	virtual void    RenderReady()                       override;
public:
	void RenderInit();
public:
	void RenderDebug(const DrawInfo& _Info);
	void RenderGBuffer(const DrawInfo& _Info);
	void RenderShadow(const DrawInfo& _Info);
	void RenderEmissive(const DrawInfo& _Info);

	float EmissivePower = 10.f;
	bool bEmissive = false;
public:
	void SetUp(const TSTRING _sMesh, const D3DXVECTOR3& _vScale, const D3DXVECTOR3& _vRotation, const D3DXVECTOR3 _vPosition);
};
#endif // !__MAP_OBJECT_H__