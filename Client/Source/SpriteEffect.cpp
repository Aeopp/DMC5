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
			_RenderUpdateInfo.World =
				FMath::Scale(_SpTransform->GetScale()) *
				Renderer::GetInstance()->_RenderInfo.Billboard *
				FMath::Translation(_SpTransform->GetPosition());

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

	// 렌더 정보 초기화 ..
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
	const float PlayTime,
	const std::optional<Vector3>& Location)
{
	if (ConstantPlayTime)
	{
		this->PlayTime = *ConstantPlayTime;
	}
	else
	{
		this->PlayTime = PlayTime;
	}

	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());

		if (_DynamicLight)
		{
			_DynamicLight->PlayStart(Location.value() , this->PlayTime);
		}
	}

	SpriteColIdx = 0.f;
	SpriteRowIdx = 0.f;


	SpriteUpdateTime = this->PlayTime / static_cast<float>((SpriteCol) * (SpriteRow));

	CurSpriteUpdateTime = SpriteUpdateTime;

	_RenderProperty.bRender = true;
	T = 0.0f;

	
};

void SpriteEffect::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;

	if (_DynamicLight)
	{
		_DynamicLight->PlayEnd();
	}
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
		const float AlphaFactor = std::sinf((FMath::Clamp(T / PlayTime,0.0f,1.f) * FMath::PI));
		_Info.Fx->SetFloat("AlphaFactor", AlphaFactor);
		_Info.Fx->SetVector("_Color", &_Color);


		_Info.Fx->SetMatrix("matWorld", &World);

		if (_SpriteTex)
		{
			_Info.Fx->SetTexture("SpriteMap", _SpriteTex->GetTexture());
		}
		else
		{
			_Info.Fx->SetTexture("SpriteMap",nullptr);
		}

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
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
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
	
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT SpriteEffect::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetScale({ 0.001,0.001,0.001 });
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.11141f,0.0f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.2f,0.0f });
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

	T += _fDeltaTime;

	CurSpriteUpdateTime -= _fDeltaTime;

	if (_DynamicLight)
	{
		_DynamicLight->Update(_fDeltaTime ,GetComponent<Transform>().lock()->GetPosition());
	}

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

			ImGui::InputFloat("Play Time", &EditPlayTime);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, 0.f, 10000.f);
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 10000.f);

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 0.f, 10000.f);
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 10000.f);

			ImGui::ColorEdit4("_Color",_Color);

			if (_DynamicLight)
			{
				_DynamicLight->Editor();
			}
		

			if (ImGui::SmallButton("Play"))
			{
				PlayStart(EditPlayTime, GetComponent<Transform>().lock()->GetPosition());
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
void SpriteEffect::InitializeFromOption(const uint32 Option)
{
	if (Option == 0)
	{
		RegistSpriteInfo(8, 4);
		RegistMesh(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
		RegistAlbedoTex(
			"..\\..\\Usable\\Spark\\0.tga");
		RegistInfo(0.1f, 1.f, Vector4{ 1.f,1.f,1.f,1.f });
		ColorIntencity = 0.400f;
		ConstantPlayTime = 0.35f;
		DynamicLight _LightDesc{};
		_LightDesc.Color = { Vector4{1.f,0.25f,0.25f,1.f },
			Vector4{1.f,0.25f,0.25f,1.f } };
		_LightDesc.Flux = { 0.0f,0.000421f };
		_LightDesc.PointRadius = { 0.161f,0.161f };
		RegistDynamicLight(_LightDesc);
	}
	else if (Option == 1)
	{	
		ConstantPlayTime = 0.11f;
		RegistSpriteInfo(2, 1);
		RegistMesh(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
		RegistAlbedoTex("..\\..\\Usable\\Spark\\1.tga");
		RegistInfo(0.1f, 1.f, Vector4{ 1.f,1.f,1.f,1.f });
		ColorIntencity = 0.35f;
		DynamicLight _LightDesc{};
		_LightDesc.Color = { Vector4{1.f,1.f,1.f,1.f }, Vector4{1.f,1.f,1.f,1.f } };
		_LightDesc.Flux = { 0.0f,0.000421f };
		_LightDesc.PointRadius = { 0.161f,0.161f };
		RegistDynamicLight(_LightDesc);
	}
	else if (Option == 2)
	{
		RegistSpriteInfo(2, 2);
		RegistMesh(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
		RegistAlbedoTex("..\\..\\Usable\\Spark\\2.tga");
		RegistInfo(0.1f, 1.f, Vector4{ 1.f,1.f,1.f,1.f });
		ColorIntencity = 0.13f;
		DynamicLight _LightDesc{};
		ConstantPlayTime = 0.1f;
		

		_LightDesc.Color = { Vector4{1.f,1.f,1.f,1.f }, Vector4{1.f,1.f,1.f,1.f } };
		_LightDesc.Flux = { 0.0f,0.00040f };
		_LightDesc.PointRadius = { 0.08f,0.08f };
		RegistDynamicLight(_LightDesc);
	}
	else if (Option == 3)
	{
		RegistSpriteInfo(4, 1);
		RegistMesh(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
		RegistAlbedoTex("..\\..\\Usable\\Spark\\3.tga");
		RegistInfo(0.1f, 1.f, Vector4{ 1.f,1.f,1.f,1.f });
		DynamicLight _LightDesc{};
		ConstantPlayTime = 0.25f;
		_LightDesc.Color = {
			Vector4{9.f / 255.f,83.f / 255.f,242.f / 255.f,1.f },
			Vector4{126.f / 255.f,153.f / 255.f,247.f / 255.f,1.f } };
		_LightDesc.Flux = { 0.0f,0.000421f };
		_LightDesc.PointRadius = { 0.161f,0.161f };
		RegistDynamicLight(_LightDesc);
	}
	else if (Option == 4)
	{
		RegistSpriteInfo(8, 4);
		RegistMesh(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
		RegistAlbedoTex("..\\..\\Usable\\Spark\\4.tga");
		RegistInfo(0.1f, 1.f, Vector4{ 1.f,1.f,1.f,1.f });
		DynamicLight _LightDesc{};
		ConstantPlayTime = 0.25f;

		_LightDesc.Color = { Vector4{1.f,1.f,1.f,1.f }, Vector4{1.f,1.f,1.f,1.f } };
		_LightDesc.Flux = { 0.0f,0.000421f };
		_LightDesc.PointRadius = { 0.161f,0.161f };
		RegistDynamicLight(_LightDesc);
	}
	else if (Option == 5)
	{
		RegistSpriteInfo(4, 2);
		RegistMesh(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
		RegistAlbedoTex("..\\..\\Usable\\Spark\\5.tga");
		RegistInfo(0.1f, 1.f, Vector4{ 1.f,1.f,1.f,1.f });
		DynamicLight _LightDesc{};
		ConstantPlayTime = 0.25f;
		_LightDesc.Color = { Vector4{1.f,1.f,1.f,1.f }, Vector4{1.f,1.f,1.f,1.f } };
		_LightDesc.Flux = { 0.0f,0.000421f };
		_LightDesc.PointRadius = { 0.161f,0.161f };
		RegistDynamicLight(_LightDesc);
	}
};

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
	_StaticMesh = Resources::Load<StaticMesh>(MeshPath, _InitInfo);
	if (_StaticMesh)
	{
		PushEditEntity(_StaticMesh.get());
	};
};

void SpriteEffect::RegistSpriteInfo(const uint32 Col, const uint32 Row)
{
	SpriteCol = Col;
	SpriteRow = Row;
};

void SpriteEffect::RegistAlbedoTex(const std::string& TexPath )
{
	_SpriteTex = Resources::Load<Texture>(TexPath);
	if (_SpriteTex)
	{
		PushEditEntity(_SpriteTex.get());
	};
};

void SpriteEffect::RegistDistortionTex(const std::string& TexPath)
{
	_DistortionTex = Resources::Load<Texture>(TexPath);
	if (_DistortionTex)
	{
		PushEditEntity(_DistortionTex.get());
	};
}

;

void SpriteEffect::RegistDynamicLight(const DynamicLight& _DynamicLight)
{
	this->_DynamicLight = _DynamicLight;
};

