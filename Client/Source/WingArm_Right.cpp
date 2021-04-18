#include "stdafx.h"
#include "WingArm_Right.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"

WingArm_Right::WingArm_Right()
	:m_bIsRender(false)
{
}

void WingArm_Right::Free()
{
}

WingArm_Right* WingArm_Right::Create()
{
	return new WingArm_Right;
}

HRESULT WingArm_Right::Ready()
{
	RenderInit();

	m_pTransform.lock()->SetScale({ 0.03f,0.03f,0.03f });

	PushEditEntity(m_pTransform.lock().get());

	m_bIsRender = true;
	//SetActive(false);

	return S_OK;
}

HRESULT WingArm_Right::Awake()
{
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());


	return S_OK;
}

HRESULT WingArm_Right::Start()
{
	return S_OK;
}

UINT WingArm_Right::Update(const float _fDeltaTime)
{
	m_pMesh->Update(_fDeltaTime);

	//float fCurAnimationTime = m_pMesh->PlayingTime();

	//if (0.95 <= fCurAnimationTime)
	//{
	//	SetActive(false);
	//}

	return 0;
}

UINT WingArm_Right::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void WingArm_Right::OnEnable()
{
	m_bIsRender = true;

	//Matrix NeroWorld = m_pNero.lock()->Get_NeroWorldMatrix();
	//std::optional<Matrix> R_HandLocal = m_pNero.lock()->Get_BoneMatrix_ByName("root");
	//Matrix R_HandWorld = *R_HandLocal * NeroWorld;

	//memcpy(NeroWorld.m[3], R_HandWorld.m[3], sizeof(Vector3));

	//Vector3 PlayerLook = m_pNero.lock()->GetComponent<Transform>().lock()->GetLook();
	//NeroWorld._41 += PlayerLook.x * -1.7;
	//NeroWorld._43 += PlayerLook.z * -1.7;

	//m_pTransform.lock()->SetWorldMatrix(NeroWorld);
	_RenderProperty.bRender = m_bIsRender;
}

void WingArm_Right::OnDisable()
{
	m_bIsRender = false;
	_RenderProperty.bRender = m_bIsRender;
	//m_pMesh->SetPlayingTime(0);
}

void WingArm_Right::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

std::string WingArm_Right::GetName()
{
	return "WingArm_Right";
}

float WingArm_Right::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float WingArm_Right::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

void WingArm_Right::RenderInit()
{
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = m_bIsRender;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_dsSK",
		[this](const DrawInfo& _Info)
			{
				RenderGBufferSK(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"ShadowSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadowSK(_Info);
		}
	} };
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
	RenderInterface::Initialize(_InitRenderProp);

	// ���̷��� �޽� �ε� ... 
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wing_Arm\\Wing_Arm_Right.fbx");

	m_pMesh->EnableToRootMatricies();

	PushEditEntity(m_pMesh.get());
}

void WingArm_Right::RenderGBufferSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void WingArm_Right::RenderShadowSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void WingArm_Right::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void WingArm_Right::RenderDebugSK(const DrawInfo& _Info)
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
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void WingArm_Right::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
		if (m_pMesh)
		{
			const uint32  Numsubset = m_pMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = m_pMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
}

void WingArm_Right::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{
		// ������ .... 
	}
}
