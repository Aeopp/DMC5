#include "..\Header\Graphic.h"
#include "GraphicSystem.h"

USING(ENGINE)

std::weak_ptr<GraphicSystem> Graphic::m_pGraphicSystem = GraphicSystem::GetInstance();

LPDIRECT3DDEVICE9 Graphic::GetDevice()
{
	if (true == m_pGraphicSystem.expired())
		return nullptr;
	return m_pGraphicSystem.lock()->GetDevice();
}