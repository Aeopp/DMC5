#include "stdafx.h"
#include "..\Header\Car.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Em5000.h"
#include "TestObject.h"
#include "Nero.h"

void Car::Free()
{
	GameObject::Free();
}

std::string Car::GetName()
{
	return "Car";
};

Car* Car::Create()
{
	return new Car;
}



void Car::RenderInit()
{
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 속성 전체 초기화 
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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
	// 스태틱 메쉬 로딩
	_StaticMesh = Resources::Load<ENGINE::SkeletonMesh>(
		L"..\\..\\Resource\\Map\\Object\\Carc_dent01\\Carc_dent01.fbx");
	PushEditEntity(_StaticMesh.get());
	_StaticMesh->EnableToRootMatricies();

}

void Car::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		_StaticMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Car::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		_StaticMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}



void Car::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = _StaticMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_StaticMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
		{
			continue;
		}
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Car::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (_StaticMesh)
		{
			const uint32  Numsubset = _StaticMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = _StaticMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
}

HRESULT Car::Ready()
{
	m_vBangDir = { 0.f, 1.f, 0.f };
	m_fBangPower = 8.f;
	m_nTag = MonsterWeapon;
	m_BattleInfo.eAttackType = Attack_KnocBack;
	m_bCollEnable = true;
	RenderInit();
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001f,0.001f,0.001f});
	PushEditEntity(InitTransform.lock().get());


	m_pTransform = GetComponent<ENGINE::Transform>();
	m_pTransform.lock()->SetPosition({ 10.f, 5.f, 10.f });

	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	// PushEditEntity(_ShaderInfo.ForwardAlphaBlendShader.get());

	return S_OK;
};

HRESULT Car::Awake()
{
	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();

	m_pEm5000Trans = m_pEm5000.lock()->GetComponent<ENGINE::Transform>();

	m_pCollider = AddComponent<BoxCollider>();
	//m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);
	m_pCollider.lock()->SetCenter({ 0.f,0.1f,0.f });
	m_pCollider.lock()->SetSize({ 0.2f,0.15f,0.42f });
	PushEditEntity(m_pCollider.lock().get());
	return S_OK;
}

HRESULT Car::Start()
{
	m_pParentBone = m_pEm5000Mesh.lock()->GetToRootMatrixPtr("R_Hand");
	return S_OK;
}

UINT Car::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	Quaternion QuatIdentity{ 0.f,0.f,0.f,1.f };
	_StaticMesh->GetRootNode()->NodeUpdate(FMath::Identity(), 0.f, "", {}, QuatIdentity);
	_StaticMesh->UpdateToRootMatricies();
	_StaticMesh->VTFUpdate();
	
	//다 짜면 이렇게 바꾸셈
	if (m_pEm5000.lock()->Get_State() == Em5000::Attack_Grap_Car)
	{
		if (m_pEm5000Mesh.lock()->PlayingTime() >= 0.65f && m_bThrow == false)
		{
			// 손에 붙이기
			m_ParentWorld = m_pEm5000Trans.lock()->GetWorldMatrix();
			m_Result = (*m_pParentBone * m_ParentWorld);
			m_pTransform.lock()->SetWorldMatrix(m_Result);
		}
	}
	if (m_pEm5000.lock()->Get_State() == Em5000::Attack_Throw_Car)
	{

		// 손에 붙이기
		if (m_pEm5000Mesh.lock()->PlayingTime() <= 0.5f && m_bThrow == false)
		{
			m_ParentWorld = m_pEm5000Trans.lock()->GetWorldMatrix();
			m_Result = (*m_pParentBone * m_ParentWorld);
			m_pTransform.lock()->SetWorldMatrix(m_Result);
		}
	}
	if (m_pEm5000.lock()->Get_State() == Em5000::Car_Turn_L ||
		m_pEm5000.lock()->Get_State() == Em5000::Car_Turn_R)
		m_bCollEnable = true;


	if (Input::GetKeyDown(DIK_Y))
	{
		if (m_bBang == false)
			m_bBang = true;
		else
			m_bBang = false;
	}

	if (m_bBang)
		BangBang(_fDeltaTime);

		

	Throw(_fDeltaTime);
	return 0;
}

UINT Car::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Car::Editor()
{
	GameObject::Editor();
}


void Car::OnEnable()
{
	GameObject::OnEnable();
}

void Car::OnDisable()
{
	GameObject::OnDisable();
}

void Car::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Car::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Car::Throw(const float _fDeltaTime)
{
	if(m_bThrow)
	{
		Vector3 vDir = m_vPlayerPos - m_pTransform.lock()->GetPosition();
		float	fDir = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);
		//플레이어충돌or바닥 충돌되면으로 조건 바꿔야함
		if (fDir < 0.3f)
		{
			m_fThrowTime = 0.f;
			m_pTransform.lock()->SetRotation({ 0.f,0.f,0.f });
		}
		else
		{
			m_pTransform.lock()->Rotate({ 20.f, 0.f,0.f });
			m_pTransform.lock()->Translate(vDir * 0.1f);
		}

		if (m_pEm5000.lock()->Get_State() == Em5000::Attack_Throw_Car && m_pEm5000Mesh.lock()->PlayingTime()>=0.9f)
			m_bThrow = false;

	}
	else
	{
		if (m_pEm5000.lock()->Get_State() == Em5000::Attack_Throw_Car)
		{
			m_fThrowTime += _fDeltaTime;
			if (m_fThrowTime >= 1.f)
			{
				m_bThrow = true;
				m_vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
			}

		}
	}
}

void Car::BangBang(const float _fDeltaTime)
{
	m_pTransform.lock()->Rotate({ 10.f, 0.f,0.f });

	if (m_pTransform.lock()->GetPosition().y >= -1.85f &&
		m_pTransform.lock()->GetPosition().y <= -1.7f)
	{
		m_pCollider.lock()->AddForce(m_fBangPower * m_vBangDir);
		m_fBangPower -= 2.f;
	}
	/*if (m_pCollider.lock()->IsGround())
	{

	}*/
	if (m_fBangPower <= 0.f)
	{
		m_bBang = false;
		m_fBangPower = 8.f;
	}
}

