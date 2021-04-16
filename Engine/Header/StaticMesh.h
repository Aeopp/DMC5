#ifndef __STATIC_MESH_H__
#define __STATIC_MESH_H__
#include "Mesh.h"
#include "RenderInterface.h"
#include <optional>

BEGIN(ENGINE)
class ENGINE_DLL StaticMesh : public Mesh				
{
protected:
	explicit StaticMesh(LPDIRECT3DDEVICE9 const _pDevice);
	explicit StaticMesh(const StaticMesh& _rOther);
	virtual ~StaticMesh() = default;
	// Mesh을(를) 통해 상속됨
	virtual void Free() override;
public:
	static StaticMesh* Create(LPDIRECT3DDEVICE9 const _pDevice, 
								const std::filesystem::path _Path,
								const std::any& InitParams);
	// Mesh을(를) 통해 상속됨
	virtual Resource* Clone() override;
	virtual void	  Editor()override;
	virtual std::string GetName() override;
protected:
	virtual HRESULT LoadMeshImplementation(const aiScene* AiScene,
		const std::filesystem::path _Path,
		const std::any& InitParams)override;
};
END
#endif // !__STATIC_MESH_H__