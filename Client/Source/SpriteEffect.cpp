#include "stdafx.h"
#include "..\Header\SpriteEffect.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>

void SpriteEffect::Free()
{
	GameObject::Free();
};

std::string SpriteEffect::GetName()
{
	return "SpriteEffect";
};

SpriteEffect* SpriteEffect::Create()
{
	return new SpriteEffect{};
};

void SpriteEffect::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		if (auto SpTransform = GetComponent<Transform>().lock();
			SpTransform)
		{
			const float CurScale = SpTransform->GetScale().x;
			_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
			if (_StaticMesh)
			{
				const uint32  Numsubset = _StaticMesh->GetNumSubset();
				_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

				for (uint32 i = 0; i < Numsubset; ++i)
				{
					const auto& _Subset = _StaticMesh->GetSubset(i);
					const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

					_RenderUpdateInfo.SubsetCullingSphere[i] = 
						_CurBS.Transform(_RenderUpdateInfo.World, CurScale);
				}
			}
		}
	}
};

void SpriteEffect::RenderInit()
{
	m_nTag = Eff_SpriteEffect;
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

	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"SpriteEffect",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
};

void SpriteEffect::PlayStart(
	/*const uint32 Col, const uint32 Row,*/ const float SpriteUpdateTime,
	const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	/*SpriteCol = Col;
	SpriteRow = Row;*/
	SpriteColIdx = 0.f;
	SpriteRowIdx = 0.f;
	CurSpriteUpdateTime = this->SpriteUpdateTime = SpriteUpdateTime;

	_RenderProperty.bRender = true;
};

void SpriteEffect::PlayEnd()
{
	_RenderProperty.bRender = false;
};

void SpriteEffect::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World =     _RenderUpdateInfo.World;
	const uint32 Numsubset = _StaticMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetFloat("SpriteXStart", SpriteColIdx / static_cast<float >(SpriteCol));
		_Info.Fx->SetFloat("SpriteXEnd", (SpriteColIdx + 1.f) / static_cast<float>(SpriteCol));
		_Info.Fx->SetFloat("SpriteYStart", SpriteRowIdx / static_cast<float>(SpriteRow));
		_Info.Fx->SetFloat("SpriteYEnd", (SpriteRowIdx + 1.f) / static_cast<float>(SpriteRow));

		_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
		_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
		_Info.Fx->SetVector("_Color", &_Color);

		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("SpriteMap", _SpriteTex->GetTexture());

		if (_DistortionTex)
		{
			_Info.Fx->SetTexture("DistortionMap", _DistortionTex->GetTexture());
		}
	}

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


void SpriteEffect::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

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


HRESULT SpriteEffect::Ready()
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

HRESULT SpriteEffect::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{0.f,0.5f,0.5f });
	return S_OK;
}

HRESULT SpriteEffect::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT SpriteEffect::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	CurSpriteUpdateTime -= _fDeltaTime;

	if (CurSpriteUpdateTime < 0.0f)
	{
		if ( ( (SpriteColIdx) >= (SpriteCol-1)  ) &&
			 ( (SpriteRowIdx) >= (SpriteRow-1)) ) 
		{
			PlayEnd();
			return 0;
		}

		CurSpriteUpdateTime += SpriteUpdateTime;

		SpriteColIdx += 1.f;

		if (SpriteColIdx >= SpriteCol)
		{
			SpriteColIdx = 0.f;
			
			SpriteRowIdx += 1.f;

			if (SpriteRowIdx >= SpriteRow)
			{
				SpriteRowIdx = 0.f;
			}
		}
	}

	return 0;
}

UINT SpriteEffect::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void SpriteEffect::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		if (ImGui::TreeNode(ChildName.c_str()))
		{
			ImGui::Text("Cur Col %2.6f", SpriteColIdx);
			ImGui::SameLine();
			ImGui::Text("Cur Row %2.6f", SpriteRowIdx);

			/*ImGui::InputInt("Play Row", &EditRow);
			ImGui::InputInt("Play Col", &EditCol);*/
			ImGui::InputFloat("Play SpriteUpdateTime", &EditSpriteUpdateTime);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, 0.f, 10000.f);
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 10000.f);

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 0.f, 10000.f);
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 10000.f);

			ImGui::ColorEdit4("_Color",_Color);

			if (ImGui::SmallButton("Play"))
			{
				PlayStart(EditSpriteUpdateTime);
			}

			ImGui::TreePop();
		};
	}
};

void SpriteEffect::OnEnable()
{
	GameObject::OnEnable();
};

void SpriteEffect::OnDisable()
{
	GameObject::OnDisable();
}
void SpriteEffect::RegistInfo(
	const float DistortionIntencity, 
	const float ColorIntencity,
	const Vector4 _Color)
{
	this->DistortionIntencity = DistortionIntencity;
	this->ColorIntencity = ColorIntencity;
	this->_Color = _Color;
};

void SpriteEffect::RegistMesh(const std::string& MeshPath)
{
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;
	_StaticMesh = Resources::Load<StaticMesh>(MeshPath,_InitInfo);
	if (_StaticMesh)
	{
		PushEditEntity(_StaticMesh.get());
	};
};

void SpriteEffect::RegistAlbedoTex(const std::string& TexPath ,
	const uint32 Col, const uint32 Row)
{
	_SpriteTex = Resources::Load<Texture>(TexPath);
	if (_SpriteTex)
	{
		PushEditEntity(_SpriteTex.get());

		SpriteCol = Col;
		SpriteRow = Row;
	};
};

void SpriteEffect::RegistDistortionTex(const std::string& TexPath)
{
	_DistortionTex = Resources::Load<Texture>(TexPath);
	if (_DistortionTex)
	{
		PushEditEntity(_DistortionTex.get());
	};
};

