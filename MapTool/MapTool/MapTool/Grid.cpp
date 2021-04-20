#include "framework.h"
#include "Grid.h"

Grid Grid::Instance;
DWORD Grid::VERTEX::dwFVF = D3DFVF_XYZ;

Grid::Grid()
	: m_nCX(2)
	, m_nCZ(2)
	, m_nX(101)
	, m_nZ(101)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
{
	m_nFace = (m_nX - 1) * (m_nZ - 1) * 2;
}

Grid::~Grid()
{
	SafeRelease(m_pVB);
	SafeRelease(m_pIB);
}

HRESULT Grid::Ready()
{
	return CreateBuffer();
}

HRESULT Grid::CreateBuffer()
{
	g_pDevice->CreateVertexBuffer(Instance.m_nX * Instance.m_nZ * sizeof(VERTEX), D3DUSAGE_WRITEONLY, Grid::VERTEX::dwFVF, D3DPOOL_MANAGED, &Instance.m_pVB, nullptr);

	LPVERTEX pVertices = nullptr;

	Instance.m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	UINT nIndex = 0;
	float fWidth = (float)(Instance.m_nCX * (Instance.m_nX - 1));
	float fHeight = (float)(Instance.m_nCZ * (Instance.m_nZ - 1));
	for (UINT z = 0; z < Instance.m_nZ; ++z)
	{
		for (UINT x = 0; x < Instance.m_nX; ++x)
		{
			nIndex = x + (z * Instance.m_nX);
			pVertices[nIndex].vPosition.x = (x * Instance.m_nCX) - fWidth / 2.f;
			pVertices[nIndex].vPosition.y = 0;
			pVertices[nIndex].vPosition.z = (z * Instance.m_nCZ) - fWidth / 2.f;
		}
	}
	Instance.m_pVB->Unlock();

	g_pDevice->CreateIndexBuffer(Instance.m_nFace * sizeof(INDEX16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &Instance.m_pIB, nullptr);

	LPINDEX16 pIndices = nullptr;

	Instance.m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	/*
		1 3
		0 2
		013 / 032
	*/

	UINT nFace = 0;
	for (UINT z = 0; z < Instance.m_nZ - 1; ++z)
	{
		for (UINT x = 0; x < Instance.m_nX - 1; ++x)
		{
			nIndex = x + (z * Instance.m_nX);
			pIndices[nFace]._0 = nIndex;
			pIndices[nFace]._1 = nIndex + Instance.m_nX;
			pIndices[nFace++]._2 = nIndex + Instance.m_nX + 1;

			pIndices[nFace]._0 = nIndex;
			pIndices[nFace]._1 = nIndex + Instance.m_nX + 1;
			pIndices[nFace++]._2 = nIndex + 1;
		}
	}
	Instance.m_pIB->Unlock();
	return S_OK;
}

void Grid::Draw()
{
	g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	g_pDevice->SetStreamSource(0, Instance.m_pVB, 0, sizeof(VERTEX));
	g_pDevice->SetFVF(Grid::VERTEX::dwFVF);
	g_pDevice->SetIndices(Instance.m_pIB);
	D3DXMATRIX matI;
	D3DXMatrixIdentity(&matI);
	g_pDevice->SetTransform(D3DTS_WORLD, &matI);
	g_pDevice->SetTexture(0, nullptr);
	g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, Instance.m_nX * Instance.m_nZ, 0, Instance.m_nFace);
	g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}
