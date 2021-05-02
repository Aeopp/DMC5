#include "stdafx.h"
#include "..\Header\DashTrail.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>

void DashTrail::Free()
{
	GameObject::Free();
};

std::string DashTrail::GetName()
{
	return "DashTrail";
};

DashTrail* DashTrail::Create()
{
	return new DashTrail{};
};

void DashTrail::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		if (auto SpTransform = GetComponent<Transform>().lock();
			SpTransform)
		{
			_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

			auto& _StaticMesh = _Meshs[static_cast<int32>((CurTrailTime / TrailEndTime) * _Meshs.size())];
			const uint32  Numsubset = _StaticMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);
			;

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = _StaticMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, SpTransform->GetScale().x);
			}

		}
	}
};

void DashTrail::RenderInit()
{
	m_nTag = Eff_DashTrail;
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
		{"DashTrail",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	// 메쉬 스태틱 이펙트 트레일 대쉬 01 
	_Meshs[0] = Resources::Load<StaticMesh>
		("..\\..\\Resource\\Mesh\\Static\\Effect\\Trail\\dash_01.fbx");
	_Meshs[1] = Resources::Load<StaticMesh>
		("..\\..\\Resource\\Mesh\\Static\\Effect\\Trail\\dash_02.fbx");
	_Meshs[2] = Resources::Load<StaticMesh>
		("..\\..\\Resource\\Mesh\\Static\\Effect\\Trail\\dash_03.fbx");
	_Meshs[3] = Resources::Load<StaticMesh>
		("..\\..\\Resource\\Mesh\\Static\\Effect\\Trail\\dash_04.fbx");
};

void DashTrail::PlayStart(
	const float TrailChangeSpeed
	, const std::optional<Vector3>& Location )
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	};

	TrailEndTime = TrailChangeSpeed;
	CurTrailTime = 0.0f;

	_RenderProperty.bRender = true;
};

void DashTrail::PlayEnd()
{
	_RenderProperty.bRender = false;
};

void DashTrail::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World =     _RenderUpdateInfo.World;
	auto& _StaticMesh =_Meshs[static_cast<int32>((CurTrailTime / TrailEndTime) * _Meshs.size())];
	const uint32 Numsubset = _StaticMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity );
		_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
		_Info.Fx->SetVector("_Color", &_Color);
	};

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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


void DashTrail::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	auto& _StaticMesh = _Meshs[static_cast<int32>((CurTrailTime / TrailEndTime) * _Meshs.size())];
	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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

HRESULT DashTrail::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.5f,0.5f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT DashTrail::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{0.f,0.5f,0.5f });
	return S_OK;
}

HRESULT DashTrail::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT DashTrail::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	CurTrailTime += _fDeltaTime;

	if (CurTrailTime >= TrailEndTime)
	{
		PlayEnd();
	}

	return 0;
}

UINT DashTrail::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void DashTrail::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		if (ImGui::TreeNode(ChildName.c_str()))
		{
			ImGui::Text("Cur Time %3.6f", CurTrailTime);
			ImGui::SameLine();
			ImGui::Text("End Time %3.6f", TrailEndTime);
			ImGui::SameLine();
			const int32 Idx = static_cast<int32>((CurTrailTime / TrailEndTime) * _Meshs.size());
			ImGui::Text("Cur Idx : %d", Idx);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, 0.f, 10000.f);
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 10000.f);

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 0.f, 100.f);
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 100.f);

			ImGui::ColorEdit4("_Color",_Color);

			ImGui::SliderFloat("EditTrailEndTime", &EditTrailEndTime, 0.f, 10.f);
			ImGui::InputFloat("In EditTrailEndTime", &EditTrailEndTime, 0.f, 10.f);

			if (ImGui::SmallButton("Play"))
			{
				PlayStart(EditTrailEndTime);
			}

			ImGui::TreePop();
		};
	}
};

void DashTrail::OnEnable()
{
	GameObject::OnEnable();
};

void DashTrail::OnDisable()
{
	GameObject::OnDisable();
}
