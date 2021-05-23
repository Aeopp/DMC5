#include "stdafx.h"
//#include "..\Header\FinalReady.h"
//#include "Transform.h"
//#include "Subset.h"
//#include "TextureType.h"
//#include "Renderer.h"
//#include <iostream>
//#include "ParticleSystem.h"
//
//void FinalReady::Free()
//{
//	GameObject::Free();
//};
//
//std::string FinalReady::GetName()
//{
//	return "FinalReady";
//};
//
//FinalReady* FinalReady::Create()
//{
//	return new FinalReady{};
//};
//
//void FinalReady::RenderReady()
//{
//	auto _WeakTransform = GetComponent<ENGINE::Transform>();
//
//	if (auto _SpTransform = _WeakTransform.lock();
//		_SpTransform)
//	{
//		_RenderUpdateInfo.World =
//			FMath::Scale(_SpTransform->GetScale()) *
//			Renderer::GetInstance()->_RenderInfo.Billboard *
//			FMath::Translation(_SpTransform->GetPosition());
//	};
//};
//
//void FinalReady::PlayParticle()
//{
//	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
//		SpTransform)
//	{
//		const uint32 ParticleCnt = 5555u;
//		const Matrix Mat = FMath::WorldMatrix
//		(SpTransform->GetScale(),
//			Vector3{ 0.f,0.f,0.f },
//			SpTransform->GetPosition());
//
//		auto _PlayableParticle = ParticleSystem::GetInstance()->
//			PlayParticle("ArtemisCylinderParticle", ParticleCnt, true);
//
//		for (int32 i = 0; i < _PlayableParticle.size();
//			++i)
//		{
//			auto& _PlayInstance = _PlayableParticle[i];
//			_PlayInstance->PlayDescBind(Mat);
//		}
//
//	}
//}
//
//void FinalReady::RenderInit()
//{
//	m_nTag = Eff_FinalReady;
//	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
//	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
//	SetRenderEnable(true);
//
//	// 렌더 정보 초기화 ...
//	ENGINE::RenderProperty _InitRenderProp;
//	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
//	// 렌더 속성 전체 초기화 
//	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
//	_InitRenderProp.bRender = false;
//
//	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
//		=
//	{
//		{"Debug" ,
//		[this](const DrawInfo& _Info)
//		{
//			RenderDebug(_Info);
//		}
//	} };
//
//	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
//	{
//		{"FinalReady",
//		[this](const DrawInfo& _Info)
//			{
//				this->RenderAlphaBlendEffect(_Info);
//			}
//		}
//	};
//
//	RenderInterface::Initialize(_InitRenderProp);
//
//	// 
//	// 스태틱 메쉬 로딩
//	Mesh::InitializeInfo _InitInfo{};
//	_InitInfo.bLocalVertexLocationsStorage = false;
//
//	_StaticMesh = Resources::Load<ENGINE::StaticMesh>
//		(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane01.fbx", _InitInfo);
//
//	_Albm = Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\LightShaft\\1.tga");
//
//	PushEditEntity(_StaticMesh.get());
//	PushEditEntity(_Albm.get());
//};
//
//void FinalReady::PlayStart(const Vector3& Location)
//{
//	if (auto SpTransform = GetComponent<Transform>().lock();
//		SpTransform)
//	{
//		SpTransform->SetPosition(Location);
//	}
//
//	this->PlayTime = PlayTime;
//	T = 0.0f;
//	_RenderProperty.bRender = true;
//}; 
//
//void FinalReady::PlayEnd()
//{
//	_RenderProperty.bRender = false;
//	T = 0.0f;
//};
//
//void FinalReady::RenderAlphaBlendEffect(const DrawInfo& _Info)
//{
//	const Matrix World = _RenderUpdateInfo.World;
//	const uint32 Numsubset = _StaticMesh->GetNumSubset();
//
//	if (Numsubset > 0)
//	{
//		_Info.Fx->SetMatrix("matWorld", &World);
//		_Info.Fx->SetTexture("AlbmMap", _Albm->GetTexture());
//
//		const float ColorIntencity = ? ? ;
//		const float AlphaFactor = ? ? ;
//
//		_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
//		_Info.Fx->SetFloat("AlphaFactor", AlphaFactor);
//	};
//
//	for (uint32 i = 0; i < Numsubset; ++i)
//	{
//		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
//			SpSubset)
//		{
//			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
//			{
//				continue;
//			}
//
//			SpSubset->Render(_Info.Fx);
//		};
//	};
//}
//
//
//void AirHike::RenderDebug(const DrawInfo& _Info)
//{
//	const Matrix World = _RenderUpdateInfo.World;
//	_Info.Fx->SetMatrix("World", &World);
//
//	const uint32 Numsubset = _StaticMesh->GetNumSubset();
//	for (uint32 i = 0; i < Numsubset; ++i)
//	{
//		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
//			SpSubset)
//		{
//			if (false ==
//				_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
//			{
//				continue;
//			}
//
//			SpSubset->Render(_Info.Fx);
//		};
//	};
//
//};
//
//
//HRESULT AirHike::Ready()
//{
//	// 트랜스폼 초기화 .. 
//	auto InitTransform = GetComponent<ENGINE::Transform>();
//	PushEditEntity(InitTransform.lock().get());
//	RenderInit();
//	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
//	return S_OK;
//};
//
//HRESULT AirHike::Awake()
//{
//	GameObject::Awake();
//
//	m_pTransform.lock()->SetPosition(Vector3{/* -12.f,-0.9f,-638.f*/0.f,0.5f,0.5f });
//	m_pTransform.lock()->SetScale({ 0.0027f,0.0027f,0.0027f });
//	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.11544f,0.f });
//	m_pTransform.lock()->SetRotation(Vector3{ 90.f ,0.f ,0.0f });
//
//	return S_OK;
//}
//
//HRESULT AirHike::Start()
//{
//	GameObject::Start();
//
//	return S_OK;
//}
//
//UINT AirHike::Update(const float _fDeltaTime)
//{
//	GameObject::Update(_fDeltaTime);
//	if (_RenderProperty.bRender == false) return 0;
//
//	T += _fDeltaTime * Speed;
//	Sin = std::sinf(T);
//
//	if (bPlayedEndParticle == false && T >= (FMath::PI / 4.f))
//	{
//		bPlayedEndParticle = true;
//
//		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
//			SpTransform)
//		{
//			if (auto _Particle =
//				ParticleSystem::GetInstance()->PlayParticle(
//					"AirHikeEndParticle", 2000ul, true);
//				_Particle.empty() == false)
//			{
//
//				for (int32 i = 0; i < _Particle.size(); ++i)
//				{
//					auto& _PlayInstance = _Particle[i];
//					_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
//				}
//			}
//		};
//	}
//
//	CurParticleTime -= _fDeltaTime;
//	if (CurParticleTime < 0.0f)
//	{
//		CurParticleTime += ParticleTime;
//		PlayParticle();
//	}
//
//
//
//	// 끝날 쯔음 .
//	if (T >= PlayTime)
//	{
//		PlayEnd();
//	};
//
//	return 0;
//}
//
//void AirHike::PlayParticle()
//{
//	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
//		SpTransform)
//	{
//		if (auto _Particle =
//			ParticleSystem::GetInstance()->PlayParticle(
//				"AirHikeParticle", 333ul, true);
//			_Particle.empty() == false)
//		{
//
//			for (int32 i = 0; i < _Particle.size(); ++i)
//			{
//				auto& _PlayInstance = _Particle[i];
//				_PlayInstance->PlayDescBind(SpTransform->GetRenderMatrix());
//			}
//		}
//	};
//};
//
//UINT AirHike::LateUpdate(const float _fDeltaTime)
//{
//	GameObject::LateUpdate(_fDeltaTime);
//
//	return 0;
//}
//
//void AirHike::Editor()
//{
//	GameObject::Editor();
//
//	if (bEdit)
//	{
//		const std::string ChildName = GetName() + "_Play";
//		ImGui::BeginChild(ChildName.c_str());
//		if (ImGui::SmallButton("Play"))
//		{
//			PlayStart();
//		}
//
//		ImGui::Text("T : %2.6f", T);
//		ImGui::SliderFloat("Speed", &Speed, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
//
//		ImGui::SliderFloat("StartIntencity", &StartIntencity, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
//		ImGui::SliderFloat("StartScale", &StartScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
//		ImGui::ColorEdit4("StartColor", StartColor);
//
//		ImGui::SliderFloat("FinalIntencity", &FinalIntencity, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
//		ImGui::SliderFloat("FinalScale", &FinalScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
//		ImGui::ColorEdit4("FinalColor", FinalColor);
//		ImGui::EndChild();
//	}
//}
//
//
//void AirHike::OnEnable()
//{
//	GameObject::OnEnable();
//}
//
//void AirHike::OnDisable()
//{
//	GameObject::OnDisable();
//}
//
//
//
