#ifndef __ENGINE_STRUCT_H__
#define __ENGINE_STRUCT_H__
#include <memory>
#include <string>
#include "TextureType.h"
#include "MathStruct.hpp"

BEGIN(ENGINE)
typedef struct tagVertexBufferDesc
{
	uint32							nBufferSize;
	uint32							nStride;
	uint32							nNumFaces;
	uint32							nNumVertices;
	uint32							nNumUVChannel;
	uint32                          nNumBones;
	uint32							nMaxBonesRefPerVtx;

	std::vector<uint32>				vecNumUVComponents;

	bool									  bHasPosition;
	bool									  bHasNormal;
	bool									  bHasTangentBiNormal;
	bool									  bHasBone;
	std::shared_ptr<std::vector<D3DXVECTOR3>> LocalVertexLocation;
	LPDIRECT3DVERTEXDECLARATION9	          pVertexDecl;
	Sphere BoundingSphere{};
public:
	tagVertexBufferDesc()
	{
		nMaxBonesRefPerVtx=nNumBones=
			nBufferSize = nStride = nNumFaces = nNumVertices = nNumUVChannel = 0u;
		pVertexDecl = nullptr;
		bHasBone = bHasPosition = bHasNormal = bHasTangentBiNormal = false;
		BoundingSphere.Radius = 0.0f;
		BoundingSphere.Center = Vector3{ 0  ,  0 , 0 };
	}
	~tagVertexBufferDesc()
	{
		SafeRelease(pVertexDecl);
	}
}VERTEXBUFFERDESC, *LPVERTEXBUFFERDESC;


typedef struct tagMaterial
{
public:
	typedef std::unordered_map<UINT, std::vector<std::shared_ptr<class Texture>>> TEXTURES;
	TEXTURES		Textures;
	std::string			sName;
	D3DMATERIAL9	tMaterial;
public:
	void Editor()
	{
		ImGui::BulletText("Name : %s", (sName).c_str());
	}

	tagMaterial()
	{
		sName.clear();
		ZeroMemory(&tMaterial, sizeof(D3DMATERIAL9));
	}
	const std::string& GetMaterialName() const
	{
		return sName;
	}
	std::shared_ptr<Texture> GetTexture(const UINT TextureType, const uint64 TextureIndex)const
	{
		if (auto TexTypeIter = Textures.find(TextureType);
			TexTypeIter!= std::end  ( Textures ))
		{
			auto& TexArray = TexTypeIter->second;
			if (TextureIndex < TexArray.size())
			{
				auto SpTexture= TexArray[TextureIndex];   
				if (SpTexture)
				{
					return SpTexture;
				}
			}
		}
		if (g_bEditMode)
		{
			
			
			const std::string DebugTex = GetTextureTypeName(
				TextureType) + " " + std::to_string(TextureIndex) + " : Search for a texture that doesn't exist";
			Log(DebugTex);
		}
		return nullptr;

	}

}MATERIAL, *LPMATERIAL;

typedef struct tagTextureDesc
{
	uint32	nUVIdx;
	float	fBlendFactor;
	uint32	eTextureOperator;
	uint32	eMappingMode;
	uint32	eMappingType;
}TEXTUREDESC, * LPTEXTUREDESC;

typedef struct _tagPxUserData
{
	std::weak_ptr<class Collider> pCollider;
}PXUSERDATA, *LPPXUSERDATA;



typedef struct tagBattleInfo
{
	int		iAttack;
	int		iMaxHp;
	int		iHp;

	ATTACKTYPE	eAttackType;
}BT_INFO;
END
#endif // !__ENGINE_STRUCT_H__