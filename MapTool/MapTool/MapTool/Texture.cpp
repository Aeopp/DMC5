#include "framework.h"
#include "Texture.h"

Texture::Texture()
	: m_pTexture(nullptr)
{
	ZeroMemory(&m_tInfo, sizeof(decltype(m_tInfo)));
	ZeroMemory(&m_tDesc, sizeof(decltype(m_tDesc)));
}

Texture::Texture(const Texture& _rOther)
	: m_pTexture(_rOther.m_pTexture)
{
	SafeAddRef(m_pTexture);
	memcpy_s(&m_tInfo, sizeof(D3DXIMAGE_INFO), &_rOther.m_tInfo, sizeof(D3DXIMAGE_INFO));
	m_tDesc = _rOther.m_tDesc;
}

Texture::~Texture()
{
	SafeRelease(m_pTexture);
}

Texture* Texture::Clone()
{
	Texture* pClone = new Texture(*this);
	return pClone;
}

HRESULT Texture::LoadTexture(const std::filesystem::path _Path)
{
#ifdef UNICODE
	std::wstring sFilePath = _Path.wstring();
#else
	std::string sFilePath = _Path.string();
#endif // UNICODE

	if (FAILED(D3DXGetImageInfoFromFile(sFilePath.c_str(), &m_tInfo)))
	{
		return E_FAIL;
	}

	if (FAILED(D3DXCreateTextureFromFileEx(g_pDevice
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
		return E_FAIL;
	}

	return S_OK;
}
