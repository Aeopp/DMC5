#include "stdafx.h"
#include "..\Header\ElectricVortex.h"
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

ElectricVortex::ElectricVortex()
{

};

void ElectricVortex::Free()
{
	GameObject::Free();
};

std::string ElectricVortex::GetName()
{
	return "ElectricVortex";
};

ElectricVortex* ElectricVortex::Create()
{
	return new ElectricVortex{};
};

void ElectricVortex::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		/*const Vector3 Scale = FMath::Lerp(StartScale, FinalSacle, T / PlayTime);
		_SpTransform->SetScale(Scale);*/
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		//if (Mesh)
		//{
		//	const uint32 Numsubset = Mesh->GetNumSubset();
		//	_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

		//	for (uint32 i = 0; i < Numsubset; ++i)
		//	{
		//		const auto& _Subset = Mesh->GetSubset(i);
		//		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		//		_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
		//	}
		//}
	}
};

void ElectricVortex::RenderInit()
{
	m_nTag = Eff_ThunderBolt;
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
		{"ElectricVortex",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
	//magic_eletric00.fbx
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	Mesh = Resources::Load<StaticMesh>("..\\..\\Usable\\Electric\\magic_eletric02.fbx", _InitInfo);
	GradMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\Grad.png");

	DistortionMap = Resources::Load<Texture>("..\\..\\Usable\\smoke_a_im.tga");
};

void ElectricVortex::PlayStart(const Vector3& PlayLocation)
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
};

void ElectricVortex::PlayEnd()
{
	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = false;
	}

	_RenderProperty.bRender = false;
	T = 0.0f;

};

void ElectricVortex::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity * std::fabsf(std::sin(T*ScrollSpeed)) );
	_Info.Fx->SetFloat("Time", T);
	_Info.Fx->SetTexture("GradMap", GradMap->GetTexture());
	_Info.Fx->SetFloat("ScrollSpeed", ScrollSpeed);

	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetTexture("DistortionMap", DistortionMap->GetTexture());

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
			if (auto SpSubset = Mesh->GetSubset(CurSubset % 4u).lock();
				SpSubset)
			{
				SpSubset->BindProperty(TextureType::DIFFUSE, 0u, "AlbmMap", _Info.Fx);
				const float RandY = FMath::Random(0.0f, 3.f);
				_Info.Fx->SetFloat("SpriteYStart", RandY/4.f);
				_Info.Fx->SetFloat("SpriteYEnd", (RandY + 1.f )/ 4.f);
				SpSubset->Render(_Info.Fx);
			};
		}
		else if (_Mode == Mode::All)
		{
			for ( uint32 j = 0; j < Mesh->GetNumSubset(); ++j)
			{
				if (auto SpSubset = Mesh->GetSubset(j).lock();
					SpSubset)
				{
					SpSubset->BindProperty(TextureType::DIFFUSE, 0u, "AlbmMap", _Info.Fx);
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
				SpSubset->BindProperty(TextureType::DIFFUSE, 0u, "AlbmMap", _Info.Fx);
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
						SpSubset->BindProperty(TextureType::DIFFUSE, 0u, "AlbmMap", _Info.Fx);
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

void ElectricVortex::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"ElectricVortexParticle", 1000ul,true);
			_Particle.empty() == false)
		{
			static constexpr uint32 JumpOffset = 1u;

			for (int32 i = 0; i < _Particle.size(); i += JumpOffset)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
			}
		}
	};
}



void ElectricVortex::RenderDebug(const DrawInfo& _Info)
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


HRESULT ElectricVortex::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT ElectricVortex::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.5f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 0.005f,0.005f,0.005f });
	return S_OK;
}

HRESULT ElectricVortex::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT ElectricVortex::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;
	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
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

		CurSubsetRand = FMath::Random(0u, 3u);
	}

	CurParticleTime -= _fDeltaTime;
	if (CurParticleTime < 0.0f)
	{
		CurParticleTime += ParticleTime;
		PlayParticle();
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

UINT ElectricVortex::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void ElectricVortex::Editor()
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
			}
			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, FLT_MIN, 10000.f, "%9.6f");
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("SubsetDelta", &SubsetDelta, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In SubsetDelta", &SubsetDelta, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("ScrollSpeed", &ScrollSpeed, FLT_MIN, 1000.f, "%9.6f");
			ImGui::InputFloat("In ScrollSpeed", &ScrollSpeed, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("PtLightRadius", &PtLightRadius, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In PtLightRadius", &PtLightRadius, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("PtLightFlux", &PtLightFlux, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In PtLightFlux", &PtLightFlux, 0.f, 0.f, "%9.6f");

			ImGui::SliderInt("Mode", &_Mode, 0, Mode::End-1);
			ImGui::InputInt("In Mode", &_Mode, 0, 0);
		}
		ImGui::EndChild();
	}
};

void ElectricVortex::OnEnable()
{
	GameObject::OnEnable();
};

void ElectricVortex::OnDisable()
{
	GameObject::OnDisable();
};


