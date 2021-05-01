#include "stdafx.h"
#include "..\Header\AirHike.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>

void AirHike::Free()
{
	GameObject::Free();
};

std::string AirHike::GetName()
{
	return "AirHike";
};

AirHike* AirHike::Create()
{
	return new AirHike{};
};

void AirHike::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		
			const float CurScale = FMath::Lerp(StartScale, FinalScale, Sin);
			if (auto SpTransform = GetComponent<Transform>().lock();
				SpTransform)
			{
				SpTransform->SetScale({ CurScale ,CurScale ,CurScale });
				SpTransform->SetRotation({ 0,0,0 });
				_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
				if (_StaticMesh)
				{
					const uint32  Numsubset = _StaticMesh->GetNumSubset();
					_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

					for (uint32 i = 0; i < Numsubset; ++i)
					{
						const auto& _Subset = _StaticMesh->GetSubset(i);
						const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

						_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, CurScale);
					}
				}
			}
		
	}
};

void AirHike::RenderInit()
{
	m_nTag = Player;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ���� ) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;
	
	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
		}
	} };

	/// <summary>
	/// DrawCollider
	/// </summary>
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };


	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		
		{"AirHike",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	// 
	// ����ƽ �޽� �ε�
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_StaticMesh = Resources::Load<ENGINE::StaticMesh>
			(L"..\\..\\Resource\\Mesh\\Static\\Effect\\AirHike\\AirHike.fbx" , _InitInfo);
	
	PushEditEntity(_StaticMesh.get());
};



void AirHike::PlayStart(const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	
	T = 0.0f;
	_RenderProperty.bRender = true;
};

void AirHike::PlayEnd()
{

	_RenderProperty.bRender = false;
		T = 0.0f;

};

void AirHike::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	
	const Matrix World = _RenderUpdateInfo.World;
	const float CurIntencity = FMath::Lerp(StartIntencity, FinalIntencity, Sin);
	const Vector4 CurColor = FMath::Lerp(StartColor, FinalColor, Sin);
	_Info.Fx->SetMatrix("matWorld", &World);
	_Info.Fx->SetVector("CurColor", &CurColor);
	_Info.Fx->SetFloat("Intencity", CurIntencity);

	const uint32 Numsubset = _StaticMesh->GetNumSubset();

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};
}


void AirHike::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false ==
				_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT AirHike::Ready()
{
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01,0.01,0.01 });
	InitTransform.lock()->SetPosition(Vector3{0.f,0.11544f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT AirHike::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetPosition(Vector3{/* -12.f,-0.9f,-638.f*/0.f,0.5f,0.5f });
	return S_OK;
}

HRESULT AirHike::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT AirHike::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime * Speed;
	Sin = std::sinf(T);

	// ���� ���� .
	if (T >= FMath::PI / 2.f)
	{
		PlayEnd();
	};

	return 0;
}

UINT AirHike::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void AirHike::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		if (ImGui::SmallButton("Play"))
		{
			PlayStart();
		}

		ImGui::Text("T : %2.6f", T);
		ImGui::SliderFloat("Speed", &Speed, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
	
		ImGui::SliderFloat("StartIntencity", &StartIntencity, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("StartScale", &StartScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit4("StartColor", StartColor);

		ImGui::SliderFloat("FinalIntencity", &FinalIntencity, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("FinalScale", &FinalScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit4("FinalColor", FinalColor);     
	}
}


void AirHike::OnEnable()
{
	GameObject::OnEnable();
}

void AirHike::OnDisable()
{
	GameObject::OnDisable();
}

