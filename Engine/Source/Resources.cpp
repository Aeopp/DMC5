#include "Resources.h"

USING(ENGINE)

std::weak_ptr<ResourceSystem> Resources::m_pResourceSystem = ResourceSystem::GetInstance();