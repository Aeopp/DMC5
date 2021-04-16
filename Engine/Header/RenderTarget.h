#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_
#include <vector>
#include "EngineDefine.h"
#include "EngineTypedef.h"
BEGIN(ENGINE)
class ENGINE_DLL RenderTarget
{
public:
	struct Info
	{
		uint32 Width;
		uint32 Height;
		uint32 Levels;
		long Usages;
		D3DPOOL _D3DPool;
		D3DFORMAT Format;
	};
	void Initialize(const Info& InitInfo)&;
	void DebugBufferInitialize(
		const Vector2& ScreenPos, const Vector2& ScreenSize);
	void DebugRender(ID3DXEffect* const Fx);
	/*void Clear( const DWORD Count, 
				const DWORD Flags,const D3DCOLOR Color,
				const float Z,const DWORD Stencil)&;*/
	void Release()&;

	~RenderTarget()noexcept { Release();  };
	IDirect3DTexture9* GetTexture()
		const& { return TargetTexture;  };
	IDirect3DSurface9* GetSurface(
		const uint64 Index=0u)const&;
	void InsertSurface(const UINT Level)&;
	Info GetInfo()& { return _RenderTargetInfo;  };
private:
	std::vector<IDirect3DSurface9*> Surfaces{};
	IDirect3DTexture9* TargetTexture{ nullptr };
	Info _RenderTargetInfo{};
	IDirect3DVertexBuffer9* VtxBuf{ nullptr };
	IDirect3DIndexBuffer9* IdxBuf{ nullptr };
	IDirect3DVertexDeclaration9* DebugBufVtxDecl{ nullptr };
	Vector2 ScreenSize{ 0,0 };
	Vector2 ScreenPos{ 0,0 };
	uint32 Stride = 0u;
};
END

#endif // !_RENDERTARGET_H_

