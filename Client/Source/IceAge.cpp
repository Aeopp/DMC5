#include "stdafx.h"
#include "..\Header\IceAge.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"

IceAge::IceAge()
{};

void IceAge::Free()
{
	GameObject::Free();
};

std::string IceAge::GetName()
{
	return "IceAge";
};

IceAge* IceAge::Create()
{
	return new IceAge{};
};

void IceAge::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
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

void IceAge::RenderInit()
{
	m_nTag = Eff_IceAge;
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
			"IceAge",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}

		/*,
		{
			"IceParticle",[this](const DrawInfo& _Info)
			{
				this->RenderEftIceParticle(_Info);
			}
		}*/
	};

	RenderInterface::Initialize(_InitRenderProp);

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = true;
	// 메시
	{
		Inner = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\nsg.fbx" , _Info);
		// IceParticle = Resources::Load<StaticMesh>("..\\..\\Usable\\Ice\\mesh_03_debris_ice00_01.fbx");
	}
	
	// 텍스쳐 
	Albedo = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\mesh_03_debris_ice00_00_albm.tga");
	TrailMap = Resources::Load<Texture>("..\\..\\Usable\\mesh_03_cs_noise_00_00_alb.tga");
	EmssiveMskMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	NoiseMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Effect\\water_new_height.png");

	PushEditEntity(Inner.get());
	PushEditEntity(Albedo.get());
	PushEditEntity(TrailMap.get());
	PushEditEntity(EmssiveMskMap.get());
	PushEditEntity(NoiseMap.get());

	NoiseScale = { 0.005f,0.02f,0.0f};
	NoiseScrollSpeed = { 0.0005f,0.005f,0.00f};
	EditRotationSpeed = 500.f;
	RollRotationSpeed = FMath::PI;
	EmissiveIntencity = 0.01f;
	ColorIntencity = 0.059f;
	DistortionIntencity = 1.2f;

	{
		ENGINE::ParticleSystem::Particle _PushParticle{};
		_PushParticle.bLerpTimeNormalized = false;
		// Particle 정보 채워주기 
		_PushParticle._ShaderKey = "IceParticle";
		// 공유 정보 바인드 
		_PushParticle.SharedResourceBind = [](
			ENGINE::ParticleSystem::Particle& TargetParticle,
			ID3DXEffect* const Fx)
		{
			if (auto Subset = TargetParticle._Mesh->GetSubset(0).lock();
				Subset)
			{
				Subset->BindProperty(TextureType::DIFFUSE, 0, "AlbmMap", Fx);
				Subset->BindProperty(TextureType::NORMALS, 0, "NrmrMap", Fx);
			}
		};

		_PushParticle.InstanceBind = [](const std::any& _InstanceVariable, ID3DXEffect* const Fx)
		{
			const auto& _Value = std::any_cast<const ParticleInstance::Ice&>(_InstanceVariable);
			Fx->SetFloat("ColorIntencity", _Value.ColorIntencity);
			return;
		};

		const uint64 PoolSize = 10000;
		ParticleSystem::GetInstance()->PreGenerated("Ice", std::move(_PushParticle), PoolSize);
	}

	// 얼음 보숭이 
	// _GeneratorParticle = AddGameObject<ShapeParticle>();
};

void IceAge::PlayStart(
	const std::optional<Vector3>& Location,
	const Vector3 Scale,
	const float RollRotateSpeed,
	const float PlayTime)
{
	Vector3 MyLocation = { 0,0,0 };
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		MyLocation = SpTransform->GetPosition();

		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			SpTransform->SetScale(Scale);
			SpTransform->SetPosition(Location.value()); 
			// 재생 ..

			const Matrix Mat = SpTransform->GetRenderMatrix();
			const uint32 RangeEnd = Inner->m_spVertexLocations->size() - 1u;
			const uint32 JumpOffset = 30u;

			const float AllParticleLifeTime = 3.f;

			{
				auto _PlayableParticle = ParticleSystem::GetInstance()->PlayableParticles("Ice", AllParticleLifeTime);
				for (auto iter = std::begin(_PlayableParticle);
					iter != std::end(_PlayableParticle);)
				{
					auto& _PlayInstance = *iter;

					Vector2 Range{ -0.1f,0.1f };
					const Vector3& TargetLocation =
						FMath::Mul((*Inner->m_spVertexLocations)[FMath::Random(0u, RangeEnd)], Mat);
					const Vector3 Cp0 = TargetLocation + FMath::Random<Vector3>({ Range.x ,Range.x,Range.x },
						{ Range.y,Range.y,Range.y });
					const Vector3 Cp1 = Cp0 + FMath::Random<Vector3>({ Range.x ,Range.x,Range.x }, { Range.y,Range.y,Range.y });
					const Vector3 End = Cp1 + FMath::Random<Vector3>({ Range.x ,Range.x,Range.x }, { Range.y,Range.y,Range.y });

					Vector2 ScaleRange{ 0.01f,0.1f };
					const float PScale = FMath::Random(ScaleRange.x, ScaleRange.y) * GScale;

					const Vector3 StartRot = {
						FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f) };

					const Vector3 RotCp0 = {
						FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f) };

					const Vector3 RotCp1 = {
						FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f) };

					const Vector3 EndRot = {
						FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f),FMath::Random(-360.f,360.f) };

					ParticleInstance::Ice _IceValue{};

					_IceValue.ColorIntencity = FMath::Random(0.01f, 1.f);

					const float LifeTime = FMath::Random(1.f, AllParticleLifeTime);

					_PlayInstance->PlayDescBind(
						{ TargetLocation ,Cp0,Cp1,End },
						{ StartRot ,RotCp0,RotCp1,EndRot },
						{ PScale,PScale,PScale }, LifeTime, 0.0f, _IceValue);

					std::advance(iter, JumpOffset);
				}
			}

		}
	}

	CurParticleTime = ParticleCycle;
	this->PlayTime = PlayTime;
	this->RollRotationSpeed = RollRotateSpeed;
	_RenderProperty.bRender = true;
	T = 0.0f;
};



void IceAge::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
	/*_IceParticles.clear();
	_IceParticles.shrink_to_fit();*/
};

void IceAge::RenderAlphaBlendEffect(const DrawInfo& _Info)
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

	_Info.Fx->SetTexture("AlbedoMap", Albedo->GetTexture());
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

void IceAge::RenderEftIceParticle(const DrawInfo& _Info)
{
	//{
	//	const uint32 Numsubset = IceParticle->GetNumSubset();
	//	for (uint32 i = 0; i < Numsubset; ++i)
	//	{
	//		if (auto SpSubset = IceParticle->GetSubset(i).lock();

	//			SpSubset)
	//		{
	//			SpSubset->BindProperty(TextureType::DIFFUSE, 0, "AlbedoMap", _Info.Fx);
	//			SpSubset->BindProperty(TextureType::NORMALS, 0, "NrmrMap", _Info.Fx);
	//			_Info.Fx->SetTexture("EmssiveMskMap", EmssiveMskMap->GetTexture());
	//			
	//			/*for (auto& _Element : _IceParticles)
	//			{
	//				_Info.Fx->SetFloat("ColorIntencity", _Element.ColorIntencity);
	//				_Info.Fx->SetFloat("EmissiveIntencity", _Element.EmissiveIntencity);
	//				_Info.Fx->SetFloat("AlphaFactor", _Element.AlphaFactor);
	//				_Info.Fx->SetFloat("SpecularPow", _Element.SpecularPow);
	//				const auto _World= FMath::WorldMatrix(_Element.Scale , _Element.Rotation  , _Element.Location);
	//				_Info.Fx->SetMatrix("matWorld", &_World);
	//				SpSubset->Render(_Info.Fx);
	//			}*/
	//		};
	//	};
	//}
};

void IceAge::RenderDebug(const DrawInfo& _Info)
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


HRESULT IceAge::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale( Vector3{ 0.005f,0.005f,0.005f } );
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT IceAge::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 0.005f,0.005f,0.005f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT IceAge::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT IceAge::Update(const float _fDeltaTime)
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

			// 파티클 재생 

		}
	}

	//if (auto _ParticleG = _GeneratorParticle.lock();
	//	_ParticleG)
	//{
	//	if (false == _ParticleG->IsPlaying())
	//	{
	//		{
	//			{
	//				const Vector3 Location = FMath::Mul(*VtxIter, _RenderUpdateInfo.World);
	//				_ParticleG->SetPosition(Location);
	//				_ParticleG->SetShapeIdx(ShapeParticle::SPHERE);
	//				_ParticleG->SetColorIdx(ShapeParticle::COLOR::RED);
	//				// _ParticleG->SetCtrlIdx(ShapeParticle::CONTROLPT::ZERO);
	//				// _ParticleG->SetCtrlIdx(ShapeParticle::CONTROLPT::ZERO);
	//				_ParticleG->SetCtrlIdx(ShapeParticle::CONTROLPT::HALF);
	//				_ParticleG->SetScale(0.003f);
	//				//_ParticleG->SetLoop(true);
	//				_ParticleG->PlayStart(1.f);
	//			}
	//		};
	//	}
	//};

	//static constexpr uint32 Jump = 30;
	//}

	return 0;
}

UINT IceAge::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void IceAge::Editor()
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

				PlayStart(Point, Vector3{ 0.005f ,0.005f ,0.005f }, EditRotationSpeed ,EditPlayTime);
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}

			ImGui::SliderFloat3("NoiseScrollSpeed", NoiseScrollSpeed, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScrollSpeed", NoiseScrollSpeed, "%9.6f");

			ImGui::SliderFloat3("NoiseScale", NoiseScale, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat3("In NoiseScale", NoiseScale, "%9.6f");


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

void IceAge::OnEnable()
{
	GameObject::OnEnable();
};

void IceAge::OnDisable()
{
	GameObject::OnDisable();
};


