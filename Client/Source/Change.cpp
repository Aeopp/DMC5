#include "stdafx.h"
#include "..\Header\Change.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"
#include "FLight.h"

Change::Change()
{};

void Change::Free()
{
	GameObject::Free();
};

std::string Change::GetName()
{
	return "Change";
};

Change* Change::Create()
{
	return new Change{};
};

void Change::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
const float _Scale=		
	FMath::Lerp(StartScale, EndScale, T / PlayTime);

	const Vector3 Scale = Vector3{ _Scale,_Scale ,_Scale };
			_SpTransform->SetScale(Scale);
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (Inner)
		{
			const uint32  Numsubset = Inner->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = Inner->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
};

void Change::RenderInit()
{
	m_nTag = Eff_Change;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{
			"Change",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}

	};

	RenderInterface::Initialize(_InitRenderProp);

	// 메시
	{
		Mesh::InitializeInfo _Info{};
		_Info.bLocalVertexLocationsStorage = true;
		Inner = Resources::Load<StaticMesh>(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\nsg.fbx" , _Info);
	}

	// 텍스쳐 

	TrailMap = Resources::Load<Texture>(
		"..\\..\\Usable\\tex_03_common_000_0002_alpg.tga");
	EmssiveMskMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	NoiseMap = Resources::Load<Texture>(
		"..\\..\\Usable\\mesh_03_cs_noise_00_01.tga");
	_ShockWave = AddGameObject<ShockWave>();
	PushEditEntity(Inner.get());
	PushEditEntity(TrailMap.get());
	PushEditEntity(EmssiveMskMap.get());
	PushEditEntity(NoiseMap.get());
};

void Change::PlayStart(
	const std::optional<Vector3>& Location,
	const float PlayTime )
{
	Vector3 MyLocation = { 0,0,0 };

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		MyLocation = SpTransform->GetPosition();

		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			SpTransform->SetPosition(Location.value()); 
			SpTransform->SetScale({ StartScale ,StartScale ,StartScale });

			// 재생 ..
		}
	}


	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (auto SpShockWave = _ShockWave.lock();
			SpShockWave)
		{
			// SpShockWave->PlayStart(SpTransform->GetPosition(), ShockWave::Option::Change);
		}

		if (auto _PtLight = PtLight.lock();
			_PtLight)
		{
			_PtLight->bEnable = false;
		}

		if (PtLight = Renderer::GetInstance()->RefRemainingDynamicLight();
			PtLight.expired() == false)
		{
			auto SpPtLight = PtLight.lock();
			SpPtLight->SetPosition
			(FMath::ConvertVector4(
				SpTransform->GetPosition(),
				1.f
			));
			SpPtLight->bEnable = true;
			SpPtLight->Color = D3DXCOLOR(
				1.f, 0.f, 0.f, 1.f);
			SpPtLight->PointRadius = PtLightRadius;
			SpPtLight->lightFlux = 0.0f;
		}
	}


	CurParticleTime = ParticleCycle;
	this->PlayTime = PlayTime;
	_RenderProperty.bRender = true;
	T = 0.0f;

	
};



void Change::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;

	if (auto _PtLight = PtLight.lock();
		_PtLight)
	{
		_PtLight->bEnable = false;
	}
};

void Change::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
	_Info.Fx->SetFloat("EmissiveIntencity", EmissiveIntencity);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);

	const float AlphaFactor = std::clamp(PlayTime - T, 0.0f, 1.f);

	_Info.Fx->SetFloat("AlphaFactor", AlphaFactor);
	_Info.Fx->SetFloatArray("NoiseScale", NoiseScale, 3u);
	const Vector3 Speed = NoiseScrollSpeed * T;
	_Info.Fx->SetFloatArray("NoiseScrollSpeed", Speed, 3u);

	_Info.Fx->SetFloatArray("NoiseDistortion0", NoiseDistortion0, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion1", NoiseDistortion1, 2u);
	_Info.Fx->SetFloatArray("NoiseDistortion2", NoiseDistortion2, 2u);

	_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());
	_Info.Fx->SetFloat("VelocityBlurIntencity", VelocityBlurIntencity);
	_Info.Fx->SetFloat("VelocityBlurAlpha", VelocityBlurAlpha);

	_Info.Fx->SetVector("_Color", &_Color);
	
	_Info.Fx->SetTexture("TrailMap", TrailMap->GetTexture());
	_Info.Fx->SetTexture("EmissiveMskMap", EmssiveMskMap->GetTexture());

	{
		const uint32 Numsubset = Inner->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Inner->GetSubset(i).lock();
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
};



void Change::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	{
		const uint32 Numsubset = Inner->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Inner->GetSubset(i).lock();
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
};


HRESULT Change::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale( Vector3{ 0.005f,0.005f,0.005f } );
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT Change::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 0.005f,0.005f,0.005f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT Change::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Change::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
	}

	CurParticleTime -= _fDeltaTime;

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		const Vector3 CurRotation = Vector3{ 0.f,RollRotationSpeed * _fDeltaTime , 0.f };
		SpTransform->Rotate(CurRotation);

		if (CurParticleTime < 0.0f)
		{
			CurParticleTime += ParticleCycle;

			const Matrix Mat = SpTransform->GetRenderMatrix();
			const uint32 RangeEnd = Inner->m_spVertexLocations->size() - 1u;

			{
				auto _PlayableParticle = ParticleSystem::GetInstance()->
					PlayParticle("ChangeParticle",333u, true);
				for (int32 i = 0; i < _PlayableParticle.size(); 
					++i)
				{
					auto& _PlayInstance = _PlayableParticle[i];
					_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
				}
			}
		}


		if (PtLight.expired() == false)
		{
			auto SpPtLight = PtLight.lock();
			SpPtLight->SetPosition
			(FMath::ConvertVector4(
				SpTransform->GetPosition(),
				1.f
			));
			SpPtLight->Color = D3DXCOLOR(
				1.f, 0.f, 0.f, 1.f);
			SpPtLight->PointRadius = PtLightRadius;
			
			;

			SpPtLight->lightFlux =
				 ( std::sinf( (T / PlayTime) * FMath::PI)  ) * PtLightFlux;
		}

	}

	


	return 0;
}

UINT Change::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void Change::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				Vector3 Point{ 0.f,0.f ,0.f };
				if (auto SpTransform = m_pTransform.lock();
					SpTransform)
				{
					Point = SpTransform->GetPosition();
				};

				PlayStart(Point,EditPlayTime);
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}

			ImGui::ColorEdit4("Color", _Color);

			ImGui::SliderFloat("StartScale", &StartScale, 0.0f, 0.1f);
			ImGui::SliderFloat("EndScale", &EndScale, 0.0f, 0.1f);
			
			ImGui::SliderFloat("PtLightFlux", &PtLightFlux, 0.0f, 100.f);
			;

			;

			ImGui::SliderFloat3("NoiseScrollSpeed", NoiseScrollSpeed, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScrollSpeed", NoiseScrollSpeed, "%9.6f");

			ImGui::SliderFloat3("NoiseScale", NoiseScale, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScale", NoiseScale, "%9.6f");

			ImGui::SliderFloat("VelocityBlurIntencity", &VelocityBlurIntencity, 0.f, 1.f);
			ImGui::SliderFloat("VelocityBlurAlpha", &VelocityBlurAlpha, 0.f, 1.f);
			;

			ImGui::SliderFloat("EditPlayTime", &EditPlayTime, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In EditPlayTime", &EditPlayTime, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("EditPlayRollRotateSpeed", &EditRotationSpeed, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EditPlayRollRotateSpeed", &EditRotationSpeed,0.f,0.f ,"%9.6f");

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("EmissiveIntencity", &EmissiveIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In EmissiveIntencity", &EmissiveIntencity, 0.f,0.f,"%9.6f");

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 0.f, "%9.6f");

		}
		ImGui::EndChild();
	}
};

void Change::OnEnable()
{
	GameObject::OnEnable();
};

void Change::OnDisable()
{
	GameObject::OnDisable();
};



