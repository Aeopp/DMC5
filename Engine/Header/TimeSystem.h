#ifndef __TIME_SYSTEM_H__
#define __TIME_SYSTEM_H__
#include "Object.h"
#include <chrono>

BEGIN(ENGINE)
class ENGINE_DLL TimeSystem final : public Object
{
	DECLARE_SINGLETON(TimeSystem)
private:
	float           m_fAccTime;
	float			m_fDeltaTime;
	float           m_fAccDeltaTime;
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
	void SetSlowly(const float Slowly);
	float Slowly = 1.f;
	bool bTimeRender = true;
	HRESULT ReadyTimeSystem();
	HRESULT UpdateTimeSystem(const float Delta);
	void	Editor();
private:
	void	UpdateDeltaTime(const float Delta);
public:
	float   AccTime();
	float	DeltaTime();
	void LostTime(const std::vector<Vector2>& ALostArr);
private:
	// X :Slow Y : Re
	std::vector<Vector2> LostArr;
	uint32 LostIdx = 0u;
	bool   bLost = false;
	float  LostT = 0.0f;
};
END
#endif // !__TIME_SYSTEM_H__
