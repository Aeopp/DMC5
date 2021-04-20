#ifndef __STRUCT_H__
#define __STRUCT_H__
typedef struct tagVertexBufferDesc
{
	UINT							nBufferSize;
	UINT							nStride;
	UINT							nNumFaces;
	UINT							nNumVertices;
	UINT							nNumUVChannel;
	UINT                          nNumBones;
	UINT							nMaxBonesRefPerVtx;

	std::vector<UINT>				vecNumUVComponents;

	bool									  bHasPosition;
	bool									  bHasNormal;
	bool									  bHasTangentBiNormal;
	bool									  bHasBone;
	std::shared_ptr<std::vector<D3DXVECTOR3>> LocalVertexLocation;
	LPDIRECT3DVERTEXDECLARATION9	          pVertexDecl;
public:
	tagVertexBufferDesc()
	{
		nMaxBonesRefPerVtx = nNumBones =
			nBufferSize = nStride = nNumFaces = nNumVertices = nNumUVChannel = 0u;
		pVertexDecl = nullptr;
		bHasBone = bHasPosition = bHasNormal = bHasTangentBiNormal = false;

	}
	~tagVertexBufferDesc()
	{
		SafeRelease(pVertexDecl);
	}
}VERTEXBUFFERDESC, * LPVERTEXBUFFERDESC;

typedef struct tagMaterial
{
public:
	typedef std::unordered_map<UINT, std::vector<class Texture*>> TEXTURES;
	TEXTURES		Textures;
	std::string		sName;
	D3DMATERIAL9	tMaterial;
public:
	tagMaterial()
	{
		sName.clear();
		ZeroMemory(&tMaterial, sizeof(D3DMATERIAL9));
	}

}MATERIAL, * LPMATERIAL;

typedef struct tagTextureDesc
{
	UINT	nUVIdx;
	float	fBlendFactor;
	UINT	eTextureOperator;
	UINT	eMappingMode;
	UINT	eMappingType;
}TEXTUREDESC, * LPTEXTUREDESC;
#endif // !__STRUCT_H__

