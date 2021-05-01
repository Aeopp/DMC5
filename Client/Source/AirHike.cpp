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
	m_nTag  = Eff_AirHike;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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
	// 스태틱 메쉬 로딩
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_StaticMesh = Resources::Load<ENGINE::StaticMesh>
			(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx" , _InitInfo);

	_MagicTexture =
		Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\MagicTexture.tga");

	_MagicAlb = 
		Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_03_decal_pl_0120_0000_alb.tga");
	_MagicMsk= 
		Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_03_decal_pl0120_0000_msk2.tga");



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
	const uint32 Numsubset = _StaticMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetVector("CurColor", &CurColor);
		_Info.Fx->SetFloat("Intencity", CurIntencity);
		_Info.Fx->SetTexture("MagicMap", _MagicTexture->GetTexture());
		_Info.Fx->SetTexture("MagicMsk", _MagicMsk->GetTexture());
		_Info.Fx->SetTexture("MagicAlb", _MagicAlb->GetTexture());
	}

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
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01,0.01,0.01 });
	InitTransform.lock()->SetPosition(Vector3{0.f,0.11544f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
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

	// 끝날 쯔음 .
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
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str()); 
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
		ImGui::EndChild();
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

