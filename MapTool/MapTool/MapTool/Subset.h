#ifndef __SUBSET_H__
#define __SUBSET_H__
class Subset
{
public:
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	VERTEXBUFFERDESC		m_tVBDesc;

	MATERIAL				m_tMaterial;
public:
	Subset();
	~Subset();
public:
	void Render();
};
#endif // !__SUBSET_H__