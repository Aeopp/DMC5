#include "stdafx.h"
#include "Em5300.h"
#include "SnatchPoint.h"
#include "Transform.h"
#include "Subset.h"
#include "Renderer.h"
#include "Nero.h"
#include "TextureType.h"

void SnatchPoint::Free()
{
	GameObject::Free();
}

SnatchPoint* SnatchPoint::Create()
{
	return new SnatchPoint;
}

std::string SnatchPoint::GetName()
{
	return "SnatchPoint";
}


HRESULT SnatchPoint::Ready()
{
	Monster::Ready();

	m_nTag = MonsterSnatchPoint;

	m_BattleInfo.iMaxHp = 1;
	m_BattleInfo.iHp = 1;

	RenderInit();

	return S_OK;
}

HRESULT SnatchPoint::Awake()
{
	Unit::Awake();
	
	m_nTag = MonsterSnatchPoint;
	m_pEm5300Trans = m_pEm5300.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(false);
	m_pCollider.lock()->SetCenter({ 0.f, 0.f, 0.f });
	m_pCollider.lock()->SetRadius(0.15f);

	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);


	m_pTransform.lock()->SetScale({ 0.0015f, 0.0015f, 0.0015f });

	if (m_iSnatchPos == 0)
		m_vStartPos = { -36.12f, -0.35f, 32.33f };
	else if (m_iSnatchPos == 1)
		m_vStartPos = { -38.44f, -0.35f, 32.06f };
	else if (m_iSnatchPos == 2)
		m_vStartPos = { -38.41f, -0.35f, 29.64f };
	else if (m_iSnatchPos == 3)
		m_vStartPos = { -35.6f,  -0.35f, 29.76f };

	m_pTransform.lock()->SetPosition(m_vStartPos);

	return S_OK;
}

HRESULT SnatchPoint::Start()
{
	Unit::Start();
	return S_OK;
}

UINT SnatchPoint::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);


	if (m_bFly)
		Fly();
	else
	{
		Vector3 vDir = m_vStartPos - m_pTransform.lock()->GetPosition();
		float   fDir = D3DXVec3Length(&vDir);

		D3DXVec3Normalize(&vDir, &vDir);

		if (fDir >= 0.4f)
			m_pTransform.lock()->Translate(vDir * 0.03f);
		else
		{
			m_pTransform.lock()->SetPosition(m_vStartPos);
			m_bTargetEnable = true;
		}
	}
	
	return 0;
}

UINT SnatchPoint::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	return 0;
}

void SnatchPoint::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void SnatchPoint::OnEnable()
{
	Unit::OnDisable();
}

void SnatchPoint::OnDisable()
{
	Unit::OnDisable();
}

void SnatchPoint::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
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

void SnatchPoint::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void SnatchPoint::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	switch (_pOther.lock()->m_nTag)
	{
	case GAMEOBJECTTAG::TAG_WireArm:
		m_bFly = true;
		break;
	}
}

void SnatchPoint::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{

}



void SnatchPoint::RenderInit()
{
	m_nTag = MonsterWeapon;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_ds",
		[this](const DrawInfo& _Info)
			{
				RenderGBuffer(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"Shadow" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadow(_Info);
		}
	} };

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
	RenderInterface::Initialize(_InitRenderProp);

	// 
	// 스태틱 메쉬 로딩

	m_pMesh = Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Static\\snatchpoint\\snatchpoint.fbx");
	PushEditEntity(m_pMesh.get());
}

void SnatchPoint::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
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
}

void SnatchPoint::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}


			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void SnatchPoint::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
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

void SnatchPoint::Snatch(const float _fDeltaTime)
{

}

void SnatchPoint::Fly()
{
	Vector3	vDir = m_pEm5300Trans.lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float   fDir = D3DXVec3Length(&vDir);

	D3DXVec3Normalize(&vDir, &vDir);
	m_bTargetEnable = false;

	if (fDir >= 0.3f)
		m_pTransform.lock()->Translate(vDir * 0.035f);
	else
		m_bFly = false;




}

void SnatchPoint::Fight(const float _fDeltaTime)
{
}

void SnatchPoint::State_Change(const float _fDeltaTime)
{
}

void SnatchPoint::Skill_CoolTime(const float _fDeltaTime)
{
}

void SnatchPoint::Buster(BT_INFO _BattleInfo, void* pArg)
{
}

void SnatchPoint::Snatch(BT_INFO _BattleInfo, void* pArg)
{
}

void SnatchPoint::Rotate(const float _fDeltaTime)
{
}

void SnatchPoint::Update_Angle()
{
}


