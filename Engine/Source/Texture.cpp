#include "Texture.h"

USING(ENGINE)

Texture::Texture(LPDIRECT3DDEVICE9 const _pDevice)
	: Resource(_pDevice)
	, m_pTexture(nullptr)
{
	
	ZeroMemory(&m_tInfo, sizeof(decltype(m_tInfo)));
	ZeroMemory(&m_tDesc, sizeof(decltype(m_tDesc)));
}

Texture::Texture(const Texture& _rOther)
	: Resource(_rOther)
	, m_pTexture(_rOther.m_pTexture)
{
	SafeAddRef(m_pTexture);
	memcpy_s(&m_tInfo, sizeof(D3DXIMAGE_INFO), &_rOther.m_tInfo, sizeof(D3DXIMAGE_INFO));
	m_tDesc=_rOther.m_tDesc;
}

void Texture::Editor()
{
	Resource::Editor();

	if (bEdit)
	{
		ImGui::BulletText("UV Index : %d", GetDesc().nUVIdx);
		ImGui::BulletText("fBlendFactor : %3.3f", GetDesc().fBlendFactor);
		ImGui::BulletText("eMappingMode : %d", GetDesc().eMappingMode);
		ImGui::BulletText("eMappingType : %d", GetDesc().eMappingType);
		ImGui::BulletText("eTextureOperator : %d", GetDesc().eTextureOperator);
	};
}

std::string Texture::GetName()
{
	return "Texture";
}

void Texture::Free()
{
	SafeRelease(m_pTexture);
	Resource::Free();
}

Texture* Texture::Create(LPDIRECT3DDEVICE9 const _pDevice, 
							const std::filesystem::path _Path ,
							const std::any& InitParams)
{
	Texture* pInstance = new Texture(_pDevice);
	if (FAILED(pInstance->LoadTextureFromFile(_Path)))
	{
		pInstance->Free();
		delete pInstance;
		return nullptr;
	}
	return pInstance;
};


Resource* Texture::Clone()
{
	Texture* pClone = new Texture(*this);
	return pClone;
}

HRESULT Texture::LoadTextureFromFile(const std::filesystem::path _Path)
{
#ifdef UNICODE
	std::wstring sFilePath = _Path.wstring();
#else
	std::string sFilePath = _Path.string();
#endif // UNICODE
	
	if (FAILED(D3DXGetImageInfoFromFile(sFilePath.c_str(), &m_tInfo)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to D3DXGetImageInfoFromFile"));
		return E_FAIL;
	}

	if (FAILED(D3DXCreateTextureFromFileEx(m_pDevice
		, sFilePath.c_str()
		, m_tInfo.Width
		, m_tInfo.Height
		, m_tInfo.MipLevels
		, 0
		, m_tInfo.Format
		, D3DPOOL_MANAGED
		, D3DX_DEFAULT
		, D3DX_DEFAULT
		, 0
		, nullptr
		, nullptr
		, &m_pTexture)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to D3DXCreateTextureFromFileEx"));
		return E_FAIL;
	}
	ResourcePath = sFilePath;
	
	return S_OK;
}

LPDIRECT3DTEXTURE9 Texture::GetTexture()
{
	return m_pTexture;
}

D3DXIMAGE_INFO Texture::GetInfo()
{
	return m_tInfo;
}

TEXTUREDESC Texture::GetDesc()
{
	return m_tDesc;
}

void Texture::SetDesc(const TEXTUREDESC& _tDesc)
{
	m_tDesc = _tDesc;
}
