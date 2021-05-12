#include "stdafx.h"
#include "..\Header\ThunderBoltSecond.h"
#include "Transform.h"
#include "Subset.h"
#include "Color.h"

#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "FLight.h"

ThunderBoltSecond::ThunderBoltSecond()
{

};

void ThunderBoltSecond::Free()
{
	GameObject::Free();
};

std::string ThunderBoltSecond::GetName()
{
	return "ThunderBoltSecond";
};

ThunderBoltSecond* ThunderBoltSecond::Create()
{
	return new ThunderBoltSecond{};
};

void ThunderBoltSecond::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = (
			FMath::Scale(_SpTransform->GetScale()) *
			Renderer::GetInstance()->_RenderInfo.Billboard *
			FMath::Translation(_SpTransform->GetPosition()));
	}
};

void ThunderBoltSecond::RenderInit()
{
	m_nTag = Eff_ThunderBoltSecond;
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
		{"ThunderBoltSecond",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
	//magic_eletric00.fbx
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	Mesh = Resources::Load<StaticMesh>("..\\..\\Usable\\Electric\\Spark\\parts.fbx", _InitInfo);
	GradMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");
	DistortionMap =Resources::Load<Texture>("..\\..\\Usable\\smoke_a_im.tga");
	AlbMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\lightning.dds");
};

void ThunderBoltSecond::PlayStart(const Vector3& PlayLocation)
{
	PlayEnd();

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(PlayLocation);
	};

	T = 0.0f;
	_RenderProperty.bRender = true;
	CurParticleTime = 0.0f;
	CurSubset = 0u;

	PtLight = Renderer::GetInstance()->RefRemainingDynamicLight();

	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = true;
	}

	CurSubsetDelta = SubsetDelta;

	Range = 0.0f;
	EndRange = 0.35f;
};

void ThunderBoltSecond::PlayEnd()
{
	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = false;
	}

	_RenderProperty.bRender = false;
	T = 0.0f;

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"ThunderBoltSecondEndParticle", 250u, true);
			_Particle.empty() == false)
		{
			
			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
			}
		}
	};
};

void ThunderBoltSecond::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity * std::fabsf(std::sin(T*ScrollSpeed)) );
	_Info.Fx->SetFloat("Time", T);
	_Info.Fx->SetTexture("GradMap", GradMap->GetTexture());
	_Info.Fx->SetFloat("ScrollSpeed", ScrollSpeed);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);

	_Info.Fx->SetFloat("UVYStartConstant", UVYStartConstant);
	_Info.Fx->SetFloat("UVYScrollSpeed", UVYScrollSpeed);

	_Info.Fx->SetFloat("Range", Range);
	_Info.Fx->SetFloat("EndRange", EndRange);

	_Info.Fx->SetTexture("DistortionMap", DistortionMap->GetTexture());
	_Info.Fx->SetTexture("AlbmMap", AlbMap->GetTexture());

	const float PlayTimehalf = PlayTime * 0.5f;
	if (T >= PlayTimehalf)
	{
		_Info.Fx->SetFloat("AlphaFactor", 1.0f - ((T-PlayTimehalf) / PlayTimehalf));
		
	}
	else
	{
		_Info.Fx->SetFloat("AlphaFactor", 1.0f);
	}

	{
		if (_Mode == Mode::Loop)
		{
			if (auto SpSubset = Mesh->GetSubset(CurSubset % 6u).lock();
				SpSubset)
			{
				const float RandY = FMath::Random(0.0f, 3.f);
				_Info.Fx->SetFloat("SpriteYStart", RandY / 4.f);
				_Info.Fx->SetFloat("SpriteYEnd", (RandY + 1.f) / 4.f);
				SpSubset->Render(_Info.Fx);
			};
		}
		else if (_Mode == Mode::All)
		{
			for (uint32 j = 0; j < Mesh->GetNumSubset(); ++j)
			{
				if (auto SpSubset = Mesh->GetSubset(j).lock();
					SpSubset)
				{
					SpSubset->BindProperty(TextureType::DIFFUSE, 0u, "", _Info.Fx);
					const float RandY = FMath::Random(0.0f, 3.f);
					_Info.Fx->SetFloat("SpriteYStart", RandY / 4.f);
					_Info.Fx->SetFloat("SpriteYEnd", (RandY + 1.f) / 4.f);

					SpSubset->Render(_Info.Fx);
				};
			}

		}
		else if (_Mode == Mode::Rand)
		{
			if (auto SpSubset = Mesh->GetSubset(CurSubsetRand).lock();
				SpSubset)
			{
				const float RandY = FMath::Random(0.0f, 3.f);
				_Info.Fx->SetFloat("SpriteYStart", RandY / 4.f);
				_Info.Fx->SetFloat("SpriteYEnd", (RandY + 1.f) / 4.f);

				SpSubset->Render(_Info.Fx);
			};
		}
		else if (_Mode == Mode::Set)
		{
			for (uint32 i = 0; i < _SubsetSets.size(); ++i)
			{
				if (_SubsetSets[i])
				{
					if (auto SpSubset = Mesh->GetSubset(i).lock();
						SpSubset)
					{
						const float RandY = FMath::Random(0.0f, 3.f);
						_Info.Fx->SetFloat("SpriteYStart", RandY / 4.f);
						_Info.Fx->SetFloat("SpriteYEnd", (RandY + 1.f) / 4.f);

						SpSubset->Render(_Info.Fx);
					};
				}
			}

		}
	}
};

void ThunderBoltSecond::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"ThunderBoltSecondParticle", 1000ul, true);
			_Particle.empty() == false)
		{
			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
			}
		}
	};
}



void ThunderBoltSecond::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	{
		const uint32 Numsubset = Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Mesh->GetSubset(i).lock();
				SpSubset)
			{
				SpSubset->Render(_Info.Fx);
			};
		};
	}

};


HRESULT ThunderBoltSecond::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();

	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT ThunderBoltSecond::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.0f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 0.05f,0.05f,0.05f});

	return S_OK;
}

HRESULT ThunderBoltSecond::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT ThunderBoltSecond::Update(const float _fDeltaTime)
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
	if (CurParticleTime < 0.0f)
	{
		CurParticleTime += ParticleTime;
		PlayParticle();
	}

	CurSubsetDelta -= _fDeltaTime;
	if (CurSubsetDelta < 0.0f)
	{
		CurSubsetDelta += SubsetDelta;
		++CurSubset;

		for (uint32 i = 0; i < _SubsetSets.size(); ++i)
		{
			_SubsetSets[i] = FMath::Random(0u, 1u);
		}

		CurSubsetRand = FMath::Random(0u, 5u);
	}


	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		if (auto SpTransform = GetComponent<Transform>().lock();
			SpTransform)
		{
			SpPtLight->SetPosition(FMath::ConvertVector4(SpTransform->GetPosition(), 1.f));
			SpPtLight->Color = D3DXCOLOR(173.f / 255.f, 162.f / 255.f, 217.f / 255.f, 1.f);
			SpPtLight->PointRadius = PtLightRadius;
			SpPtLight->lightFlux = PtLightFlux * std::fabsf(std::sin(T * ScrollSpeed));
		}
		else
		{
			SpPtLight->SetPosition(Vector4{ FLT_MAX,FLT_MAX ,FLT_MAX ,1.f });
		}
	}

	return 0;
}

UINT ThunderBoltSecond::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void ThunderBoltSecond::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				if (auto SpTransform = GetComponent<Transform>().lock();
					SpTransform)
				{
					PlayStart(SpTransform->GetPosition());
				}
			}

			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			};

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("UVYScrollSpeed", &UVYScrollSpeed, FLT_MIN, 100.f, "%9.6f");
			ImGui::InputFloat("In UVYScrollSpeed", &UVYScrollSpeed, 0.f, 0.f, "%9.6f");
			
			ImGui::SliderFloat("UVYStartConstant", &UVYStartConstant, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In UVYStartConstant ", &UVYStartConstant, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("ScrollSpeed", &ScrollSpeed, FLT_MIN, 1000.f, "%9.6f");
			ImGui::InputFloat("In ScrollSpeed", &ScrollSpeed, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("PtLightRadius", &PtLightRadius, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In PtLightRadius", &PtLightRadius, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("PtLightFlux", &PtLightFlux, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In PtLightFlux", &PtLightFlux, 0.f, 0.f, "%9.6f");

			ImGui::SliderInt("Mode", &_Mode, 0, Mode::End - 1);
			ImGui::InputInt("In Mode", &_Mode, 0, 0);

		}
		ImGui::EndChild();
	}
};

void ThunderBoltSecond::OnEnable()
{
	GameObject::OnEnable();
};

void ThunderBoltSecond::OnDisable()
{
	GameObject::OnDisable();
};


