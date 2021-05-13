#include "stdafx.h"
#include "..\Header\SecretVision.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "TimeSystem.h"

#include "RedQueen.h"
#include "ParticleSystem.h"

SecretVision::SecretVision()
{

};

void SecretVision::Free()
{
	GameObject::Free();
};

std::string SecretVision::GetName()
{
	return "SecretVision";
};

SecretVision* SecretVision::Create()
{
	return new SecretVision{};
};

void SecretVision::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		if (_Mesh)
		{
			const uint32 Numsubset = _Mesh->GetNumSubset();
			
			if (Numsubset > 0)
			{
				_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);
				
				if (auto spTransform = GetComponent<ENGINE::Transform>().lock();
					spTransform)
				{
					_RenderUpdateInfo.World = spTransform->GetRenderMatrix();
				}

				for (uint32 i = 0; i < Numsubset; ++i)
				{
					const auto& _Subset = _Mesh->GetSubset(i);
					const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

					_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
				}
			}
		}
	}
};

void SecretVision::RenderInit()
{
	m_nTag = TAG_SecretVision;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"SecretVision",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = true;
	_Mesh = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");

	_TextureArr[0] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\3.tga");
	_TextureArr[1] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\7.tga");
	_TextureArr[2] = Resources::Load<Texture>("..\\..\\Usable\\SecretVision\\9.tga");

	_NoiseMap = Resources::Load<Texture>("..\\..\\Resource\\Texture\\Sky\\mission02\\smoke_01_iam.tga");
};

void SecretVision::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetTexture("NoiseMap", _NoiseMap->GetTexture());
	_Info.Fx->SetFloat("NoiseWrap", NoiseWrap);
	_Info.Fx->SetFloat("TimeCorr", TimeCorr);
	_Info.Fx->SetFloat("Time", TimeSystem::GetInstance()->AccTime());

	for (uint32 i = 0; i < _SVDescs.size(); ++i)
	{
		_Info.Fx->SetTexture("AlbmMap", _TextureArr[i]->GetTexture());
		_Info.Fx->SetFloat("ColorIntencity", _SVDescs[i].ColorIntencity);
		_Info.Fx->SetFloat("AlphaFactor", _SVDescs[i].AlphaFactor);
		_Info.Fx->SetFloat("DistortionIntencity", _SVDescs[i].DistortionIntencity);
		_Info.Fx->SetBool("bDistortion", _SVDescs[i].bDistortion);
		auto SpTransform = GetComponent<Transform>().lock();
		Matrix CurRenderMatrix = _RenderUpdateInfo.World;

		if (SpTransform)
		{
			const Vector3 Bias = SpTransform->GetLook() * static_cast<float>(i) * VisionBias;
			CurRenderMatrix._41 += Bias.x;
			CurRenderMatrix._42 += Bias.y;
			CurRenderMatrix._43 += Bias.z;
		}

		_Info.Fx->SetMatrix("matWorld", &CurRenderMatrix);
		const uint32 Numsubset = _Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = _Mesh->GetSubset(i).lock();
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

void SecretVision::Interaction(const uint32 Idx)
{
	_SVDescs[Idx].AlphaFactor += HitAddAlphaFactor;
	_SVDescs[Idx].ColorIntencity += HitAddColorIntencity;
	_SVDescs[Idx].DistortionIntencity += HitMinusDistortionIntencity;
	--_SVDescs[Idx].Life;

	if (_SVDescs[Idx].Life < 0)
	{
		Disappear(Idx);
	}
};

void SecretVision::Disappear(const uint32 Idx)
{

};

void SecretVision::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	{
		const uint32 Numsubset = _Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = _Mesh->GetSubset(i).lock();
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


HRESULT SecretVision::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();


	return S_OK;
};

HRESULT SecretVision::Awake()
{
	GameObject::Awake();

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale(Vector3{ 0.00370f,0.00602f,0.00447f });
	InitTransform.lock()->SetPosition(Vector3{-3.32006f,1.61942f,17.74776f});
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT SecretVision::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT SecretVision::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	return 0;
}

UINT SecretVision::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void SecretVision::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::Begin(ChildName.c_str());
		{
			ImGui::SliderFloat("NoiseWrap", &NoiseWrap, FLT_MIN, 2.f, "%9.6f");
			ImGui::SliderFloat("TimeCorr", &TimeCorr, FLT_MIN, 2.f, "%9.6f");

			ImGui::SliderFloat("HitAddColorIntencity", &HitAddColorIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::SliderFloat("HitAddAlphaFactor", &HitAddAlphaFactor, FLT_MIN, 1.f, "%9.6f");
			ImGui::SliderFloat("HitMinusDistortionIntencity", &HitMinusDistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			for (uint32 i = 0; i < _SVDescs.size(); ++i)
			{
				const std::string   Str  = std::to_string(i);

				if (ImGui::TreeNode(Str.c_str()))
				{
					if (ImGui::Button("Hit!"))
					{
						Interaction(i);
					}

					ImGui::SliderFloat("ColorIntencity", &_SVDescs[i].ColorIntencity, FLT_MIN, 100000.f, "%9.6f");
					ImGui::InputFloat("In ColorIntencity", &_SVDescs[i].ColorIntencity, 0.f, 0.f, "%9.6f");

					ImGui::SliderFloat("AlphaFactor", &_SVDescs[i].AlphaFactor, FLT_MIN, 1.f, "%9.6f");
					ImGui::InputFloat("In AlphaFactor", &_SVDescs[i].AlphaFactor, 0.f, 0.f, "%9.6f");

					ImGui::SliderFloat("DistortionIntencity", &_SVDescs[i].DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
					ImGui::InputFloat("In DistortionIntencity", &_SVDescs[i].DistortionIntencity, 0.f, 0.f, "%9.6f");

					ImGui::Checkbox("bDistortion", &_SVDescs[i].bDistortion);

					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}
};

void SecretVision::OnEnable()
{
	GameObject::OnEnable();
};

void SecretVision::OnDisable()
{
	GameObject::OnDisable();
};


