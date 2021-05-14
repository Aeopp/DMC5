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

	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
	};

	if (auto _Collider = GetComponent<BoxCollider>().lock();
		_Collider)
	{
		_Collider->SetSize(ColliderBoxSize);
		_Collider->SetActive(true);
	};


	// 트리거 발동 함수 등록 
	_Event = CallBack;
	// 트리거와 검사할 태그 
	_TargetTag = TargetTag;
	_Option = Option::None;

	// 즉시 트리거 활성화 할지 나중에 켤지 컨트롤.  
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
	const std::function<void()>& SpawnAfterEvent  ,
	const std::function<void()>& WaveEndEvent)
{
	CheckIfTheDead = std::vector<std::weak_ptr<class Monster>>();

	for (auto& WpMonster : WaveMonster)
	{
		// 몬스터 웨이브 이니 나중에 몬스터가 전부 없어지는 시점을 활성화 하기 위해 푸시 .
		CheckIfTheDead->push_back(WpMonster);

		if (auto SpMonster = WpMonster.lock();
			SpMonster)
		{
			// 몬스터 전부 비활성화 
			SpMonster->SetActive(false);
		}
	}

	// 몬스터 배열이 비어있었으므로 몬스터 사망 시점 체크할 필요 없으니 Optional 비워줌 
	if (CheckIfTheDead.value().empty())
	{
		CheckIfTheDead = std::nullopt;
	}

	// 트리거 발동 함수 
	auto _EventCallBack = [WaveMonster, SpawnAfterEvent]()
	{
		for (auto& WpMonster : WaveMonster)
		{
			if (auto SpMonster = WpMonster.lock();
				SpMonster)
			{
				// 몬스터 활성화 
				SpMonster->SetActive(true);
			}
		}

		// 몬스터 활성화 하고 난 직후 이벤트도 넘겨받았다면 이벤트 호출 . 
		if (SpawnAfterEvent)
		{
			SpawnAfterEvent();
		}
	};

	// 웨이브 끝났을시의 이벤트 등록 . 
	this->_WaveEndEvent = WaveEndEvent;

	// 나머지 로직은 공통되므로 해당 함수로 처리가능 
	EventRegist(_EventCallBack,
		Location,
		Size,
		ImmediatelyEnable,
		TargetTag);

	_Option = Option::MonsterWave;
}

bool Trigger::IsAfterEvent()
{
	return bAfterEvent;
}

bool Trigger::IsEnable()
{
	return bEnable;
}

void Trigger::TriggerEnable()
{
	bEnable = true;

	if (auto _Collider = GetComponent<BoxCollider>().lock();
		_Collider)
	{
		_Collider->SetActive(true);
	}

	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
	};
}

void Trigger::TriggerDisable()
{
	bEnable = false;

	if (auto _Collider = GetComponent<BoxCollider>().lock();
		_Collider)
	{
		_Collider->SetActive(false);
	};

	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
	};
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
	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
	};
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
	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
	};
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

	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
	};

	if (IsAfterEvent() 
		&&  (_Option == Option::MonsterWave )  
		&&  CheckIfTheDead.has_value())
	{
		const bool bWaveEnd = std::all_of(std::begin(*CheckIfTheDead), std::end(*CheckIfTheDead), []
		(const auto& _Monster)
			{
				return (_Monster.expired() || (_Monster.lock()->Get_BattleInfo().iHp <= 0));
			});

		// 몬스터 전부 만료되었다면 웨이브 종료는 true 이므로 호출 
		if(bWaveEnd)
		{
			if (_WaveEndEvent)
			{
				_WaveEndEvent();
			}
			// null로 초기화해서 다음번에는 로직 안타도록 체크 . 
			CheckIfTheDead = std::nullopt;
		}
	}

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

		if (ImGui::Button("Enable"))
		{
			TriggerEnable();
		}

		if (ImGui::Button("Disable"))
		{
			TriggerDisable();
		}

		const char* Msg = IsEnable() ? "Enable" : "Disable";
		ImGui::Text(Msg);

		const char* AfterEventMsg = IsAfterEvent() ? "AfterEvent" : "BeforeEvent";
		ImGui::Text(AfterEventMsg);

		const char* CheckIfTheDeadMsg = CheckIfTheDead.has_value() ? "CheckIfTheDead Has Value" : "CheckIfTheDead NULL";
		ImGui::Text(CheckIfTheDeadMsg);

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
	// 태그가 일치하지 않다면 리턴 
	if (_TargetTag != _pOther.lock()->m_nTag)
		return;

	// 태그 일치  && 활성화 이고 이벤트 등록되었다면 호출 하고 AfterEvent 에 이벤트가 호출되었다고 표시 . 
	if (IsEnable())
	{
		if (_Event)
		{
			_Event();
			bAfterEvent = true;
		}

		// 호출하고 트리거 비활성화 .
		TriggerDisable();
	}
}


void Trigger::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{

}



