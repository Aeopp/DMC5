
#ifndef _VERTEXS_H_
#define _VERTEXS_H_
#include "EngineTypedef.h"
#include "EngineDefine.h"
BEGIN(Vertex)
struct Screen
{
	Vector3 NDC{ 0.f,0.f,0.f };
	Vector2 UV2D{ 0,0 };
	static IDirect3DVertexDeclaration9* GetVertexDecl(IDirect3DDevice9* const Device)
	{
		D3DVERTEXELEMENT9 Decl[] =
		{
			{ 0, 0,   D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }  ,
			D3DDECL_END()
		};
		IDirect3DVertexDeclaration9* VertexDeclaration{ nullptr };
		Device->CreateVertexDeclaration(Decl, &VertexDeclaration);
		return VertexDeclaration;
	};
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
};
END
#endif // !_VERTEXS_H_

