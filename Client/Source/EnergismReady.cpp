#include "stdafx.h"
#include "..\Header\EnergismReady.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "NuclearLensFlare.h"

void EnergismReady::Free()
{
	GameObject::Free();
};

std::string EnergismReady::GetName()
{
	return "EnergismReady";
};

EnergismReady* EnergismReady::Create()
{
	return new EnergismReady{};
};

void EnergismReady::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const float LerpT = FMath::Clamp(T / GrowTime, 0.0f, 1.f);
		const float CurScale = FMath::Lerp(0.0f, GrowEndScale, LerpT);

		_SpTransform->SetScale(
			Vector3{ CurScale ,CurScale ,CurScale });

		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void EnergismReady::RenderInit()
{
	m_nTag = Eff_EnergismReady;
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

		{"EnergismReady",
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
		(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\sphere02.fbx", _InitInfo);

	_LightMsk = Resources::Load<ENGINE::Texture>(
		L"..\\..\\Usable\\Artemis\\Water.tga"
		);

	_Noise = Resources::Load<ENGINE::Texture>(
		L"..\\..\\Usable\\Artemis\\1.tga"
		);

	_AlbmMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png"
		);

	_ShockWave = AddGameObject<ShockWave>();
	PushEditEntity(_Mesh.get());
	_NuclearLensFlare = AddGameObject<NuclearLensFlare>();

	_DynamicLight.Color = 
	{
		ColorLow, 
		ColorHigh
	};

	_DynamicLight.Flux = 
	{
		FluxLow,FluxHigh
	};

	_DynamicLight.PointRadius=
	{
		RadiusLow,RadiusHigh
	};
};

void EnergismReady::PlayStart(
	const Vector3& Location,  const float GroundY, const bool bEditPlay)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location);
			SpTransform->SetScale(Vector3{ 0.f,0.f,0.f });
			_NuclearLensFlare.lock()->
				PlayStart(Location, PlayTime);
			_NuclearLensFlare.lock()->ScaleEnd = 0.008f;
			_NuclearLensFlare.lock()->ColorIntencity = 0.0018f;

			_DynamicLight.PlayStart(Location, 
				PlayTime);
		}
	}

	T = 0.0f;
	_RenderProperty.bRender = true;
	CurParticleDelta = 0.0f;

	PlayParticle();
};


void EnergismReady::PlayEnd()
{
	_DynamicLight.PlayEnd();
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void EnergismReady::UpdatePosition(const Vector3& Location)
{
	GetComponent<Transform>().lock()->SetPosition(Location);
};


void EnergismReady::ParticleUpdate(const float DeltaTime)
{
	CurParticleDelta -= DeltaTime;
	if (CurParticleDelta < 0.0f)
	{
		CurParticleDelta += ParticleDelta;
		PlayParticle();
	}
};

void EnergismReady::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"EnergismReadyParticle", 16u, true);
			_Particle.empty() == false)
		{
			const Matrix ParticleWorld = FMath::Scale(Vector3{ ParticleWorldScale ,ParticleWorldScale ,ParticleWorldScale })
				*
				SpTransform->GetRotationMatrix()
				*
				FMath::Translation(SpTransform->GetPosition());

			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(ParticleWorld);
			}
		}
	};
};

void EnergismReady::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("LightMskMap", _LightMsk->GetTexture());
		_Info.Fx->SetTexture("AlbmMap", _AlbmMap->GetTexture());
		_Info.Fx->SetTexture("NoiseMap", _Noise->GetTexture());
		
		const float LerpT = 
			FMath::Clamp(T / GrowTime, 0.0f, 1.f);

		const float CurColor =
			FMath::Lerp(StartColorIntencity, EndColorIntencity, LerpT);

		_Info.Fx->SetFloat("Time", T * NoiseTimeCorr);
		_Info.Fx->SetFloat("ColorIntencity", CurColor);
		_Info.Fx->SetFloat("AlphaFactor", LerpT);
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


void EnergismReady::RenderDebug(const DrawInfo& _Info)
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


HRESULT EnergismReady::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT EnergismReady::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.0020f,0.0020f,0.0020f });
	m_pTransform.lock()->SetPosition(Vector3{ -37.411f,0.821f,30.663f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });

	return S_OK;
}

HRESULT EnergismReady::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT EnergismReady::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
	}


	const float LerpT = FMath::Clamp(T / GrowTime, 0.f, 1.f);

	ParticleUpdate(_fDeltaTime);
	_NuclearLensFlare.lock()->
		UpdatePlayVariable(LerpT);
	
	if (T <= GrowTime)
	{
		const float LerpT = T / GrowTime;
		const float Radius = FMath::Lerp(RadiusLow, RadiusHigh, LerpT);
		const float Flux=FMath::Lerp(FluxLow, FluxHigh, LerpT);
		const D3DXCOLOR _Color = FMath::ToColor(FMath::Lerp(ColorLow,ColorHigh ,LerpT));
		_DynamicLight.Update(_Color, Radius, Flux, 
			GetComponent<Transform>().lock()->GetPosition());



		CurShockWaveDelta -= _fDeltaTime;
		if (CurShockWaveDelta < 0.0f)
		{
			CurShockWaveDelta += ShockWaveDelta;
			_ShockWave.lock()->PlayStart(GetComponent<Transform>().lock()->GetPosition(), ShockWave::Option::EnergismReady);
		}
	}
	else if (T > GrowTime)
	{
		_DynamicLight.Update(LaunchColor, LaunchRadius, LaunchFlux,
			GetComponent<Transform>().lock()->GetPosition());
	}

	return 0;
}


UINT EnergismReady::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void EnergismReady::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		if (ImGui::SmallButton("Play"))
		{
			PlayStart(
				GetComponent<Transform>().lock()->GetPosition(), -1.45f, true);
		}

		ImGui::Text("T : %2.6f", T);
		ImGui::SliderFloat("NoiseTimeCorr", &NoiseTimeCorr, 0.0f, 100.f);

		D3DXCOLOR LaunchColor{
	194.f / 255.f, 181.f / 255.f, 247.f / 255.f ,1.f };
		float LaunchRadius{ 5.f };
		float LaunchFlux{ 2.2f };


		ImGui::SliderFloat("FluxLow", &FluxLow, 0.0f, 10.f);
		ImGui::SliderFloat("FluxHigh", &FluxHigh, 0.0f, 10.f);
		ImGui::SliderFloat("RadiusLow", &RadiusLow, 0.0f, 10.f);
		ImGui::SliderFloat("RadiusHigh", &RadiusHigh, 0.0f, 10.f);

		ImGui::SliderFloat("ShockWaveDelta", &ShockWaveDelta, 0.0f, 1.f);

		ImGui::SliderFloat("ParticleDelta", &ParticleDelta, 0.0f, 1.f);
		ImGui::SliderFloat("ParticleWorldScale", &ParticleWorldScale, 0.0f, 1.f);

		ImGui::SliderFloat("PlayTime", &PlayTime, 0.0f, 10.f);
		ImGui::SliderFloat("GrowTime", &GrowTime, 0.0f, 10.f);

		ImGui::SliderFloat("StartColorIntencity", &StartColorIntencity, 0.0f, 10.f);
		ImGui::SliderFloat("EndColorIntencity", &EndColorIntencity, 0.0f, 10.f);

		ImGui::InputFloat("GrowEndScale", &GrowEndScale);

		ImGui::EndChild();
	}
};

void EnergismReady::OnEnable()
{
	GameObject::OnEnable();
};

void EnergismReady::OnDisable()
{
	GameObject::OnDisable();
};


