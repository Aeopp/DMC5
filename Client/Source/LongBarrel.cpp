#include "stdafx.h"
#include "..\Header\LongBarrel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "Thunderbolt.h"

void LongBarrel::Free()
{
	GameObject::Free();
};

std::string LongBarrel::GetName()
{
	return "LongBarrel";
};

LongBarrel* LongBarrel::Create()
{
	return new LongBarrel{};
};




HRESULT LongBarrel::Ready()
{
	// 트랜스폼 초기화 .. 
	m_nTag = Eff_BlitzAttack;
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.

	for (auto& _DescElement : _PlayEffectDescs)
	{
		EffectDesc _Desc{};
		_Desc._Effect = AddGameObject<ThunderBolt>();
		_DescElement = _Desc;
	}

	return S_OK;
};

HRESULT LongBarrel::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01,0.01,0.01 });
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.5f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });
	return S_OK;
}

HRESULT LongBarrel::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT LongBarrel::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	
	T += _fDeltaTime;

	for (auto& _EffectRest : _PlayEffectDescs)
	{

	}

	return 0;
}


UINT LongBarrel::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void LongBarrel::Editor()
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

		ImGui::SliderFloat3("PlayDirection", PlayDirection, -1.f, 1.f);
		ImGui::SliderFloat("PlayLocationOffset", &PlayLocationOffset, 0.f, 1.f);
		ImGui::SliderFloat("VelocityOffset Begin", &VelocityOffset.first, 0.f, 9.9f);
		ImGui::SliderFloat("VelocityOffset End", &VelocityOffset.second, 0.f, 10.f);
		ImGui::SliderFloat("ScaleOffset", &ScaleOffset, 0.f, 1.f);
		ImGui::InputFloat("In ScaleOffset", &ScaleOffset, 0.f, 1.f);

		ImGui::SliderFloat("LightFlux", &LightFlux, 0.0f, 1.f);

		ImGui::SliderFloat("DirYawOffset ", &DirYawOffset, -FMath::PI ,FMath::PI);
		ImGui::SliderFloat("DirPitchOffset ", &DirPitchOffset, -FMath::PI, FMath::PI);

		ImGui::SliderFloat("PlayTimeRange Begin", &PlayTimeRange.first, 0.0f,1.f);
		ImGui::SliderFloat("PlayTimeRange End", &PlayTimeRange.second, 0.0f,1.f);
		;
	}
};

void LongBarrel::OnEnable()
{
	GameObject::OnEnable();
}

void LongBarrel::OnDisable()
{
	GameObject::OnDisable();
}

void LongBarrel::PlayStart(const Vector3& Location, const Vector3& PlayDirection)
{
	for (auto& _DescElement : _PlayEffectDescs)
	{
		if (auto _PlayEffect = _DescElement._Effect.lock();
			_PlayEffect)
		{
			const Vector3 _Location = FMath::RandomVector(PlayLocationOffset);
			const float _Velocity = FMath::Random(VelocityOffset.first, VelocityOffset.second);
			const Vector3 _Scale = FMath::RandomScale(ScaleOffset);
			const Vector3 _Right = FMath::Normalize(FMath::Cross(Vector3{ 0,1,0 }, PlayDirection)); 
			const Vector3 _Up = FMath::Normalize(FMath::Cross( PlayDirection, _Right ));
			Vector3 _Direction = PlayDirection;

			_Direction =FMath::RotationVecNormal(_Direction, _Up, 
							FMath::Random(-FMath::PI*DirYawOffset,FMath::PI * DirYawOffset));
			_Direction = FMath::RotationVecNormal(_Direction, _Right, 
							FMath::Random(-FMath::PI * DirPitchOffset, FMath::PI * DirPitchOffset));
			
			_PlayEffect->PlayStart(Location + _Location, _Direction, _Velocity ,_Scale);
			_PlayEffect->PlayTime = FMath::Random(PlayTimeRange);
			_PlayEffect->PtLightFlux = LightFlux;
			_PlayEffect->DistortionIntencity = 0.0f;
		}
	}
}