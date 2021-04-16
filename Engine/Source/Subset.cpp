#include "Subset.h"
#include "Texture.h"

USING(ENGINE)
Subset::Subset(LPDIRECT3DDEVICE9 const _pDevice)
	: m_pDevice(_pDevice)
	, m_pVertexBuffer(nullptr)
	, m_pIndexBuffer(nullptr)
{
	SafeAddRef(_pDevice);
}

void Subset::Free()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pDevice);
	Object::Free();
}

void Subset::Editor()
{
	Object::Editor();
	if (bEdit)
	{
		std::string RenderText = "Render ? : " +  std::to_string(UniqueID);
		ImGui::Checkbox(RenderText.c_str(), &bRender);

		ImGui::Text("nNumUVChannel %d", m_tVertexBufferDesc.nNumUVChannel);
		ImGui::Text("nMaxBonesRefPerVtx %d", m_tVertexBufferDesc.nMaxBonesRefPerVtx);
		for (uint32 i = 0; i < m_tVertexBufferDesc.nNumUVChannel; ++i)
		{
			ImGui::Text("Channel %d NumComponents %d", i,  m_tVertexBufferDesc.vecNumUVComponents[i]);
		};
		ImGui::Text("bHasPosition %d", m_tVertexBufferDesc.bHasPosition);
		ImGui::Text("bHasNormal %d", m_tVertexBufferDesc.bHasNormal);
		ImGui::Text("bHasTangentBiNormal %d", m_tVertexBufferDesc.bHasTangentBiNormal);
		ImGui::Text("bHasBone %d", m_tVertexBufferDesc.bHasBone);
		
		m_tMaterial.Editor();
		for (auto&  [Key,TexArray] : m_tMaterial.Textures)
		{
			for(auto& TexPtr: TexArray)
			{
				if (TexPtr)
				{
					TexPtr->Editor();
				}
			}
		}
	}
}

std::string Subset::GetName()
{
	return "Subset";
}

Subset* Subset::Create(LPDIRECT3DDEVICE9 const _pDevice)
{
	Subset* pInstance = new Subset(_pDevice);
	return pInstance;
}

void Subset::Render(ID3DXEffect*const Fx)
{
	if (nullptr == m_pVertexBuffer || nullptr == m_pIndexBuffer)
		return;
	if (Fx)
	{
		if (m_tVertexBufferDesc.bHasBone)
		{
			Fx->SetInt("nMaxBonesRefPerVtx", m_tVertexBufferDesc.nMaxBonesRefPerVtx);
		}
	}
	Fx->CommitChanges();
	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_tVertexBufferDesc.nStride);
	m_pDevice->SetVertexDeclaration(m_tVertexBufferDesc.pVertexDecl);
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_tVertexBufferDesc.nNumVertices, 0, m_tVertexBufferDesc.nNumFaces);
}

const LPDIRECT3DVERTEXBUFFER9 Subset::GetVertexBuffer()
{
	return m_pVertexBuffer;
}

const LPDIRECT3DINDEXBUFFER9 Subset::GetIndexBuffer()
{
	return m_pIndexBuffer;
}

void Subset::BindProperty(const UINT TexType,const uint64 TexIdx,const std::string& ShaderParamName ,
	ID3DXEffect* const Fx)&
{
	if (auto Tex = m_tMaterial.GetTexture(TexType, TexIdx);
		Tex)
	{
		if (FAILED(Fx->SetTexture(ShaderParamName.c_str(), Tex->GetTexture())))
		{
		}
		else
		{
			return;
		}
	}

    Log("Shader Texture Bind Failed !!");
}

void Subset::BindProperty(const UINT TexType, const uint64 TexIdx, const UINT RegisterIdx, IDirect3DDevice9* const _Device)&
{
	if (auto Tex = m_tMaterial.GetTexture(TexType, TexIdx);
		Tex)
	{
		if (FAILED(_Device->SetTexture(RegisterIdx, Tex->GetTexture())))
		{
		}
		else
		{
			return;
		}
	}

	Log("Shader Texture Bind Failed !!");
}

const VERTEXBUFFERDESC& Subset::GetVertexBufferDesc()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_tVertexBufferDesc;
}

const MATERIAL& Subset::GetMaterial()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_tMaterial;
}

void Subset::Initialize(LPDIRECT3DVERTEXBUFFER9 const _pVB, LPDIRECT3DINDEXBUFFER9 const _pIB, const VERTEXBUFFERDESC& _tVBDesc, const MATERIAL& _tMaterial)
{
	m_pVertexBuffer		= _pVB;
	m_pIndexBuffer		= _pIB;
	m_tVertexBufferDesc = _tVBDesc;
	m_tMaterial			= _tMaterial;
}

