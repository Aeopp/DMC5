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
	const bool ImmediatelyEnable,/*���� ���ڸ��� Ȱ��ȭ ??*/
	const GAMEOBJECTTAG& TargetTag ,
	const Vector3& Rotation)
{
	TriggerLocation = Location;
	TriggerRotation = Rotation;

	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
		_Transform->SetRotation(TriggerRotation);
	};

	if (auto _Collider = GetComponent<BoxCollider>().lock();
		_Collider)
	{
		_Collider->SetSize(ColliderBoxSize);
		_Collider->SetActive(true);
	};


	// Ʈ���� �ߵ� �Լ� ��� 
	_Event = CallBack;
	// Ʈ���ſ� �˻��� �±� 
	_TargetTag = TargetTag;
	_Option = Option::None;

	// ��� Ʈ���� Ȱ��ȭ ���� ���߿� ���� ��Ʈ��.  
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
		// ���� ���̺� �̴� ���߿� ���Ͱ� ���� �������� ������ Ȱ��ȭ �ϱ� ���� Ǫ�� .
		CheckIfTheDead->push_back(WpMonster);

		if (auto SpMonster = WpMonster.lock();
			SpMonster)
		{
			// ���� ���� ��Ȱ��ȭ 
			SpMonster->SetActive(false);
		}
	}

	// ���� �迭�� ����־����Ƿ� ���� ��� ���� üũ�� �ʿ� ������ Optional ����� 
	if (CheckIfTheDead.value().empty())
	{
		CheckIfTheDead = std::nullopt;
	}

	// Ʈ���� �ߵ� �Լ� 
	auto _EventCallBack = [WaveMonster, SpawnAfterEvent]()
	{
		for (auto& WpMonster : WaveMonster)
		{
			if (auto SpMonster = WpMonster.lock();
				SpMonster)
			{
				// ���� Ȱ��ȭ 
				SpMonster->SetActive(true);
			}
		}

		// ���� Ȱ��ȭ �ϰ� �� ���� �̺�Ʈ�� �Ѱܹ޾Ҵٸ� �̺�Ʈ ȣ�� . 
		if (SpawnAfterEvent)
		{
			SpawnAfterEvent();
		}
	};

	// ���̺� ���������� �̺�Ʈ ��� . 
	this->_WaveEndEvent = WaveEndEvent;

	// ������ ������ ����ǹǷ� �ش� �Լ��� ó������ 
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
		_Transform->SetRotation(TriggerRotation);
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
		_Transform->SetRotation(TriggerRotation);
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
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	if (auto _Transform = GetComponent<Transform>().lock();
		_Transform)
	{
		_Transform->SetPosition(TriggerLocation);
		_Transform->SetRotation(TriggerRotation);

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
		{"ColliderTrigger" ,
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
		_Transform->SetRotation(TriggerRotation);

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
		_Transform->SetRotation(TriggerRotation);

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

		// ���� ���� ����Ǿ��ٸ� ���̺� ����� true �̹Ƿ� ȣ�� 
		if(bWaveEnd)
		{
			if (_WaveEndEvent)
			{
				_WaveEndEvent();
			}
			// null�� �ʱ�ȭ�ؼ� ���������� ���� ��Ÿ���� üũ . 
			CheckIfTheDead = std::nullopt;
		}
	}

	if (g_bEditMode)
	{
		if (Input::GetKeyDown(DIK_DELETE))
		{
			if (_Option == Option::MonsterWave && bAfterEvent)
			{
				if (CheckIfTheDead.has_value())
				{
					for (auto& WpMonster : CheckIfTheDead.value())
					{
						if (auto SpMonter = WpMonster.lock();
							SpMonter)
						{
							SpMonter->Set_Hp(0);
						}
					}
				}
			}
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
		ImGui::Text("Location : %2.6f %2.6f %2.6f", TriggerLocation.x, TriggerLocation.y, TriggerLocation.z);

		Vector3 InputPosition = TriggerLocation;
		if (ImGui::InputFloat3("In Position", InputPosition))
		{
			TriggerLocation = InputPosition;
		}
		static float PositionSensitivy = 0.05f;
		ImGui::InputFloat("PositionSensitivy", &PositionSensitivy);
		Vector3 Position{ 0,0,0 };
		if (ImGui::SliderFloat3("Position", Position, -10.f, +10.f))
		{
			TriggerLocation  =(TriggerLocation + Position * PositionSensitivy);
		}

		ImGui::Text("Rotation : %2.6f %2.6f %2.6f", TriggerRotation.x, TriggerRotation.y, TriggerRotation.z);

		Vector3 InputRotation = TriggerRotation;
		if (ImGui::InputFloat3("In Rotation", InputRotation))
		{
			TriggerRotation = InputRotation;
		}
		static float RotationSensitivy = 0.05f;
		ImGui::InputFloat("RotationSensitivy", &RotationSensitivy);
		Vector3 Rotation{ 0,0,0 };
		if (ImGui::SliderFloat3("Rotation", Rotation, -1.f, +1.f))
		{
			TriggerRotation = (TriggerRotation + Rotation * RotationSensitivy);
		}


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
			ImGui::Text("Box Size %2.6f %2.6f %2.6f", 
				CurrentBoxColliderSize.x, CurrentBoxColliderSize.y, CurrentBoxColliderSize.z);

			static float BoxSensitivy = 0.000001f;
			ImGui::InputFloat("BoxSensitivy", &BoxSensitivy);
			Vector3 BoxSize{ 0,0,0 };
			if (ImGui::SliderFloat3("BoxSize", BoxSize, -1.f, +1.f))
			{
				_Collider->SetSize(_Collider->GetSize()+ BoxSize);
			}
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
	// �±װ� ��ġ���� �ʴٸ� ���� 
	if (_TargetTag != _pOther.lock()->m_nTag)
		return;

	// �±� ��ġ  && Ȱ��ȭ �̰� �̺�Ʈ ��ϵǾ��ٸ� ȣ�� �ϰ� AfterEvent �� �̺�Ʈ�� ȣ��Ǿ��ٰ� ǥ�� . 
	if (IsEnable())
	{
		if (_Event)
		{
			_Event();
			bAfterEvent = true;
		}

		// ȣ���ϰ� Ʈ���� ��Ȱ��ȭ .
		TriggerDisable();
	}
}


void Trigger::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{

}



