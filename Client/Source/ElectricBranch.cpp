#include "stdafx.h"
#include "..\Header\ElectricBranch.h"
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

ElectricBranch::ElectricBranch()
{

};

void ElectricBranch::Free()
{
	GameObject::Free();
};

std::string ElectricBranch::GetName()
{
	return "ElectricBranch";
};

ElectricBranch* ElectricBranch::Create()
{
	return new ElectricBranch{};
};

void ElectricBranch::RenderReady()
{
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void ElectricBranch::RenderInit()
{
	m_nTag = Eff_ElectricBranch;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
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
		{"ElectricBranch",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
	//magic_eletric00.fbx
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	Mesh = Resources::Load<StaticMesh>("..\\..\\Usable\\Electric\\Spark\\branch.fbx", _InitInfo);
	GradMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");
	DistortionMap =Resources::Load<Texture>("..\\..\\Usable\\smoke_a_im.tga");
	AlbMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\lightning.dds");
};

void ElectricBranch::PlayStart(
	const Vector3& PlayLocation, const std::optional<Vector3>& PlayRotation, const std::optional<Vector3>& PlayScale)
{
	PlayEnd();

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(PlayLocation);
		if (PlayRotation)
		{
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

void ElectricBranch::PlayEnd()
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
				"ElectricBranchEndParticle", 250u, true);
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
float ElectricBranch::GetPlayTime()
{
	return PlayTime;
}
;

void ElectricBranch::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity * std::fabsf(std::sin(T*ScrollSpeed)) );
	_Info.Fx->SetFloat("Time", T);
	_Info.Fx->SetTexture("GradMap", GradMap->GetTexture());
	_Info.Fx->SetFloat("ScrollSpeed", ScrollSpeed);
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetFloat("BlurIntencity", BlurIntencity * (PlayTime-T)) ;


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

void ElectricBranch::PlayParticle()
{
	if (bParticle)
	{
		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			if (auto _Particle =
				ParticleSystem::GetInstance()->PlayParticle(
					"ElectricBranchParticle", 250ul, true);
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
}



void ElectricBranch::RenderDebug(const DrawInfo& _Info)
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


HRESULT ElectricBranch::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();

	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT ElectricBranch::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.5f,0.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 0.01f,0.01f,0.01f });

	return S_OK;
}

HRESULT ElectricBranch::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT ElectricBranch::Update(const float _fDeltaTime)
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
	}
	

	return 0;
}

UINT ElectricBranch::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void ElectricBranch::Editor()
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

			ImGui::SliderFloat("BlurIntencity", &BlurIntencity, FLT_MIN, 1.f , "%9.6f");


			ImGui::SliderFloat("SubsetDelta", &SubsetDelta, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In SubsetDelta", &SubsetDelta, 0.f, 1.f, "%9.6f");

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

void ElectricBranch::OnEnable()
{
	GameObject::OnEnable();
};

void ElectricBranch::OnDisable()
{
	GameObject::OnDisable();
};


