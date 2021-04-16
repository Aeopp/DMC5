#include "..\Header\Timer.h"
#include "TimeSystem.h"

USING(ENGINE)

std::weak_ptr<TimeSystem> Time::m_pTimeSystem = TimeSystem::GetInstance();

float Time::DeltaTime()
{
	return m_pTimeSystem.lock()->DeltaTime();
}
