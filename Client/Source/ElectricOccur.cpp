#include "stdafx.h"
#include "..\Header\ElectricOccur.h"
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

ElectricOccur::ElectricOccur()
{

};

void ElectricOccur::Free()
{
	GameObject::Free();
};

std::string ElectricOccur::GetName()
{
	return "ElectricOccur";
};

ElectricOccur* ElectricOccur::Create()
{
	return new ElectricOccur{};
};

void ElectricOccur::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		/*const Vector3 Scale = FMath::Lerp(StartScale, FinalSacle, T / PlayTime);
		_SpTransform->SetScale(Scale);*/
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		/*if (Mesh)
		{
			const uint32 Numsubset = Mesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = Mesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}*/
	}
};

void ElectricOccur::RenderInit()
{
	m_nTag = Eff_ElectricOccur;
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
		{"ElectricOccur",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
	//magic_eletric00.fbx
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	Mesh = Resources::Load<StaticMesh>("..\\..\\Usable\\Electric\\magic_eletric00.fbx", _InitInfo);
	GradMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\Grad.png");
	DistortionMap = Resources::Load<Texture>("..\\..\\Usable\\smoke_a_im.tga");
};

void ElectricOccur::PlayStart(const Vector3& PlayLocation)
{
	PlayEnd();

	// SetActive(true);

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(PlayLocation);
	};

	T = 0.0f;
	_RenderProperty.bRender = true;
	CurParticleTime = 0.0f;

	PtLight = Renderer::GetInstance()->RefRemainingDynamicLight();

	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = true;
	}
};

void ElectricOccur::PlayEnd()
{
	// SetActive(false);

	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = false;
	}

	_RenderProperty.bRender = false;
	T = 0.0f;
}
float ElectricOccur::GetPlayTime()
{
	return PlayTime;
}
;

void ElectricOccur::RenderAlphaBlendEffect(const DrawInfo& _Info)
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
		const uint32 Numsubset = Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Mesh->GetSubset(i).lock();
				SpSubset)
			{
				SpSubset->BindProperty(TextureType::DIFFUSE, 0u, "AlbmMap", _Info.Fx);
				SpSubset->Render(_Info.Fx);
			};
		};
	}

};

void ElectricOccur::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"ElectricParticle", 1000ul,true);
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



void ElectricOccur::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	{
		const uint32 Numsubset = Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Mesh->GetSubset(i).lock();
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


HRESULT ElectricOccur::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT ElectricOccur::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.5f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 0.0001f,0.0001f,0.0001f });

	return S_OK;
}

HRESULT ElectricOccur::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT ElectricOccur::Update(const float _fDeltaTime)
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

UINT ElectricOccur::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void ElectricOccur::Editor()
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

			ImGui::SliderFloat("ScrollSpeed", &ScrollSpeed, FLT_MIN, 1000.f, "%9.6f");
			ImGui::InputFloat("In ScrollSpeed", &ScrollSpeed, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("PtLightRadius", &PtLightRadius, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In PtLightRadius", &PtLightRadius, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("PtLightFlux", &PtLightFlux, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In PtLightFlux", &PtLightFlux, 0.f, 0.f, "%9.6f");
		}
		ImGui::EndChild();
	}
};

void ElectricOccur::OnEnable()
{
	GameObject::OnEnable();
};

void ElectricOccur::OnDisable()
{
	GameObject::OnDisable();
};


