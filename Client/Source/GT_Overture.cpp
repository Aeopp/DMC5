#include "stdafx.h"
#include "GT_Overture.h"

GT_Overture::GT_Overture()
{
	m_BattleInfo.eAttackType = ATTACKTYPE::Attack_Front;
	m_BattleInfo.iAttack = 10;
}

void GT_Overture::Free()
{
	Gauntlet::Free();
}

GT_Overture* GT_Overture::Create()
{
	return new GT_Overture;
}

HRESULT GT_Overture::Ready()
{
	Gauntlet::Ready();

	RenderInit();

	m_pTransform.lock()->SetScale({ 0.013f,0.013f,0.013f });

	PushEditEntity(m_pTransform.lock().get());

	return S_OK;
}

HRESULT GT_Overture::Awake()
{
	Gauntlet::Awake();
	m_pMesh->PlayAnimation("Shoot_Front", false);

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetCenter({ 0.f,0.08f,0.f });
	m_pCollider.lock()->SetRadius(0.07f);
	m_pCollider.lock()->SetActive(false);
	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT GT_Overture::Start()
{
	Gauntlet::Start();
	return S_OK;
}

UINT GT_Overture::Update(const float _fDeltaTime)
{
	Gauntlet::Update(_fDeltaTime);
	return 0;
}

UINT GT_Overture::LateUpdate(const float _fDeltaTime)
{
	Gauntlet::LateUpdate(_fDeltaTime);
	return 0;
}

void GT_Overture::OnEnable()
{
	Gauntlet::OnEnable();
}

void GT_Overture::OnDisable()
{
	Gauntlet::OnDisable();
}

void GT_Overture::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

std::string GT_Overture::GetName()
{
	return "Overture";
}

void GT_Overture::Editor()
{
	Gauntlet::Editor();
	if(bEdit)
	{
		static char BoneName[MAX_PATH] = "";
		ImGui::InputText("BoneName", BoneName, MAX_PATH);
		if (ImGui::Button("ChangeBone"))
		{
			m_pParentMat = m_pNero.lock()->Get_BoneMatrixPtr(BoneName);
		}
	}
}

void GT_Overture::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void GT_Overture::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}


void GT_Overture::RenderReady()
{
	Gauntlet::RenderReady();
}

void GT_Overture::RenderGBufferSK(const DrawInfo& _Info)
{
	Gauntlet::RenderGBufferSK(_Info);
}

void GT_Overture::RenderShadowSK(const DrawInfo& _Info)
{
	Gauntlet::RenderShadowSK(_Info);
}

void GT_Overture::RenderDebugBone(const DrawInfo& _Info)
{
	Gauntlet::RenderDebugBone(_Info);
}

void GT_Overture::RenderDebugSK(const DrawInfo& _Info)
{
	Gauntlet::RenderDebugSK(_Info);
}

void GT_Overture::RenderInit()
{
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
		// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = true;
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

	// ���̷��� �޽� �ε� ... 
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;

	m_pMesh = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Gauntlet\\Overture\\Overture.fbx", _InitInfo);
	m_pMesh->EnableToRootMatricies();
	PushEditEntity(m_pMesh.get());


}