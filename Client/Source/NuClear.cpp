#include "stdafx.h"
#include "..\Header\NuClear.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "NuclearLensFlare.h"
#include "Nero.h"
#include "MainCamera.h"

void NuClear::Free()
{
	GameObject::Free();
};

std::string NuClear::GetName()
{
	return "NuClear";
};

NuClear* NuClear::Create()
{
	return new NuClear{};
};

void NuClear::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const float LerpT = FMath::Clamp(T / GrowEndT, 0.0f, 1.f);
		const float CurScale = FMath::Lerp(0.0f, GrowEndScale, LerpT);

		_SpTransform->SetScale(
			Vector3{ CurScale ,CurScale ,CurScale });
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void NuClear::RenderInit()
{
	m_nTag = Eff_NuClear;
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

		{"NuClear",
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
		L"..\\..\\Usable\\Artemis\\LightSphere2.tga"
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
		ExplosionReadyFluxLow,ExplosionReadyFluxHigh
	};

	_DynamicLight.PointRadius=
	{
		ExplosionReadyRadiusLow,ExplosionReadyRadiusHigh
	};
};

void NuClear::PlayStart(const Vector3& Location,  const float GroundY, const bool bEditPlay)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location);
			SpTransform->SetScale(Vector3{ 0.f,0.f,0.f });
			_NuclearLensFlare.lock()->PlayStart(Location, ExplosionReadyTime);
			_DynamicLight.PlayStart(Location, ExplosionReadyTime + FreeFallTime + ExplosionTime);
		}
	}

	EditYVelocity =   std::fabsf ( (Location.y - GroundY) /FreeFallTime);

	T = 0.0f;
	_RenderProperty.bRender = true;
	CurParticleDelta = 0.0f;

	PlayParticle();

	this->bEditPlay = bEditPlay;
	bExplosion = false;
	bKaboom = false;
	bBlackOut = false;
};

void NuClear::Kaboom()
{
	KaboomParticle();
	bKaboom = true;
	_RenderProperty.bRender = false;
	T = 0.0f;
	_ShockWave.lock()->PlayStart(
		Vector3{ KaboomMatrix._41,KaboomMatrix._42,KaboomMatrix._43 } ,
		ShockWave::Option::Kaboom);
	m_pCollider.lock()->SetActive(true);

	std::list<std::weak_ptr<GameObject>> _SnatchPoints = FindGameObjectsWithTag(MonsterSnatchPoint);
	for (auto& pSnatchPoint : _SnatchPoints)
	{
		pSnatchPoint.lock()->SetActive(false);
	}
	m_pMainCamera.lock()->CalcEm5300NeroAngle();
	m_pMainCamera.lock()->SetDistance(0.9f);
	m_pMainCamera.lock()->Set_At_Transform(m_pNero.lock()->GetComponent<Transform>(), MainCamera::AT_PLAYER);
	m_pMainCamera.lock()->SetFloatingAmount(0.17f);
};

void NuClear::PlayEnd()
{
	_DynamicLight.PlayEnd();
	bBlackOut = true;
	bExplosion = false;
	auto SpTransform = GetComponent<Transform>().lock();

	Vector3 _KaboomLocation = SpTransform->GetPosition();
	_KaboomLocation.y = -1.035f;

	KaboomMatrix =
		FMath::Scale(Vector3{ ParticleWorldScale ,ParticleWorldScale ,ParticleWorldScale })
		*
		SpTransform->GetRotationMatrix()
		*
		FMath::Translation(_KaboomLocation);

	if (bEditPlay)
	{
		SpTransform->SetPosition(Vector3{ -37.411f,0.821f,30.663f });
	}
};

bool NuClear::IsKaboom()
{
	return bKaboom;
};

bool NuClear::IsBlackOut()
{
	return bBlackOut;
};

bool NuClear::IsFallTime()
{
	return  T > (ExplosionReadyTime );
};

void NuClear::KaboomParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"Kaboom", 3333u, true);
			_Particle.empty() == false)
		{
			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(KaboomMatrix);
			}
		}
	};
}

void NuClear::ParticleUpdate(const float DeltaTime)
{
	if (T > ExplosionReadyTime)
		return;

	CurParticleDelta -= DeltaTime;
	if (CurParticleDelta < 0.0f)
	{
		CurParticleDelta += ParticleDelta;
		PlayParticle();
	}
};

void NuClear::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"NuClearParticle", 22u, true);
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

void NuClear::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	if (T >= (ExplosionReadyTime + FreeFallTime))
		return;

	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("LightMskMap", _LightMsk->GetTexture());
		_Info.Fx->SetTexture("AlbmMap", _AlbmMap->GetTexture());
		_Info.Fx->SetTexture("NoiseMap", _Noise->GetTexture());

		
		const float LerpT = 
			FMath::Clamp(T / GrowEndT, 0.0f, 1.f);

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


void NuClear::RenderDebug(const DrawInfo& _Info)
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


HRESULT NuClear::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT NuClear::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.0020f,0.0020f,0.0020f });
	m_pTransform.lock()->SetPosition(Vector3{ -37.411f,0.821f,30.663f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });

	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_pMainCamera = static_pointer_cast<MainCamera>(FindGameObjectWithTag(GAMEOBJECTTAG::TAG_Camera).lock());

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetRadius(30.f);
	m_pCollider.lock()->SetActive(false);
	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT NuClear::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT NuClear::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	// 끝날 쯔음 .
	const float BeyondPlay = ExplosionReadyTime + FreeFallTime + ExplosionTime;
	const float BeyondKaboom =  ExplosionReadyTime + FreeFallTime + ExplosionTime + 0.6f;

	if (T > BeyondKaboom)
	{
		Kaboom();
		return 0;
	}
	else if (T > BeyondPlay)
	{
		PlayEnd();
	}
	

	ParticleUpdate(_fDeltaTime);

	const float BeyondReady = ExplosionReadyTime + FreeFallTime;

	if (T <= ExplosionReadyTime)
	{
		_NuclearLensFlare.lock()->UpdatePlayVariable(T / ExplosionReadyTime);
	}

	if (T <= GrowEndT)
	{
		const float LerpT = T / GrowEndT;
		const float Radius = FMath::Lerp(ExplosionReadyRadiusLow, ExplosionReadyRadiusHigh, LerpT);
		const float Flux=FMath::Lerp(ExplosionReadyFluxLow, ExplosionReadyFluxHigh, LerpT);
		const D3DXCOLOR _Color = FMath::ToColor(FMath::Lerp(ColorLow,ColorHigh ,LerpT));
		_DynamicLight.Update(_Color, Radius, Flux, GetComponent<Transform>().lock()->GetPosition());
	}
	else if (FMath::IsRange(ExplosionReadyTime, ExplosionReadyTime + FreeFallTime, T))
	{
		Vector3 CurPosition = GetComponent<Transform>().lock()->GetPosition();
		CurPosition.y -= EditYVelocity * _fDeltaTime;
		GetComponent<Transform>().lock()->SetPosition(CurPosition);
	}
	else if (T > BeyondReady)
	{
		if (false == bExplosion)
		{
			bExplosion = true;
			// KaboomParticle();
		}

		const float ExplosionT =  ( T - BeyondReady ) / ExplosionTime;
		const float Radius = FMath::Lerp(ExplosionRadiusLow, ExplosionRadiusHigh, ExplosionT);
		const float Flux = FMath::Lerp(ExplosionFluxLow, ExplosionFluxHigh, ExplosionT);
		const D3DXCOLOR _Color = FMath::ToColor(FMath::Lerp(ColorLow, ColorHigh, ExplosionT));
		 Vector3 CurPosition = GetComponent<Transform>().lock()->GetPosition(); 
		_DynamicLight.Update(_Color, Radius, Flux, CurPosition);
	}

	if (!m_pNero.expired())
	{
		Vector3 MyPos = m_pTransform.lock()->GetPosition();
		Vector3 PlayerPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
		MyPos.y = 0.f;
		PlayerPos.y = 0.f;
		Vector3 Length = MyPos - PlayerPos;
		float Distance = D3DXVec3Length(&Length);

		Vector3 Dir = *D3DXVec3Normalize(&Dir, &Length);
		if (Distance >= 0.3f)
		{
			m_pNero.lock()->GetComponent<Transform>().lock()->Translate(Dir * 0.6f * _fDeltaTime);
		}
	}
	if (!m_pMainCamera.expired())
	{
		//m_pMainCamera.lock()->ControlDistance(_fDeltaTime * 0.3f, 1.8f);
		m_pMainCamera.lock()->ControlDistance(_fDeltaTime * 0.3f, 1.5f);
		if (!IsFallTime())
		{
			m_pMainCamera.lock()->SetFloatingAmount(0.4f);
		}
	}

	return 0;
}


UINT NuClear::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void NuClear::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		if (ImGui::SmallButton("Play"))
		{
			PlayStart(
				GetComponent<Transform>().lock()->GetPosition(),-1.45f, true);
		}

		ImGui::Text("T : %2.6f", T);
		ImGui::SliderFloat("NoiseTimeCorr", &NoiseTimeCorr, 0.0f, 100.f);

		ImGui::SliderFloat("EditYVelocity", &EditYVelocity, 0.0f, 1.f);
		

		ImGui::SliderFloat("ExplosionReadyFluxLow", &ExplosionReadyFluxLow, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionReadyFluxHigh", &ExplosionReadyFluxHigh, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionReadyRadiusLow", &ExplosionReadyRadiusLow, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionReadyRadiusHigh", &ExplosionReadyRadiusHigh, 0.0f, 10.f);

		ImGui::SliderFloat("ExplosionFluxLow", &ExplosionFluxLow, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionFluxHigh", &ExplosionFluxHigh, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionRadiusLow", &ExplosionRadiusLow, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionRadiusHigh", &ExplosionRadiusHigh, 0.0f, 10.f);

		ImGui::SliderFloat("ParticleDelta", &ParticleDelta, 0.0f, 1.f);
		ImGui::SliderFloat("ParticleWorldScale", &ParticleWorldScale, 0.0f, 1.f);
		

		ImGui::SliderFloat("GrowEndT", &GrowEndT, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionReadyTime", &ExplosionReadyTime, 0.0f, 40.f);
		ImGui::SliderFloat("FreeFallTime", &FreeFallTime, 0.0f, 10.f);
		ImGui::SliderFloat("ExplosionTime", &ExplosionTime, 0.0f, 10.f);

		ImGui::SliderFloat("StartColorIntencity", &StartColorIntencity, 0.0f, 10.f);
		ImGui::SliderFloat("EndColorIntencity", &EndColorIntencity, 0.0f, 10.f);
		
		ImGui::SliderFloat("GrowEndScale", &GrowEndScale, 0.0f, 1.f);

		ImGui::EndChild();
	}
}


void NuClear::OnEnable()
{
	GameObject::OnEnable();
}

void NuClear::OnDisable()
{
	GameObject::OnDisable();
}



