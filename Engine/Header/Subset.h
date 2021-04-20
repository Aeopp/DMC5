#ifndef __SUBSET_H__
#define __SUBSET_H__
#include "Object.h"
BEGIN(ENGINE)
class ENGINE_DLL Subset final : public Object
{
public:
	LPDIRECT3DDEVICE9		m_pDevice;

	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	m_pIndexBuffer;

	VERTEXBUFFERDESC		m_tVertexBufferDesc;

	MATERIAL				m_tMaterial;
private:
	explicit Subset(LPDIRECT3DDEVICE9 const _pDevice);
	// Object��(��) ���� ��ӵ�
	virtual ~Subset() = default;
	virtual void Free() override;
public:
	static Subset* Create(LPDIRECT3DDEVICE9 const _pDevice);
	virtual void Editor()override;
	virtual std::string GetName()override;
public:
	bool bRender = true;
public:
	void Initialize(LPDIRECT3DVERTEXBUFFER9 const _pVB, LPDIRECT3DINDEXBUFFER9 const _pIB, const VERTEXBUFFERDESC & _tVBDesc, const MATERIAL & _tMaterial);
	void Render(ID3DXEffect *const Fx);
	// �ؽ��ĸ� ����Ʈ�� ���ε�
	void BindProperty(const UINT TexType, const uint64 TexIdx, const std::string & ShaderParamName , ID3DXEffect* const Fx)&;
	// �ؽ��ĸ� ��ġ �������Ϳ� ���ε� . 
	void BindProperty(const UINT TexType, const uint64 TexIdx,
		const UINT RegisterIdx, IDirect3DDevice9* const _Device)&;
public:
	const LPDIRECT3DVERTEXBUFFER9	GetVertexBuffer();
	const LPDIRECT3DINDEXBUFFER9	GetIndexBuffer();
	const VERTEXBUFFERDESC& GetVertexBufferDesc();
	const MATERIAL&			GetMaterial();
};
END
#endif // !__SUBSET_H__