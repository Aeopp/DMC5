#ifndef __SPHERE_COLLIDER_H__
#define __SPHERE_COLLIDER_H__
#include "Collider.h"
BEGIN(ENGINE)
class ENGINE_DLL SphereCollider : public Collider
{
private:
	static	LPD3DXMESH m_pMesh;
	float		m_fRadius;
private:
	explicit SphereCollider(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~SphereCollider() = default;
	// Collider을(를) 통해 상속됨
	virtual void Free() override;
public:
	static SphereCollider* Create(std::weak_ptr<GameObject> const _pGameObject);
public:
	virtual HRESULT ReadyCollider()	override;
	virtual	HRESULT DrawCollider(LPD3DXEFFECT _pEffect)	override;
public:
	float		GetRadius();
	void		SetRadius(const float _fRadius);
};
END
#endif // !__SPHERE_COLLIDER_H__
