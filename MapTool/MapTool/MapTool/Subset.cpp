#include "framework.h"
#include "Subset.h"
#include "Texture.h"

Subset::Subset()
	: m_pVB(nullptr)
	, m_pIB(nullptr)
{
}

Subset::~Subset()
{
	SafeRelease(m_pVB);
	SafeRelease(m_pIB);
	SafeRelease(m_tVBDesc.pVertexDecl);


	for (auto& rPair : m_tMaterial.Textures)
	{
		for (auto& pTexture : rPair.second)
		{
			delete pTexture;
			pTexture = nullptr;
		}
		rPair.second.clear();
		rPair.second.shrink_to_fit();
	}
	m_tMaterial.Textures.clear();
	
}

void Subset::Render()
{
	g_pDevice->SetVertexDeclaration(m_tVBDesc.pVertexDecl);
	g_pDevice->SetStreamSource(0, m_pVB, 0, m_tVBDesc.nStride);
	g_pDevice->SetIndices(m_pIB);
	if (0 != m_tMaterial.Textures[1].size() && nullptr != m_tMaterial.Textures[1][0])
		g_pDevice->SetTexture(0, m_tMaterial.Textures[1][0]->m_pTexture);
	else
		g_pDevice->SetTexture(0, nullptr);

	g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_tVBDesc.nNumVertices, 0, m_tVBDesc.nNumFaces);
}
