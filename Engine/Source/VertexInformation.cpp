#include "VertexInformation.h"

USING(ENGINE)

VertexInformation::VertexInformation()
	: m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
	, m_pVertexDecl(nullptr)
{
}

void VertexInformation::Free()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer)
}
