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
#include "PreLoader.h"
#include "NhDoor.h"


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
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetBool("bDistortion", bDistortion);

	for (uint32 i = 0; i < _SVDescs.size(); ++i)
	{
		if (_SVDescs[i].bSurvive ==false)
		{
			continue;
		}

		_Info.Fx->SetTexture("AlbmMap", _TextureArr[i]->GetTexture());
		_Info.Fx->SetFloat("ColorIntencity", _SVDescs[i].ColorIntencity);
		_Info.Fx->SetFloat("AlphaFactor", _SVDescs[i].AlphaFactor);

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
	bInteraction = false;

	DistortionIntencity += HitMinusDistortionIntencity;
	NoiseWrap += HitMinusNoiseWrap;
	_SVDescs[Idx].AlphaFactor += HitAddAlphaFactor;
	_SVDescs[Idx].ColorIntencity += HitAddColorIntencity;
	--_SVDescs[Idx].Life;

	if (_SVDescs[Idx].Life < 0)
	{
		Disappear(Idx);
	}
};

void SecretVision::Disappear(const uint32 Idx)
{
	++InteractionIdx;
};

void SecretVision::PuzzleEndParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle("SecretVisionDisappear", 15000ul, true);
			_Particle.empty() == false)
		{
			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
			}
		}
	};
};

void SecretVision::Default()
{
	NoiseWrap = DefaultNoiseWrap;
	DistortionIntencity = DefaultDistortionIntencity;

	const bool bPuzzleEnd =
		std::all_of(std::begin(_SVDescs), std::end(_SVDescs), []
		(const SecretVisionDesc& _Desc)
			{
				return !_Desc.bSurvive;
			});

	if (bPuzzleEnd)
	{
		PuzzleEnd();
	}
}
void SecretVision::PuzzleStart()
{
	if (auto SpCollider = _Collider.lock();
		SpCollider)
	{
		SpCollider->SetActive(true);
	}

	bEnable = true;
	_RenderProperty.bRender = true;
	InteractionIdx = 0u;
};

uint32 SecretVision::GetInteractionIdx() const
{
	return InteractionIdx;
};

void SecretVision::SetInteractionEnable(const bool bInteraction)
{
	this->bInteraction = bInteraction;
};

void SecretVision::PuzzleEnd()
{
	// SetActive(false);
	if (auto SpCollider = _Collider.lock();
		SpCollider)
	{
		SpCollider->SetActive(true);
	}

	_RenderProperty.bRender = false;
	bEnable = false;

	NhDoorOpenTime = 0.0f;
	PuzzleEndParticle();
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
	InitTransform.lock()->SetPosition(Vector3{ -3.32006f,1.61942f,17.74776f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });

	_Collider = AddComponent<BoxCollider>();

	if (auto SpCollider = _Collider.lock(); SpCollider)
	{
		SpCollider->ReadyCollider();
		SpCollider->SetSize(Vector3{ 1.f,1.f,1.f });
		PushEditEntity(SpCollider.get());
	}



	return S_OK;
}

HRESULT SecretVision::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT SecretVision::Update(const float _fDeltaTime)
{
	if (NhDoorOpenTime)
	{
		(*NhDoorOpenTime) += _fDeltaTime;

		if ((*NhDoorOpenTime) >= PreLoader::SecretVisionDisappearParticleLifeEnd)
		{
			SetActive(false);
			if (auto _NhDoor = std::dynamic_pointer_cast<NhDoor>( FindGameObjectWithTag(TAG_NhDoor).lock());
				_NhDoor)
			{
				_NhDoor->DissolveStart();
			}
		};
	}

	if (bEnable == false)
		return 0;

	GameObject::Update(_fDeltaTime);

	for (auto& _SVDesc : _SVDescs)
	{
		if (_SVDesc.Life < 0 && _SVDesc.bSurvive)
		{
			if (_SVDesc.T > 1.f)
			{
				_SVDesc.bSurvive = false;
				_SVDesc.ColorIntencity = 0.0f;
				_SVDesc.AlphaFactor = 0.0f;
				_SVDesc.Life = SecretVisionDesc::DefaultLife;
				_SVDesc.T=0.0f;
				Default();
			}

			_SVDesc.ColorIntencity= 
				FMath::BezierCurve(_SVDesc.ColorIntencity, SecretVisionDesc::DisappearEndColorIntencity,0.0f, _SVDesc.T);
// 			_SVDesc.AlphaFactor = FMath::Lerp(_SVDesc.AlphaFactor, 0.0f, _SVDesc.T);

			_SVDesc.T += (_fDeltaTime * SecretVisionDesc::DisappearAcc);
		}
	}

	return 0;
}

UINT SecretVision::LateUpdate(const float _fDeltaTime)
{
	if (bEnable == false)
		return 0;

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
			if (ImGui::Button("Default"))
			{
				Default();
			}

			if (ImGui::Button("Puzzle Start"))
			{
				PuzzleStart();
			}

			ImGui::SliderFloat("DisappearEndColorIntencity", 
				&SecretVisionDesc::DisappearEndColorIntencity, FLT_MIN, 100000.f, "%9.6f");
			ImGui::SliderFloat("DisappearAcc",
				&SecretVisionDesc::DisappearAcc, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat("NoiseWrap", &NoiseWrap, FLT_MIN, 2.f, "%9.6f");
			ImGui::SliderFloat("TimeCorr", &TimeCorr, FLT_MIN, 2.f, "%9.6f");
			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, 0.f, 0.f, "%9.6f");
			ImGui::Checkbox("bDistortion", &bDistortion);

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

					ImGui::Checkbox("bSurvive", &_SVDescs[i].bSurvive);

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

void SecretVision::OnTriggerEnter(std::weak_ptr<GameObject> _Target)
{
	if (!bInteraction)return;

	static const std::set<uint32> HitEnableTargetSet
	{
			TAG_RedQueen,
			Tag_Cbs_Short,
			Tag_Cbs_Middle,
			Tag_Cbs_Long,
	};

	if (auto SpTarget = _Target.lock();
		SpTarget)
	{
		if ( HitEnableTargetSet.contains(SpTarget->m_nTag))
		{
			Interaction(InteractionIdx);
			if (InteractionIdx >=3u)
			{
				PuzzleEnd();
			}
		}
	}
};


