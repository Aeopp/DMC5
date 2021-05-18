#include "stdafx.h"
#include "..\Header\ThunderBolt.h"
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

ThunderBolt::ThunderBolt()
{

};

void ThunderBolt::Free()
{
	GameObject::Free();
};

std::string ThunderBolt::GetName()
{
	return "ThunderBolt";
};

ThunderBolt* ThunderBolt::Create()
{
	return new ThunderBolt{};
};

void ThunderBolt::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = 
			FMath::Scale(_SpTransform->GetScale())
			* DirRotMatrix * 
			FMath::Translation(_SpTransform->GetPosition());
	}
};

void ThunderBolt::RenderInit()
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
		{"ThunderBont",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
	//magic_eletric00.fbx
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	Mesh = Resources::Load<StaticMesh>("..\\..\\Usable\\Electric\\magic_eletric01.fbx", _InitInfo);
	GradMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");
	DistortionMap =Resources::Load<Texture>("..\\..\\Usable\\smoke_a_im.tga");
};

void ThunderBolt::PlayStart(
	const Vector3& PlayLocation,
	Vector3 Direction,
	const float Velocity,
	const std::optional<Vector3>& PlayScale ,
	const bool bEditPlay)
{
	PlayEnd();
	Direction = FMath::Normalize(Direction);

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(PlayLocation);
		const Vector3 Up = Direction.y  > 0.f ? Vector3{ 0.f,-1.f,0.f } : Vector3{ 0.f,1.f,0.f };
		const Vector3 Axis = FMath::Cross(Direction, Up);
		const Vector3 NormalAxis = FMath::Normalize(Axis);
		const float Rad = std::asinf(FMath::Length(Axis));

		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			DirRotMatrix  = FMath::RotationAxisMatrix(NormalAxis, Rad);
		};

		if (PlayScale)
		{
			SpTransform->SetScale(*PlayScale);
		}
	};


	this->Velocity = Velocity;
	this->Direction = Direction;

	T = 0.0f;
	_RenderProperty.bRender = true;
	CurParticleTime = 0.0f;

	PtLight = Renderer::GetInstance()->RefRemainingDynamicLight();

	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = true;
	}

	Range = 0.0f;
	EndRange = 0.35f;

	this->bEditPlay = bEditPlay;
};

void ThunderBolt::PlayStart(
	const Vector3& PlayLocation, 
	const std::optional<Vector3>& PlayRotation, 
	const std::optional<Vector3>& PlayScale
)
{
	PlayEnd();
	Velocity = 0.0f;
	Direction = Vector3{ 0.f,0.f,0.f };
	// SetActive(true);

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(PlayLocation);
		if (PlayRotation)
		{
			DirRotMatrix = FMath::Rotation(*PlayRotation);
			SpTransform->SetRotation(*PlayRotation);
		}
		if (PlayScale)
		{
			SpTransform->SetScale(*PlayScale);
		}
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

	Range = 0.0f;
	EndRange = 0.35f;
};

void ThunderBolt::PlayEnd()
{
	if (auto SpPtLight = PtLight.lock();
		SpPtLight)
	{
		SpPtLight->bEnable = false;
	}

	_RenderProperty.bRender = false;
	T = 0.0f;

	if (bParticle)
	{
		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			if (auto _Particle =
				ParticleSystem::GetInstance()->PlayParticle(
					"ThunderBoltEndParticle", 250u, true);
				_Particle.empty() == false)
			{

				for (int32 i = 0; i < _Particle.size(); ++i)
				{
					auto& _PlayInstance = _Particle[i];
					_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
				}
			}
		};
	}

	if (bEditPlay)
	{
		GetComponent<Transform>().lock()->SetPosition(Vector3{ 0.f,0.25f,0.f });
	}
	

	// SetActive(false);
}
float ThunderBolt::GetPlayTime()
{
	return PlayTime;
}
;

void ThunderBolt::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity * std::fabsf(std::sin(T*ScrollSpeed)) );
	_Info.Fx->SetFloat("Time", T);
	_Info.Fx->SetTexture("GradMap", GradMap->GetTexture());
	_Info.Fx->SetFloat("ScrollSpeed", ScrollSpeed);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);

	_Info.Fx->SetFloat("Range", Range);
	_Info.Fx->SetFloat("EndRange", EndRange);

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
		const uint32 TargetSubsetIdx = FMath::Random(0u, 1u);
		if (auto SpSubset = Mesh->GetSubset(TargetSubsetIdx).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0u, "AlbmMap", _Info.Fx);
			SpSubset->Render(_Info.Fx);
		};
	}
};

void ThunderBolt::PlayParticle()
{
	if (bParticle)
	{
		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			if (auto _Particle =
				ParticleSystem::GetInstance()->PlayParticle(
					"ThunderBoltParticle", 500ul, true);
				_Particle.empty() == false)
			{
				for (int32 i = 0; i < _Particle.size(); ++i)
				{
					auto& _PlayInstance = _Particle[i];
					_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
				}
			}
		};
	}
	
}



void ThunderBolt::RenderDebug(const DrawInfo& _Info)
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


HRESULT ThunderBolt::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();

	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT ThunderBolt::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.25f,0.f } );
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 0.001f,0.001f,0.001f });

	return S_OK;
}

HRESULT ThunderBolt::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT ThunderBolt::Update(const float _fDeltaTime)
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

	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(
			SpTransform->GetPosition() + Direction * Velocity);
	}

	if (PtLightFlux > 0.0f)
	{
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
	};




	return 0;
}

UINT ThunderBolt::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void ThunderBolt::Editor()
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

			if (ImGui::CollapsingHeader("Dir"))
			{
				static Vector3 Dir{ 0.f,0.f ,0.f };
				static float SliderVelocity = 1.f;
				ImGui::SliderFloat3("Direction", Dir, -1.f, 1.f);
				ImGui::SliderFloat("SliderVelocity", &SliderVelocity, 0.f, 1.f);
				ImGui::InputFloat("In SliderVelocity", &SliderVelocity);
				if (ImGui::SmallButton("Play_Dir"))
				{
					if (auto SpTransform = GetComponent<Transform>().lock();
						SpTransform)
					{
						PlayStart(
							SpTransform->GetPosition(),
							Dir ,
							SliderVelocity ,std::nullopt,true);
					}
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

void ThunderBolt::OnEnable()
{
	GameObject::OnEnable();
};

void ThunderBolt::OnDisable()
{
	GameObject::OnDisable();
};


