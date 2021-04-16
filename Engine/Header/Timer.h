#ifndef __TIME_H__
#define __TIME_H__
#include "EngineStdafx.h"
BEGIN(ENGINE)
class TimeSystem;
class ENGINE_DLL Time abstract
{
private:
	static std::weak_ptr<TimeSystem> m_pTimeSystem;
public:
	static float DeltaTime();
};
END
#endif // !__TIME_H__
