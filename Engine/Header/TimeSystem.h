#ifndef __TIME_SYSTEM_H__
#define __TIME_SYSTEM_H__
#include "Object.h"
#include <chrono>

BEGIN(ENGINE)
class TimeSystem final : public Object
{
	DECLARE_SINGLETON(TimeSystem)
private:
	float           m_fAccTime;
	float			m_fDeltaTime;
	double          m_fAccDeltaTime;
	LARGE_INTEGER	m_tCPUTick;
	LARGE_INTEGER	m_tStartFrame;
	LARGE_INTEGER	m_tEndFrame;
	uint64			m_uiFrameRate;
	uint64          m_uiUpdateCount;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_PrevTime;
private:
	explicit TimeSystem();
	virtual ~TimeSystem() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	HRESULT ReadyTimeSystem();
	HRESULT UpdateTimeSystem();
	void	Editor();
private:
	void	UpdateDeltaTime();
public:
	float   AccTime();
	float	DeltaTime();
};
END
#endif // !__TIME_SYSTEM_H__
