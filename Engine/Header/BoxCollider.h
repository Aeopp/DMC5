#ifndef __BOX_COLLIDER_H__
#define __BOX_COLLIDER_H__
#include "Collider.h"
BEGIN(ENGINE)
class ENGINE_DLL BoxCollider : public Collider
{
private:
	static LPD3DXMESH	m_pMesh;
	D3DXVECTOR3 m_vSize;
private:
	explicit BoxCollider(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~BoxCollider() = default;
	// Collider을(를) 통해 상속됨
	virtual void Free() override;
public:
	static BoxCollider* Create(std::weak_ptr<GameObject> const _pGameObject);
public:
	virtual HRESULT ReadyCollider()						override;
	virtual	HRESULT DrawCollider(LPD3DXEFFECT _pEffect)	override;
	virtual void	Editor()							override;
public:
	D3DXVECTOR3	GetSize();
	void		SetSize(const D3DXVECTOR3 _vSize);
};
END
#endif // !__BOX_COLLIDER_H__