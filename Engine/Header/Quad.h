#ifndef _QUAD_H_
#define _QUAD_H_
#include "EngineTypedef.h"
#include "EngineDefine.h"

BEGIN(ENGINE)
struct ENGINE_DLL Quad
{
	void Initialize(LPDIRECT3DDEVICE9 _Device)&;
	
	void Render(LPDIRECT3DDEVICE9 _Device,
				/*사이즈는 클라이언트 창 사이즈에 대한 비율 .. */
				const float SizeXRatio=1.f,
				const float SizeYRatio = 1.f,
				LPD3DXEFFECT const Fx=nullptr)&;
	void Release()&;

	~Quad()noexcept { Release();  };

	LPDIRECT3DVERTEXBUFFER9 VtxBuf{ nullptr };
	LPDIRECT3DINDEXBUFFER9 IdxBuf{ nullptr };
	LPDIRECT3DVERTEXDECLARATION9 VtxDecl{ nullptr };
	uint64 Stride{ 0u };
};
END


#endif // !


