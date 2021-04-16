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
	/*QueryPerformanceCounter(&m_tStartFrame);
	QueryPerformanceCounter(&m_tEndFrame);
	QueryPerformanceFrequency(&m_tCPUTick);*/
	m_PrevTime= std::chrono::high_resolution_clock::now();

	return S_OK;
}

HRESULT TimeSystem::UpdateTimeSystem()
{
	UpdateDeltaTime();
	return S_OK;
}

void TimeSystem::Editor()
{
	ImGui::Begin("Time");
	ImGui::Text("Frame Rate : %d ", m_uiFrameRate);
	ImGui::Text("Delta Time : %2.6f ", m_fDeltaTime);
	ImGui::Text("Accumulate Time : %9.1f ", m_fAccTime);
	ImGui::End();
}

void TimeSystem::UpdateDeltaTime()
{
	/*QueryPerformanceFrequency(&m_tCPUTick);
	QueryPerformanceCounter(&m_tEndFrame);*/

	auto CurTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float,std::ratio<1,1000>> Delta = CurTime - m_PrevTime;

	m_PrevTime = CurTime;
	m_fDeltaTime = (Delta.count() * 0.001f);

	//m_fDeltaTime = (m_tEndFrame.QuadPart - m_tStartFrame.QuadPart)
	//	/			m_tCPUTick.QuadPart;


	m_fAccDeltaTime += m_fDeltaTime;
	m_fAccTime += m_fDeltaTime;
	++m_uiUpdateCount;
	if (m_fAccDeltaTime > 1.0)
	{
		m_fAccDeltaTime -= 1.0;
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


