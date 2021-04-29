#include "stdafx.h"
#include "..\Header\CircleWave.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>

void CircleWave::Free()
{
	GameObject::Free();
};

std::string CircleWave::GetName()
{
	return "CircleWave";
};

CircleWave* CircleWave::Create()
{
	return new CircleWave{};
};

void CircleWave::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
			auto SpTransform = GetComponent<Transform>().lock();
			if (SpTransform)
			{
				SpTransform->SetScale({ WaveScale,WaveScale ,WaveScale });
				SpTransform->SetRotation({ 90.f,0.f,0.f });
				_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
				if (_WaveCircle)
				{
					const uint32  Numsubset = _WaveCircle->GetNumSubset();
					_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

					for (uint32 i = 0; i < Numsubset; ++i)
					{
						const auto& _Subset = _WaveCircle->GetSubset(i);
						const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

						_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, WaveScale);
					}
				}
			}
	}
};

void CircleWave::RenderInit()
{
	m_nTag = Player;
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

	/// <summary>
	/// DrawCollider
	/// </summary>
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };


	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
			{"Debug" ,[this](const DrawInfo& _Info)
			{
				DrawCollider(_Info);
			}
		}
	};

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"WaveCircle",[this](const DrawInfo& _Info)
			{
				this->RenderWaveCircle(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	// 
	// 스태틱 메쉬 로딩
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_WaveCircle = Resources::Load < ENGINE::StaticMesh >
			(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx" , _InitInfo);

	_WaveMask = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_03_wp01_007_0000_01_00_MSK4.tga");

	PushEditEntity(_WaveCircle.get());
};

void CircleWave::PlayStart(const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	T = 0.0f;
	_RenderProperty.bRender = true;
};

void CircleWave::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void CircleWave::RenderWaveCircle(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	
	_Color.w =  (  FMath::Clamp(   ( EndT-T ) /EndT +MinAlpha  ,0.0f,1.f));
	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("Intencity", WaveIntencity);
	_Info.Fx->SetFloat("Progress", T);
	_Info.Fx->SetFloat("UV_VOffset", UV_VOffset);
	_Info.Fx->SetBool("bWaveDistortion", bWaveDistortion);

	if (bWaveDistortion)
	{
		_Info.Fx->SetTexture("WaveMaskMap", _WaveMask->GetTexture());
	}

	const uint32 Numsubset = _WaveCircle->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _WaveCircle->GetSubset(i).lock();
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


void CircleWave::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = _WaveCircle->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _WaveCircle->GetSubset(i).lock();
			SpSubset)
		{
			if (false ==
				_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT CircleWave::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.01,0.01,0.01 });
	InitTransform.lock()->SetPosition(Vector3{0.f,0.11544f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT CircleWave::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetPosition(Vector3{/* -12.f,-0.9f,-638.f*/0.f,0.5f,0.5f });
	return S_OK;
}

HRESULT CircleWave::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT CircleWave::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime * WaveSpeed;
	Sin = std::sinf(T);

	// 끝날 쯔음 .
	if (T >= EndT)
	{
		PlayEnd();
	};

	return 0;
}

UINT CircleWave::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void CircleWave::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		if (ImGui::SmallButton("Play"))
		{
			PlayStart();
		}

		ImGui::Text("T : %2.6f", T);
		ImGui::BulletText("r %1.6f g %1.6f b %1.6f a %1.6f",_Color.x,_Color.y,_Color.z,_Color.w);

			ImGui::Checkbox("bWaveDistortion", &bWaveDistortion);
			ImGui::SliderFloat("WaveScale", &WaveScale, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("WaveSpeed", &WaveSpeed, 0.f, 10.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("WaveIntencity", &WaveIntencity, -1.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("MinAlpha", &MinAlpha, 0.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("UV_VOffset", &UV_VOffset, -1.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
			ImGui::SliderFloat("EndT", &EndT, -1.f, 1.f, "%2.6f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic);
	}
}


void CircleWave::OnEnable()
{
	GameObject::OnEnable();
}

void CircleWave::OnDisable()
{
	GameObject::OnDisable();
}

