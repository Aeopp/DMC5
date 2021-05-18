#include "stdafx.h"
#include "..\Header\Satellite.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "Thunderbolt.h"
#include "ElectricBranch.h"


void Satellite::Free()
{
	GameObject::Free();
};

std::string Satellite::GetName()
{
	return "Satellite ";
};

Satellite* Satellite::Create()
{
	return new Satellite{};
};


HRESULT Satellite::Ready()
{
	// 트랜스폼 초기화 .. 
	m_nTag = Eff_BiAttack;
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.

	for (auto& _DescElement : _PlayEffectDescs)
	{
		Satellite::EffectDesc _Desc{};
		_Desc._Effect = AddGameObject<ThunderBolt>();
		_DescElement = _Desc;
	}

	_ElectricBranch = AddGameObject<ElectricBranch>();

	return S_OK;
};

HRESULT Satellite::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01,0.01,0.01 });
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.5f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });
	return S_OK;
}

HRESULT Satellite::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Satellite::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (bPlay == false)return     0;

	T += _fDeltaTime;	

	CurThunderBoltDelta -= _fDeltaTime;

	if ( CurThunderBoltDelta < 0.0f )  
	{
		CurThunderBoltDelta += ThunderBoltDelta;

		 for (auto& _EffectRest : _PlayEffectDescs)
		 {
			 if (auto SpThunderBolt = _EffectRest._Effect.lock();
				 SpThunderBolt)
			 {
				 if (SpThunderBolt->_RenderProperty.bRender == false)
				 {
					 const Vector3 NormDirection = FMath::Normalize(PlayDirection);
					 const Vector3 PlayLocation = GetComponent<Transform>().lock()->GetPosition()
												 + (NormDirection * ElectricForwardOffset);
					 const Vector3 _Location = FMath::RandomVector(PlayLocationOffset);
					 const float _Velocity = FMath::Random(VelocityOffset.first, VelocityOffset.second);
					 const Vector3 _Scale = FMath::RandomScale(ScaleOffset);
					 const Vector3 _Right = FMath::Normalize(FMath::Cross(Vector3{ 0,1,0 }, PlayDirection));
					 const Vector3 _Up = FMath::Normalize(FMath::Cross(PlayDirection, _Right));

					 Vector3 _Direction = NormDirection;
					 _Direction = FMath::RotationVecNormal(_Direction, _Up,
						 FMath::Random(-FMath::HalfPI * DirYawOffset, FMath::HalfPI * DirYawOffset));
					 _Direction = FMath::RotationVecNormal(_Direction, _Right,
						 FMath::Random(-FMath::HalfPI * DirPitchOffset, FMath::HalfPI * DirPitchOffset));

					 SpThunderBolt->PlayStart(PlayLocation + _Location, _Direction, _Velocity, _Scale);
					 SpThunderBolt->PlayTime = FMath::Random(PlayTimeRange);
					 SpThunderBolt->PtLightFlux = LightFlux;
					 SpThunderBolt->PtLightRadius = LightRadius;
					 SpThunderBolt->DistortionIntencity = 0.0f;

					 break;
				 }
			 }
		 }
	}


	return 0;
}


UINT Satellite::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void Satellite::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		/*const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str()); */

		if (ImGui::SmallButton("Play"))
		{
			PlayStart(GetComponent<Transform>().lock()->GetPosition(), PlayDirection);
		}


		ImGui::SliderFloat("ThunderBoltDelta", &ThunderBoltDelta, 0.f, 0.1f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);

		ImGui::SliderFloat("ElectricForwardOffset",&ElectricForwardOffset, 0.f, 1.f ,"%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);

		ImGui::SliderFloat3("PlayDirection", PlayDirection, -1.f, 1.f);
		ImGui::SliderFloat("PlayLocationOffset", &PlayLocationOffset, 0.f, 1.f,"%.6f",ImGuiSliderFlags_::ImGuiSliderFlags_None );
		ImGui::SliderFloat("VelocityOffset Begin", &VelocityOffset.first, 0.f, 9.9f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);
		ImGui::SliderFloat("VelocityOffset End", &VelocityOffset.second, 0.f, 10.f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);
		ImGui::SliderFloat("ScaleOffset", &ScaleOffset, 0.f, 1.f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);
		ImGui::InputFloat("In ScaleOffset", &ScaleOffset, 0.f, 1.f);
		
		ImGui::SliderFloat("BranchScale", &BranchScale, 0.f, 0.1f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);
		ImGui::InputFloat("In BranchScale", &BranchScale, 0.f, 0.1f);

		ImGui::SliderFloat("LightFlux", &LightFlux, 0.0f, 1.f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);
		ImGui::SliderFloat("LightRadius", &LightRadius, 0.0f, 1.f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);

		ImGui::SliderFloat("DirYawOffset ", &DirYawOffset, -FMath::PI ,FMath::PI);
		ImGui::SliderFloat("DirPitchOffset ", &DirPitchOffset, -FMath::PI, FMath::PI);

		ImGui::SliderFloat("PlayTimeRange Begin", &PlayTimeRange.first, 0.0f,1.f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);
		ImGui::SliderFloat("PlayTimeRange End", &PlayTimeRange.second, 0.0f,1.f, "%.6f", ImGuiSliderFlags_::ImGuiSliderFlags_None);;
	}
};

void Satellite::OnEnable()
{
	GameObject::OnEnable();
}

void Satellite::OnDisable()
{
	GameObject::OnDisable();
}

void Satellite::PlayStart(const Vector3& Location,
						 const Vector3& PlayDirection)
{
	bPlay = true;
	auto SpTransform = GetComponent<Transform>().lock();
	SpTransform->SetPosition(Location);
	const Vector3 CurrentPlayDireciton = FMath::Normalize(PlayDirection);
	const Vector3 PlayLocation = Location + CurrentPlayDireciton * ElectricForwardOffset;

	_ElectricBranch.lock()->PlayStart(PlayLocation, std::nullopt, Vector3{ BranchScale, BranchScale,BranchScale } );
}

void Satellite::PlayEnd()
{
	bPlay = false;
}

