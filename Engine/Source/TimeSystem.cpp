#include "..\Header\TimeSystem.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(TimeSystem)

TimeSystem::TimeSystem()
	: m_fDeltaTime(0.f),
	m_fAccDeltaTime(0.f),
	m_uiFrameRate(0u),
	m_uiUpdateCount(0u),
	m_fAccTime(0.0f)
{
	ZeroMemory(&m_tCPUTick, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tStartFrame, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tEndFrame, sizeof(LARGE_INTEGER));
};


void TimeSystem::Free()
{

};

void TimeSystem::LostTime(const std::vector<Vector3>& ALostArr)
{
	LostArr = ALostArr;
	LostIdx = 0;
	bLost = false;
	LostT = 0.0f;
	SetSlowly(LostArr[0].z);
}

void TimeSystem::SetSlowly(const float Slowly)
{
	this->Slowly = Slowly;
}

HRESULT TimeSystem::ReadyTimeSystem()
{
	m_PrevTime = std::chrono::high_resolution_clock::now();

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
	ImGui::Checkbox("FrameLimit", &g_bFrameLimit);
	ImGui::Checkbox("FixedDeltaTime", &g_bFixedDeltaTime);
	ImGui::Text("Frame Rate : %d ", m_uiFrameRate);
	ImGui::Text("Delta Time : %2.6f ", m_fDeltaTime);
	ImGui::Text("Accumulate Time : %8.6f ", m_fAccTime);
	ImGui::SliderFloat("Slow", &Slowly, 0.0f, 1.f, "%2.6f");
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

	if (LostIdx >= LostArr.size())
	{
		SetSlowly(1.f);
		bLost = false;
		return;
	}
	else if (LostArr.empty()==false)
	{
		LostT += Delta;

		if (bLost)
		{
			if (LostT >= LostArr[LostIdx].y)
			{
				bLost = false;
				SetSlowly(LostArr[LostIdx].z);
				LostT = 0.0f;
			}
		}
		else
		{
			if (LostT >= LostArr[LostIdx].x)
			{
				bLost = true;
				LostT = 0.0f;
				SetSlowly(1.f);
				++LostIdx;
			}
		}
	}
}

float TimeSystem::AccTime()
{
	return m_fAccTime;
}

float TimeSystem::DeltaTime()
{
	return m_fDeltaTime * Slowly;
}

float TimeSystem::OriginDeltaTime()
{
	return m_fDeltaTime;
}


