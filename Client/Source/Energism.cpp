#include "stdafx.h"
#include "..\Header\Energism.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "NuclearLensFlare.h"

void Energism::Free()
{
	GameObject::Free();
};

std::string Energism::GetName()
{
	return "Energism";
};

Energism* Energism::Create()
{
	return new Energism{};
};

void Energism::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void Energism::RenderInit()
{
	m_nTag = Eff_Energism;
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

		{"Energism",
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

	_Mesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Usable\\Artemis\\Energism.fbx", _InitInfo);


	_AlbmMap =  Resources::Load<ENGINE::Texture>(
		"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png"
		);

	_NoiseMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Usable\\Artemis\\1.tga"
		);

	PushEditEntity(_Mesh.get());

	for (auto& _Reverberation : ReverberationArr)
	{
		_Reverberation = AddGameObject<Reverberation>();
	}
};

void Energism::PlayStart(const Vector3& Location, const float Yaw)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location);
			SpTransform->SetRotation(Vector3{ 0.f,Yaw,0.f });
		}
	}

	T = 0.0f;
	_RenderProperty.bRender = true;
};

void Energism::UpdateYaw(const float Yaw)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetRotation(Vector3{ 0.f,Yaw,0.f } );
	};
} 

void Energism::PlayEnd()
{
	T = 0.0f;
	_RenderProperty.bRender = false;
};

void Energism::UpdateReverberation(const float DeltaTime)
{
	CurReverberationDelta -= DeltaTime;
	if (CurReverberationDelta < 0.0f)
	{
		CurReverberationDelta += ReverberationDelta;
		PlayReverberation();
	}
}; 

void Energism::PlayReverberation()
{
	auto SpTransform = GetComponent<Transform>().lock();
	const Vector3 Look = -FMath::Normalize(SpTransform->GetLook());
	const float OffsetScale = SpTransform->GetScale().x * ReverberationOffsetScale; 

	const Vector3 CurLocation = SpTransform->GetPosition() + FMath::Random(0.0f, OffsetScale) * Look;

	const float ReverationStartScale = FMath::Random(ReverationStartRange.first, ReverationStartRange.second); 
	const float ReverationEndScale = FMath::Random(ReverationEndRange.first, ReverationEndRange.second);

	ReverberationArr[CurReverberationIdx].lock()->PlayStart(CurLocation,Look,ReverationStartScale,ReverationEndScale);
	CurReverberationIdx = (CurReverberationIdx + 1) % ReverberationCount;
};

void Energism::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("AlbmMap", _AlbmMap->GetTexture());
		_Info.Fx->SetTexture("NoiseMap", _NoiseMap->GetTexture());
		_Info.Fx->SetFloat("NoiseFactor", NoiseFactor);
		
		_Info.Fx->SetFloat("Time",  T*TimeCorr);
		_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
		_Info.Fx->SetFloat("CurveScale", CurveScale);
	}

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset =
			_Mesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}


void Energism::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = _Mesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT Energism::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT Energism::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.03f,0.03f,0.03f });
	m_pTransform.lock()->SetPosition(Vector3{ -37.411f,0.821f,30.663f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });

	return S_OK;
}

HRESULT Energism::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Energism::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
	};

	UpdateReverberation(_fDeltaTime);

	return 0;
}


UINT Energism::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void Energism::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());

		static float EditPlayYaw = 0.0f;
		ImGui::SliderFloat("EditPlayYaw", &EditPlayYaw, -360.f, 360.f);

		if (ImGui::SmallButton("Play"))
		{
			PlayStart(GetComponent<Transform>().lock()->GetPosition() ,EditPlayYaw);
		}

		ImGui::Text("T : %2.6f", T);

		ImGui::SliderFloat("ReverberationOffsetScale", &ReverberationOffsetScale, 0.0f, 10000.f);

		ImGui::SliderFloat("ReverationStartRange First", &ReverationStartRange.first, 0.0f, 1.f);
		ImGui::SliderFloat("ReverationStartRange Second", &ReverationStartRange.second, 0.0f, 1.f);

		

		;

		ImGui::SliderFloat("ReverationEndScale First", &ReverationEndRange.first, 0.0f, 1.f);
		ImGui::SliderFloat("ReverationEndScale Second", &ReverationEndRange.second, 0.0f, 1.f);

		ImGui::SliderFloat("TimeCorr", &TimeCorr, 0.0f, 10.f);
		ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 
			0.0f, 10.f);
		ImGui::SliderFloat("CurveScale", &CurveScale,
			0.0f, 100.f);

		ImGui::SliderFloat("NoiseFactor", &NoiseFactor,
			0.0f, 100.f);

		ImGui::EndChild();
	}
}


void Energism::OnEnable()
{
	GameObject::OnEnable();
}

void Energism::OnDisable()
{
	GameObject::OnDisable();
}



