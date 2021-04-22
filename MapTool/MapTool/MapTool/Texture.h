#ifndef __TEXTURE_H__
#define __TEXTURE_H__
class Texture
{
public:
	LPDIRECT3DTEXTURE9	m_pTexture;
	D3DXIMAGE_INFO		m_tInfo;
	TEXTUREDESC			m_tDesc;
public:
	Texture();
	Texture(const Texture& _rOther);
	~Texture();
public:
	Texture* Clone();
public:
	HRESULT LoadTexture(const std::filesystem::path _Path);
};
#endif // !__TEXTURE_H__