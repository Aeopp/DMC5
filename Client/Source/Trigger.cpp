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
	TriggerLocation = Location;

	if (auto _Collider = GetComponent<BoxCollider>().lock();
		_Collider)
	{
		_Collider->SetSize(ColliderBoxSize);
		_Collider->SetActive(true);
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

	//if (auto _Collider = GetComponent<BoxCollider>().lock();
	//	_Collider)
	//{
	//	_Collider->SetActive(true);
	//}

	//if (auto _Transform = GetComponent<Transform>().lock();
	//	_Transform)
	//{
	//	_Transform->SetPosition(TriggerLocation);
	//};
}

void Trigger::TriggerDisable()
{
	SetActive(false);
	/*if (auto _Collider = GetComponent<BoxCollider>().lock();
		_Collider)
	{
		_Collider->SetActive(false);
	};*/
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
}

void Trigger::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

HRESULT Trigger::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());

	_Collider = AddComponent<BoxCollider>();
	_Collider.lock()->SetTrigger(true);

	PushEditEntity(_Collider.lock().get());

	m_nTag = TAG_Trigger;
	SetRenderEnable(true);
	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };
	RenderInterface::Initialize(_InitRenderProp);

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

		if (auto _Collider = GetComponent<BoxCollider>().lock();
			_Collider)
		{
			const char* ColliderMsg = _Collider->IsActive() ? "ColliderEnable" : "ColliderDisable";
			ImGui::Text(ColliderMsg);

			Vector3 CurrentBoxColliderSize = _Collider->GetSize();
			ImGui::SliderFloat3("BoxColliderSize", CurrentBoxColliderSize, 0.0f, 10.f);
		}
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

void Trigger::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
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


void Trigger::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{

}



