#include "stdafx.h"
#include "Wire_Arm.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"
#include "Wire_Arm_Grab.h"

Wire_Arm::Wire_Arm()
	:m_bIsRender(false)
	,m_pBoneMatrix(nullptr)
{
	m_nTag = TAG_WireArm;
}

void Wire_Arm::Free()
{
	Unit::Free();
}

Wire_Arm* Wire_Arm::Create()
{
	return new Wire_Arm;
}

HRESULT Wire_Arm::Ready()
{
	Unit::Ready();
	RenderInit();

	m_pTransform.lock()->SetScale({ 0.03f,0.03f,0.03f });
	
	PushEditEntity(m_pTransform.lock().get());

	m_pWireArmGrab = AddGameObject<Wire_Arm_Grab>();
	SetActive(false);

	return S_OK;
}

HRESULT Wire_Arm::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());

	
	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRadius(0.05f);
	PushEditEntity(m_pCollider.lock().get());

	m_pBoneMatrix = m_pMesh->GetToRootMatrixPtr("R_B_Thumb1_front_ST");


	return S_OK;
}

HRESULT Wire_Arm::Start()
{
	Unit::Start();
	return S_OK;
}

UINT Wire_Arm::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);
	
	Matrix NeroWorld = m_pNero.lock()->Get_NeroWorldMatrix();
	Matrix CollWorld;
	CollWorld = *m_pBoneMatrix * NeroWorld;

	Vector3 PlayerRight;
	memcpy(&PlayerRight, NeroWorld.m[0], sizeof(Vector3));
	D3DXVec3Normalize(&PlayerRight, &PlayerRight);
	CollWorld._41 += PlayerRight.x * -0.05f;
	CollWorld._43 += PlayerRight.z * -0.05f;
	m_pTransform.lock()->SetPosition({ CollWorld._41,CollWorld._42,CollWorld._43 });
	if (m_pMesh->IsAnimationEnd())
	{
		SetActive(false);
	}

	return 0;
}

UINT Wire_Arm::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void Wire_Arm::OnEnable()
{
	Unit::OnEnable();
	m_bIsRender = true;
	D3DXMatrixIdentity(&m_MyRenderMatrix);

	Matrix NeroWorld = m_pNero.lock()->Get_NeroWorldMatrix();
	std::optional<Matrix> R_HandLocal = m_pNero.lock()->Get_BoneMatrix_ByName("R_Hand");
	Matrix R_HandWorld = *R_HandLocal * NeroWorld;

	memcpy(NeroWorld.m[3], R_HandWorld.m[3],sizeof(Vector3));

	Vector3 PlayeUp = m_pNero.lock()->GetComponent<Transform>().lock()->GetUp();
	NeroWorld._42 += PlayeUp.y * -0.12f;

	m_MyRenderMatrix = NeroWorld;
	_RenderProperty.bRender = m_bIsRender;
	m_bCollEnable = true;
}

void Wire_Arm::OnDisable()
{
	Unit::OnDisable();
	m_bIsRender = false;
	_RenderProperty.bRender = m_bIsRender;
	m_bCollEnable = false;
	//m_pMesh->SetPlayingTime(0);
}

void Wire_Arm::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (!m_bCollEnable)
		return;
	m_bCollEnable = false;
	UINT ObjTag = _pOther.lock()->m_nTag;

	switch (ObjTag)
	{
	case GAMEOBJECTTAG::Monster100:
	{
		Vector3 MyPos = m_pTransform.lock()->GetPosition();
		MyPos.y -= 0.2f;
		m_pWireArmGrab.lock()->GetComponent<Transform>().lock()->SetPosition(MyPos);
		m_pWireArmGrab.lock()->SetGrabedMonster(_pOther);
		m_pWireArmGrab.lock()->SetActive(true);
		_RenderProperty.bRender = m_bIsRender = false;
	}
		break;
	case GAMEOBJECTTAG::Monster0000:
	{
		Vector3 MyPos = m_pTransform.lock()->GetPosition();
		MyPos.y -= 0.2f;
		m_pWireArmGrab.lock()->GetComponent<Transform>().lock()->SetPosition(MyPos);
		m_pWireArmGrab.lock()->SetGrabedMonster(_pOther);
		m_pWireArmGrab.lock()->SetActive(true);
		_RenderProperty.bRender = m_bIsRender = false;
	}
	break;
	default:
		break;
	}
}

void Wire_Arm::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{

}

void Wire_Arm::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Wire_Arm::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

std::string Wire_Arm::GetName()
{
	return "Wire_Arm";
}

float Wire_Arm::Get_PlayingTime()
{
	return m_pMesh->PlayingTime();
}

float Wire_Arm::Get_PlayingAccTime()
{
	return m_pMesh->PlayingAccTime();
}

void Wire_Arm::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = m_MyRenderMatrix;
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

void Wire_Arm::Editor()
{
	Unit::Editor();
	if (bEdit)
	{
		// 에디터 .... 
	}
}

void Wire_Arm::RenderInit()
{
	SetRenderEnable(true);

	// 렌더 속성 전체 초기화 
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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

	// 스켈레톤 메쉬 로딩 ... 
	Mesh::InitializeInfo _InitInfo{};
	// 버텍스 정점 정보가 CPU 에서도 필요 한가 ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wire_Arm\\Wire_Arm.fbx");
	m_pMesh->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Wire_Arm\\Wire_Arm.Animation");
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());
}

void Wire_Arm::RenderGBufferSK(const DrawInfo& _Info)
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

void Wire_Arm::RenderShadowSK(const DrawInfo& _Info)
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

void Wire_Arm::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Wire_Arm::RenderDebugSK(const DrawInfo& _Info)
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
