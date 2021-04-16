#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__
#include "EngineStdafx.h"
BEGIN(ENGINE)
class GraphicSystem;
class ENGINE_DLL Graphic abstract
{
private:
	static std::weak_ptr<GraphicSystem> m_pGraphicSystem;
public:
	static LPDIRECT3DDEVICE9	GetDevice();
};
END
#endif // !__GRAPHIC_H__
