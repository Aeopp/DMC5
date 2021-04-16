#ifndef __GRAPHIC_SYSTEM_H__
#define __GRAPHIC_SYSTEM_H__
#include "Object.h"
BEGIN(ENGINE)
class GraphicSystem final : public Object
{
	DECLARE_SINGLETON(GraphicSystem)
private:
	LPDIRECT3D9			m_pSDK;
	LPDIRECT3DDEVICE9	m_pDevice;
private:
	explicit GraphicSystem();
	virtual	~GraphicSystem() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	HRESULT ReadyGraphicSystem(
		const bool bWindowed ,
		const bool bMultiSample);
	void Begin()&;
	void End()&;
public:
	LPDIRECT3DDEVICE9	GetDevice();
};
END
#endif // !__GRAPHIC_SYSTEM_H__