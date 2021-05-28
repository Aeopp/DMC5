#include "stdafx.h"
#include "JudgementSword.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"
#include "JudgementSwordCollider.h"
#include "SoundSystem.h"
JudgementSword::JudgementSword()
	:m_pParentBoneMat(nullptr)
{
}
void JudgementSword::Free()
{
	GameObject::Free();
}

JudgementSword* JudgementSword::Create()
{
	return new JudgementSword;
}

HRESULT JudgementSword::Ready()
{
	GameObject::Ready();

	m_nTag = TAG_JudgementSword;
	RenderInit();

	m_NRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\wp01_006_NRMR.tga");
	m_ATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\wp01_006_ATOS.tga");
	m_GradationTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\grad.png");

	m_pTransform.lock()->SetScale({ GScale,GScale,GScale });

	PushEditEntity(m_pTransform.lock().get());

	SetActive(false);
	m_vecColliders.reserve(8);
	auto _Collider = AddGameObject<JudgementSwordCollider>();
	_Collider.lock()->SetParentBoneMat(m_pMesh->GetToRootMatrixPtr("_000"));
	m_vecColliders.emplace_back(_Collider);
	for (int i = 0; i < 7; ++i)
	{
		auto _Collider = AddGameObject<JudgementSwordCollider>();
		string BoneName("_000_" + to_string(i + 2));
		_Collider.lock()->SetParentBoneMat(m_pMesh->GetToRootMatrixPtr(BoneName));
		m_vecColliders.emplace_back(_Collider);
	}
	return S_OK;
}

HRESULT JudgementSword::Awake()
{
	GameObject::Awake();

	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	if (!m_pNero.expired())
		m_pParentBoneMat = m_pNero.lock()->Get_BoneMatrixPtr("root");

	return S_OK;
}

HRESULT JudgementSword::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT JudgementSword::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	//
	m_fAccTime += _fDeltaTime;

	if (m_pMesh->IsAnimationEnd())
	{
		// 이호준 <- 내가 범했음 . 2021 05 28 D-Day
		// m_pNero.lock()->StopEffect(Eff_JudgementSwordTrail);
		SetActive(false);
	}
	return 0;
}

UINT JudgementSword::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	
	Matrix								ParentWorldMatrix, FinalWorld;

	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

	if (nullptr != m_pParentBoneMat)
	{
		FinalWorld =  *m_pParentBoneMat * ParentWorldMatrix;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}

	return 0;
}

void JudgementSword::OnEnable()
{
	GameObject::OnEnable();
	_RenderProperty.bRender = true;

	for (auto _pCollider : m_vecColliders)
	{
		_pCollider.lock()->SetActive(true);
	}
	SoundSystem::GetInstance()->Play("Judgement_1", 0.5f, false);
}

void JudgementSword::OnDisable()
{
	GameObject::OnDisable();
	_RenderProperty.bRender = false;

	for (auto _pCollider : m_vecColliders)
	{
		_pCollider.lock()->SetActive(false);
	}
}

void JudgementSword::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

void JudgementSword::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void JudgementSword::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}
}

std::string JudgementSword::GetName()
{
	return "JudgementSword";
}

void JudgementSword::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void JudgementSword::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void JudgementSword::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void JudgementSword::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void JudgementSword::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	}
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			_Info.Fx->SetTexture("NRMR0Map", m_NRMRTex->GetTexture());
			_Info.Fx->SetTexture("ATOS0Map", m_ATOSTex->GetTexture());
			_Info.Fx->SetTexture("GradationMap", m_GradationTex->GetTexture());
			_Info.Fx->SetFloat("_BrightScale", 0.015f);
			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_AccumulationTexV", m_fAccTime * 0.6f);

			SpSubset->Render(_Info.Fx);
		}
	}
}

void JudgementSword::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"NeroWingArmSK",
		[this](const DrawInfo& _Info)
			{
				RenderAlphaBlendEffect(_Info);
			}
		},
	};
	//_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	//{
	//	{"gbuffer_dsSK",
	//	[this](const DrawInfo& _Info)
	//		{
	//			RenderGBufferSK(_Info);
	//		}
	//	},
	//};
	//_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
	//	=
	//{
	//	{"ShadowSK" ,
	//	[this](const DrawInfo& _Info)
	//	{
	//		RenderShadowSK(_Info);
	//	}
	//} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::DebugBone]
		=
	{
		{"DebugBone" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugBone(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"DebugSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugSK(_Info);
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
	RenderInterface::Initialize(_InitRenderProp);

	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\WingSword\\JudgementSword.fbx", _InitInfo);

	m_pMesh->EnableToRootMatricies();

	PushEditEntity(m_pMesh.get());
}

Matrix* JudgementSword::Get_BoneMatrixPtr(std::string _BoneName)
{
	return m_pMesh->GetToRootMatrixPtr(_BoneName);
}
