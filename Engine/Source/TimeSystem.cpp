#include "..\Header\TimeSystem.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(TimeSystem)

TimeSystem::TimeSystem()
	: m_fDeltaTime(0.f) ,  
	m_fAccDeltaTime (0.f) , 
	m_uiFrameRate(0u),
	m_uiUpdateCount(0u)
{
	ZeroMemory(&m_tCPUTick, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tStartFrame, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tEndFrame, sizeof(LARGE_INTEGER));
}

void TimeSystem::Free()
{

}

HRESULT TimeSystem::ReadyTimeSystem()
{
	m_PrevTime= std::chrono::high_resolution_clock::now();

	return S_OK;
}

HRESULT TimeSystem::UpdateTimeSystem(const float Delta)
{
	UpdateDeltaTime(Delta);
	return S_OK;
}

void TimeSystem::Editor()
{
	ImGui::Begin("Time");
	ImGui::Text("Frame Rate : %d ", m_uiFrameRate);
	ImGui::Text("Delta Time : %2.6f ", m_fDeltaTime);
	ImGui::Text("Accumulate Time : %8.6f ", m_fAccTime);
	ImGui::End();
}

void TimeSystem::UpdateDeltaTime(const float Delta)
{
	m_fDeltaTime = Delta;
	m_fAccDeltaTime += Delta;
	m_fAccTime += Delta;

	++m_uiUpdateCount;
	if (m_fAccDeltaTime > 1.0f)
	{
		m_fAccDeltaTime -= 1.0f;
		m_uiFrameRate = m_uiUpdateCount;
		m_uiUpdateCount = 0u;
	}
	m_tStartFrame = m_tEndFrame;
}

float TimeSystem::AccTime()
{
	return m_fAccTime;
}

float TimeSystem::DeltaTime()
{
	return m_fDeltaTime;
}


