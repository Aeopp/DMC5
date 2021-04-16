#include "RenderTarget.h"
#include "GraphicSystem.h"
#include "Vertexs.h"
#include "FMath.hpp"
#include "Renderer.h"


USING(ENGINE)

void RenderTarget::Release ()&
{
	if (TargetTexture)
		TargetTexture->Release();
	if (VtxBuf)
		VtxBuf->Release();
	if (IdxBuf)
		IdxBuf->Release();
	if (DebugBufVtxDecl)
		DebugBufVtxDecl->Release();

	for (auto& DeleteSurface : Surfaces)
	{
		if (DeleteSurface)
		{
			DeleteSurface->Release();
		}
	}
}
void RenderTarget::Initialize(const Info& InitInfo)&
{
	auto _Device = GraphicSystem::GetInstance()->GetDevice();
	
	if (FAILED(_Device->CreateTexture(
		InitInfo.Width, 
		InitInfo.Height, 
		InitInfo.Levels, 
		InitInfo.Usages, 
		InitInfo.Format,
		InitInfo._D3DPool, &TargetTexture, NULL)))
	{
		PRINT_LOG(L"Failed!", L"Failed Render Target Texture !");
	}
	else
	{
		_RenderTargetInfo = InitInfo;
		Surfaces.resize(1);
		if (FAILED(TargetTexture->GetSurfaceLevel(0, &(Surfaces[0]))))
		{
			PRINT_LOG(L"Failed!", L"Failed Render Target Surfaces[0]!");
		}
	}
}
void RenderTarget::DebugBufferInitialize(const Vector2& ScreenPos, const Vector2& ScreenSize)
{
	this->ScreenSize = ScreenSize;
	this->ScreenPos = ScreenPos;
	Stride = sizeof(Vertex::Screen);


	g_pDevice->CreateVertexBuffer(
		Stride * 4u,
		D3DUSAGE_WRITEONLY, NULL,
		D3DPOOL_MANAGED,
		&VtxBuf, nullptr);

	g_pDevice->CreateIndexBuffer(sizeof(uint32) * 6u,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&IdxBuf, nullptr);

	DebugBufVtxDecl = Vertex::Screen::GetVertexDecl(g_pDevice);

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
};

void RenderTarget::DebugRender(ID3DXEffect* const Fx)
{
	const Matrix ScreenMatrix = 
		FMath::WorldMatrix
		({ ScreenSize.x,ScreenSize.y,0 }, { 0,0,0 }, { ScreenPos.x,ScreenPos.y,0 });

	Fx->SetTexture("RenderTargetTexture", TargetTexture);
	Fx->SetMatrix("Ortho", 
		&Renderer::GetInstance()->_RenderInfo.Ortho);
	Fx->SetMatrix("ScreenMatrix", &ScreenMatrix);
	Fx->CommitChanges();
	uint32 Pass = 0u;
	Fx->Begin(&Pass, NULL);
	for (uint32 i = 0; i < Pass; ++i)
	{
		Fx->BeginPass(i);
		{
			g_pDevice->SetStreamSource(0, VtxBuf, 0u, Stride);
			g_pDevice->SetVertexDeclaration(DebugBufVtxDecl);
			g_pDevice->SetIndices(IdxBuf);
			g_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4u, 0u, 2u);
		}
		Fx->EndPass();
	}
	Fx->End();

}
//void RenderTarget::Clear(
//	const DWORD Count ,
//	const DWORD Flags, const D3DCOLOR Color,
//	const float Z, const DWORD Stencil)&
//{
//
//};

IDirect3DSurface9* RenderTarget::GetSurface(const uint64 Index)const&
{
	return Surfaces[Index];
}
void RenderTarget::InsertSurface(const UINT Level)&
{
	IDirect3DSurface9* InsertSurface{ nullptr };

	if (FAILED(TargetTexture->GetSurfaceLevel(Level,  &InsertSurface)))
	{
		PRINT_LOG(L"Failed!", L"Failed Render Target Surface !");
	}
	else
	{
		Surfaces.push_back(InsertSurface);
	}
}
