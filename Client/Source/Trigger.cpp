#include "stdafx.h"
#include "..\Header\Trigger.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "Monster.h"

void Trigger::EventRegist(
	const std::function<void()>& CallBack,
	const Vector3& Location,
	const Vector3& ColliderBoxSize,
	const bool ImmediatelyEnable,/*생성 하자마자 활성화 ??*/
	const GAMEOBJECTTAG& TargetTag)
{
	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(Location);
	}

	if (auto _Collider = GetComponent<BoxCollider>().lock();
		_Collider)
	{
		_Collider->SetSize(ColliderBoxSize);
	}

	_Event = CallBack;
	_TargetTag = TargetTag;
	_Option = Option::None;

	if (ImmediatelyEnable)
		TriggerEnable();
	else
		TriggerDisable();
};

void Trigger::EventRegist(
	const std::vector<std::weak_ptr<class Monster>>& WaveMonster,
	const Vector3& Location,
	const Vector3& Size,
	const bool ImmediatelyEnable,
	const GAMEOBJECTTAG& TargetTag,
	const std::function<void()>& SpawnAfterEvent )
{
	for (auto& WpMonster : WaveMonster)
	{
		if (auto SpMonster = WpMonster.lock();
			SpMonster)
		{
			SpMonster->SetActive(false);
		}
	}

	auto _EventCallBack = [WaveMonster, SpawnAfterEvent]()
	{
		for (auto& WpMonster : WaveMonster)
		{
			if (auto SpMonster = WpMonster.lock();
				SpMonster)
			{
				SpMonster->SetActive(true);
			}
		}

		if (SpawnAfterEvent)
		{
			SpawnAfterEvent();
		}
	};

	EventRegist(_EventCallBack,
		Location,
		Size,
		ImmediatelyEnable,
		TargetTag);

	_Option = Option::MonsterWave;
}

bool Trigger::IsEnable()
{
	return IsActive();
}

void Trigger::TriggerEnable()
{
	SetActive(true);
}

void Trigger::TriggerDisable()
{
	SetActive(false);
}

void Trigger::Free()
{
	GameObject::Free();
};

std::string Trigger::GetName()
{
	return "Trigger";
};

Trigger* Trigger::Create()
{
	return new Trigger{};
};

HRESULT Trigger::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());

	_Collider = AddComponent<BoxCollider>();
	_Collider.lock()->SetTrigger(true);
	// _Collider.lock()->SetActive(false);
	//_Collider.lock()->SetSize(Vector3{ 1.f,1.f,1.f });

	PushEditEntity(_Collider.lock().get());

	return S_OK;
};

HRESULT Trigger::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	return S_OK;
}

HRESULT Trigger::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Trigger::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	return 0;
};

UINT Trigger::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void Trigger::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		if (ImGui::Button("Event Call"))
		{
			if (_Event)
			{
				_Event();
			}
		}

		const char* Msg = IsEnable() ? "Enable" : "Disable";
		ImGui::Text(Msg);
	}
}


void Trigger::OnEnable()
{
	GameObject::OnEnable();
}

void Trigger::OnDisable()
{
	GameObject::OnDisable();
}

void Trigger::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	if (_TargetTag != _pOther.lock()->m_nTag)
		return;

	if (IsEnable())
	{
		if (_Event)
		{
			_Event();
		}

		TriggerDisable();
	}
}


void Trigger::OnCollisionExit(std::weak_ptr<GameObject> _pOther)
{

}



