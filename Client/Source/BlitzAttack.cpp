#include "stdafx.h"
#include "..\Header\AirHike.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "..\Header\BlitzAttack.h"
#include "Thunderbolt.h"
#include "ThunderboltSecond.h"


void BlitzAttack::Free()
{
	GameObject::Free();
};

std::string BlitzAttack::GetName()
{
	return "BlitzAttack";
};

BlitzAttack* BlitzAttack::Create()
{
	return new BlitzAttack{};
};


void BlitzAttack::PlayEnd()
{
    T = 0.0f;
};



HRESULT BlitzAttack::Ready()
{
	// 트랜스폼 초기화 .. 
	m_nTag = Eff_BlitzAttack;
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.

	for (auto& _DescElement : _PlayEffectDescs)
	{
		EffectDesc _Desc{};
		_Desc.LocationOffset = FMath::Random(Vector3{-0.5f,0.0f,-0.5f}, Vector3{ 0.5f,0.0f,0.5f });
		_Desc.SpawnTime = FMath::Random(0.f, 3.f);
		_Desc.bPlayed = true;
		const uint32 Dice = FMath::Random(0u, 1u);

		if (Dice == 1u)
		{
			_Desc._Effect = AddGameObject<ThunderBoltSecond>();
			_Desc._Tag = GAMEOBJECTTAG::Eff_ThunderBoltSecond;

			/*_Desc._Effect = AddGameObject<ThunderBolt>();
			_Desc._Tag = GAMEOBJECTTAG::Eff_ThunderBolt;*/
		}
		else
		{
			_Desc._Effect = AddGameObject<ThunderBoltSecond>();
			_Desc._Tag = GAMEOBJECTTAG::Eff_ThunderBoltSecond;
		}

		_DescElement = _Desc;
	}

	return S_OK;
};

HRESULT BlitzAttack::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01,0.01,0.01 });
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.02f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });
	return S_OK;
}

HRESULT BlitzAttack::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT BlitzAttack::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	
	T += _fDeltaTime;

	bool bPlayEnd = true;

	for (auto& _EffectRest : _PlayEffectDescs)
	{
		if (auto SpEffect = _EffectRest._Effect.lock();
			SpEffect)
		{
			if (_EffectRest.bPlayed == false)
			{
				bPlayEnd = false;
				if (T>= _EffectRest.SpawnTime)
				{
					_EffectRest.bPlayed = true;
					const Vector3 Location = GetComponent<Transform>().lock()->GetPosition();

					if (_EffectRest._Tag == GAMEOBJECTTAG::Eff_ThunderBolt)
					{
						auto SpThunderBolt = std::dynamic_pointer_cast<ThunderBolt>(SpEffect);
						SpThunderBolt->PlayStart(Location + _EffectRest.LocationOffset);
					}
					else if (_EffectRest._Tag == GAMEOBJECTTAG::Eff_ThunderBoltSecond)
					{
						auto SpThunderBolt = std::dynamic_pointer_cast<ThunderBoltSecond>(SpEffect);
						SpThunderBolt->PlayStart(Location + _EffectRest.LocationOffset);
					}
				}
			}
		}
	}

	if (bPlayEnd)
	{
		PlayEnd();
	}

	return 0;
}


UINT BlitzAttack::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void BlitzAttack::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str()); 
		if (ImGui::SmallButton("Play"))
		{
			PlayStart(GetComponent<Transform>().lock()->GetPosition());
		}

		static float LocationOffsetY = 0.0f;
		ImGui::InputFloat("LocationOffsetY", &LocationOffsetY);
		for (auto& _EffectDesc : _PlayEffectDescs)
		{
			if (_EffectDesc._Tag == GAMEOBJECTTAG::Eff_ThunderBolt)
			{
				_EffectDesc.LocationOffset.y = LocationOffsetY;
			}
		}

		uint32 Idx = 0;
		for (auto& _EffectDesc : _PlayEffectDescs)
		{
			if (ImGui::TreeNode(std::to_string(Idx).c_str() ))
			{
				ImGui::SliderFloat("SpawnTime", &_EffectDesc.SpawnTime, 0.0f, 3.f);
				ImGui::SliderFloat3("LocationOffset", _EffectDesc.LocationOffset, -10.f, 10.f);
				ImGui::TreePop();
			}
			++Idx;
		}

		ImGui::EndChild();
	}
}


void BlitzAttack::OnEnable()
{
	GameObject::OnEnable();
}

void BlitzAttack::OnDisable()
{
	GameObject::OnDisable();
}

void BlitzAttack::PlayStart(const Vector3& Location)
{
	for (auto& _EffectRest : _PlayEffectDescs)
	{
		_EffectRest.bPlayed = false;
	}
	T = 0.0f;
}
