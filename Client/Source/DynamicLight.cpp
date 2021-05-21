#include "stdafx.h"
#include "..\Header\DynamicLight.h"
#include "Renderer.h"
#include "FMath.hpp"

void DynamicLight::PlayStart(const Vector3& Location , const float PlayTime)
{
	PlayEnd();

	if (_PtLight = Renderer::GetInstance()->RefRemainingDynamicLight();
		_PtLight.expired() == false)
	{
		auto SpPtLight = _PtLight.lock();
		SpPtLight->bEnable = true;
		SpPtLight->Color = FMath::ToColor(Color.first);
		SpPtLight->PointRadius = PointRadius.first;
		SpPtLight->lightFlux = Flux.first;
		SpPtLight->SetPosition(FMath::ConvertVector4( Location,1.f));
	};

	T = 0.0f;
	this->PlayTime = PlayTime;
}

void DynamicLight::Update(const float Dt,const Vector3& Location)
{
	T += Dt;
	if (T > PlayTime)
	{
		PlayEnd();
	}

	if (auto SpPtLight = _PtLight.lock();
		SpPtLight)
	{
		const float LerpTime = std::sinf((T / PlayTime) * FMath::PI);
		SpPtLight->Color = FMath::ToColor(FMath::Lerp(Color.first, Color.second, LerpTime));
		SpPtLight->PointRadius = FMath::Lerp(PointRadius.first, PointRadius.second, LerpTime);
		SpPtLight->lightFlux = FMath::Lerp(Flux.first, Flux.second, LerpTime);
		SpPtLight->SetPosition(FMath::ConvertVector4(Location,1.f));
	}
}

void DynamicLight::PlayEnd()
{
	if (auto SpPtLight = _PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = false;
	}

	T = 0.0f;
}

void DynamicLight::Editor()
{
	if (ImGui::CollapsingHeader("PtLight"))
	{
		ImGui::ColorEdit4("Color Begin", Color.first);
		ImGui::ColorEdit4("Color End", Color.second);

		ImGui::SliderFloat("Radius Begin", &PointRadius.first, 0.f, 10.f);
		ImGui::SliderFloat("Radius End", &PointRadius.second, 0.f, 10.f);

		ImGui::SliderFloat("Flux Begin", &Flux.first, 0.f, 10.f);
		ImGui::SliderFloat("Flux End", &Flux.second, 0.f, 10.f);
	}
}
