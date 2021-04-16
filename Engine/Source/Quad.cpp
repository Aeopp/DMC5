#include "Quad.h"
#include "Vertexs.h"
#include "FMath.hpp"

USING(ENGINE)

void Quad::Initialize(LPDIRECT3DDEVICE9 _Device)&
{
	Stride = sizeof(Vertex::Screen);

	_Device->CreateVertexBuffer(
		Stride * 4u,
		D3DUSAGE_WRITEONLY, NULL,
		D3DPOOL_MANAGED,
		&VtxBuf, nullptr);

	_Device->CreateIndexBuffer(sizeof(uint32) * 6u,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&IdxBuf, nullptr);

	VtxDecl = Vertex::Screen::GetVertexDecl(_Device);

	Vertex::Screen* VtxBufPtr{ nullptr };

	VtxBuf->Lock(0, 0, reinterpret_cast<void**>(&VtxBufPtr), 0);

	VtxBufPtr[0].NDC = { -1.f ,+1.f,0.f };
	VtxBufPtr[0].UV2D = { 0.f,0.f };

	VtxBufPtr[1].NDC = { +1.f, +1.f,0.f };
	VtxBufPtr[1].UV2D = { 1.f,0.f };

	VtxBufPtr[2].NDC = { +1.f,-1.f,0.f };
	VtxBufPtr[2].UV2D = { 1.f,1.f };

	VtxBufPtr[3].NDC = { -1.f,-1.f,0.f };
	VtxBufPtr[3].UV2D = { 0.f,1.f };

	VtxBuf->Unlock();

	uint32* IdxBufPtr{ nullptr };
	IdxBuf->Lock(0u, 0u, reinterpret_cast<void**>(&IdxBufPtr), 0u);

	IdxBufPtr[0] = 0u;
	IdxBufPtr[1] = 1u;
	IdxBufPtr[2] = 2u;

	IdxBufPtr[3] = 0u;
	IdxBufPtr[4] = 2u;
	IdxBufPtr[5] = 3u;

	IdxBuf->Unlock();
}

void Quad::Render(
	LPDIRECT3DDEVICE9 _Device ,
	const float SizeXRatio, 
	const float SizeYRatio ,
	LPD3DXEFFECT const Fx)&
{
	if (Fx)
	{
		Fx->SetFloat("SizeXRatio", SizeXRatio);
		Fx->SetFloat("SizeYRatio", SizeYRatio);
	}
	
	{
		_Device ->SetStreamSource(0, VtxBuf, 0u, Stride);
		_Device ->SetVertexDeclaration(VtxDecl);
		_Device ->SetIndices(IdxBuf);
		_Device ->DrawIndexedPrimitive
			(D3DPT_TRIANGLELIST, 0, 0, 4u, 0u, 2u);
	}
}

void Quad::Release()&
{
	if (VtxBuf)
	{
		VtxBuf->Release();
	}

	if (IdxBuf)
	{
		IdxBuf->Release();
	}

	Stride = 0u;
}
