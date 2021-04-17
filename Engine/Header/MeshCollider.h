#ifndef __MESH_COLLIDER_H__
#define __MESH_COLLIDER_H__
#include "Collider.h"
BEGIN(ENGINE)
class ENGINE_DLL MeshCollider final : public Collider
{
private:
	PxTriangleMesh * m_pTriangleMesh;
private:
	explicit MeshCollider(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~MeshCollider() = default;
	// Collider을(를) 통해 상속됨
	virtual void Free() override;
public:
	static MeshCollider* Create(std::weak_ptr<GameObject> const _pGameObject);
public:
	virtual HRESULT ReadyCollider() override;
	virtual	HRESULT DrawCollider(const DrawInfo & _Info)	override;
public:
	void ReadyMeshCollider(D3DXVECTOR3 * _pPoints, UINT _nNumPoint, UINT * _pIndices, UINT _nNumFace);
public:
	void CookTriangleMesh(D3DXVECTOR3 * _pPoints, UINT _nNumPoint, UINT * _pIndices, UINT _nNumFace);
};
END
#endif // !__MESH_COLLIDER_H__