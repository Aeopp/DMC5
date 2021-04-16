#ifndef __VERTEX_INFORMATION_H__
#define __VERTEX_INFORMATION_H__
#include "Object.h"
BEGIN(ENGINE)
class VertexInformation : public Object
{
public:
	LPDIRECT3DVERTEXBUFFER9			m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9			m_pIndexBuffer;
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDecl;
private:
	explicit VertexInformation();
	virtual ~VertexInformation() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
};
END
#endif // !__VERTEX_INFORMATION_H__