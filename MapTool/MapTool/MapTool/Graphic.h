#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

class Graphic final
{
public:
	LPDIRECT3D9			m_pSDK;
	LPDIRECT3DDEVICE9	m_pDevice;
public:
	Graphic();
	~Graphic();
public:
	HRESULT ReadyGraphic();
};

#endif // !__GRAPHIC_H__
