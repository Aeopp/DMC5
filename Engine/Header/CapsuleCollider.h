#ifndef __CAPSULE_COLLIDER_H__
#define __CAPSULE_COLLIDER_H__
#include "Collider.h"
BEGIN(ENGINE)
class ENGINE_DLL CapsuleCollider final: public Collider
{
private:
	static	LPD3DXMESH		m_pMesh;
	static	LPD3DXBUFFER	m_pAdjacency;
	static	LPD3DXBUFFER	m_pSubset;
	static	DWORD			m_nNumSubset;

	float		m_fRadius;
	float		m_fHeight;
private:
	explicit CapsuleCollider(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~CapsuleCollider() = default;
	// Collider을(를) 통해 상속됨
	virtual void Free() override;
public:
	static CapsuleCollider* Create(std::weak_ptr<GameObject> const _pGameObject);
public:
	virtual HRESULT ReadyCollider()						override;
	virtual	HRESULT DrawCollider(const DrawInfo & _Info)	override;
	virtual void	Editor()							override;
public:
	float	GetRadius();
	void	SetRadius(const float _fRadius);

	float	GetHeight();
	void	SetHeight(const float _fHeight);
};
END
#endif // !__CAPSULE_COLLIDER_H__
