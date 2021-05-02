#include "stdafx.h"
#include "Em5300Rain.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "Em5300.h"
#include "Nero.h"

void Em5300Rain::Free()
{
	GameObject::Free();
}

std::string Em5300Rain::GetName()
{
	return "Em5300Rain";
};

Em5300Rain* Em5300Rain::Create()
{
	return new Em5300Rain{};
};

void Em5300Rain::RenderReady()
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

			for (uint32 i = 0; i < Numsubset ;  ++i)
			{
				const auto& _Subset = m_pMesh->GetSubset(i); 
				const auto& _CurBS =_Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i]= _CurBS.Transform(_RenderUpdateInfo.World, Scale.x); 
			}
		}
	}
}

void Em5300Rain::RenderInit()
{
	m_nTag = MonsterWeapon;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ���� ) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
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
	// ����ƽ �޽� �ε�

	m_pMesh = Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Static\\Sphere.fbx");
	PushEditEntity(m_pMesh.get());
};

void Em5300Rain::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset =m_pMesh->GetNumSubset();
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
void Em5300Rain::RenderShadow(const DrawInfo& _Info)
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

void Em5300Rain::Rain(const float _fDeltaTime)
{
	if (m_bStartRain == false)
	{
		m_pTransform.lock()->Translate({ 0.f, 0.05f,0.f });

		m_fRainUpTime += _fDeltaTime;

		if(m_fRainUpTime>= 1.5f)
		{
			m_bStartRain = true;
			m_fRainUpTime = 0.f;
			m_fRainDownTime = 0.f;
			m_bRaindir = true;
		}

	}
	else
	{
		m_fRainDownTime += _fDeltaTime;
		//if (m_bRaindir)
		//{
		//	Vector3 vTemp = m_pTransform.lock()->GetPosition();
		//	Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
		//	m_pTransform.lock()->SetPosition({ vPlayerPos.x, vTemp.y, vPlayerPos.z });
		//	m_bRaindir = false;
		//}

		if (m_iRainPos >= 0.f && m_iRainPos < 3)
		{
			if (m_fRainDownTime >= 0.6f && m_fRainDownTime <= 0.61f)
				m_vPlayerPos[0] = m_pPlayerTrans.lock()->GetPosition();
		}
		if (m_iRainPos >= 3.f && m_iRainPos < 6)
		{
			if (m_fRainDownTime >= 1.2f && m_fRainDownTime <= 1.21f)
				m_vPlayerPos[1] = m_pPlayerTrans.lock()->GetPosition();
		}
		if (m_iRainPos >= 6.f && m_iRainPos < 9)
		{
			if (m_fRainDownTime >= 1.8f && m_fRainDownTime <= 1.81f)
				m_vPlayerPos[2] = m_pPlayerTrans.lock()->GetPosition();
		}
		if (m_iRainPos >= 9.f && m_iRainPos < 12)
		{
			if (m_fRainDownTime >= 2.4f && m_fRainDownTime <= 2.41f)
				m_vPlayerPos[3] = m_pPlayerTrans.lock()->GetPosition();
		}


		switch (m_iRainPos)
		{
		case 0:
			if (m_bRaindir && m_fRainDownTime >= 0.6f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[0].x += fRandomX;
				m_vPlayerPos[0].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[0].x, vTemp.y, m_vPlayerPos[0].z });
			
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 0.6f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 1:
			if (m_bRaindir && m_fRainDownTime >= 0.8f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[0].x += fRandomX;
				m_vPlayerPos[0].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[0].x, vTemp.y, m_vPlayerPos[0].z });
			

				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 0.8f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 2:
			if (m_bRaindir && m_fRainDownTime >= 1.f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[0].x += fRandomX;
				m_vPlayerPos[0].z += fRandomZ;


				m_pTransform.lock()->SetPosition({ m_vPlayerPos[0].x, vTemp.y, m_vPlayerPos[0].z });
				m_bRaindir = false;


			}
			else if (!m_bRaindir && m_fRainDownTime >= 1.f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 3:
			if (m_bRaindir && m_fRainDownTime >= 1.2f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[1].x += fRandomX;
				m_vPlayerPos[1].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[1].x, vTemp.y, m_vPlayerPos[1].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 1.2f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 4:
			if (m_bRaindir && m_fRainDownTime >= 1.4f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[1].x += fRandomX;
				m_vPlayerPos[1].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[1].x, vTemp.y, m_vPlayerPos[1].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 1.4f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 5:
			if (m_bRaindir && m_fRainDownTime >= 1.6f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[1].x += fRandomX;
				m_vPlayerPos[1].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[1].x, vTemp.y, m_vPlayerPos[1].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 1.6f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 6:
			if (m_bRaindir && m_fRainDownTime >= 1.8f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();
				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[2].x += fRandomX;
				m_vPlayerPos[2].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[2].x, vTemp.y, m_vPlayerPos[2].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 1.8f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 7:
			if (m_bRaindir && m_fRainDownTime >= 2.f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[2].x += fRandomX;
				m_vPlayerPos[2].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[2].x, vTemp.y, m_vPlayerPos[2].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 2.f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 8:
			if (m_bRaindir && m_fRainDownTime >= 2.2f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[2].x += fRandomX;
				m_vPlayerPos[2].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[2].x, vTemp.y, m_vPlayerPos[2].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 2.2f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 9:
			if (m_bRaindir && m_fRainDownTime >= 2.4f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[3].x += fRandomX;
				m_vPlayerPos[3].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[3].x, vTemp.y, m_vPlayerPos[3].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 2.4f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 10:
			if (m_bRaindir && m_fRainDownTime >= 2.6f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[3].x += fRandomX;
				m_vPlayerPos[3].z += fRandomZ;

				m_pTransform.lock()->SetPosition({ m_vPlayerPos[3].x, vTemp.y, m_vPlayerPos[3].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >=2.6f)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		case 11:
			if (m_bRaindir && m_fRainDownTime >= 2.8f)
			{
				Vector3 vTemp = m_pTransform.lock()->GetPosition();
				Vector3 vPlayerPos = m_pPlayerTrans.lock()->GetPosition();

				float fRandomX = FMath::Random<float>(-0.05f, 0.05f);
				float fRandomZ = FMath::Random<float>(-0.05f, 0.05f);

				m_vPlayerPos[3].x += fRandomX;
				m_vPlayerPos[3].z += fRandomZ;


				m_pTransform.lock()->SetPosition({ m_vPlayerPos[3].x, vTemp.y, m_vPlayerPos[3].z });
				m_bRaindir = false;
			}
			else if (!m_bRaindir && m_fRainDownTime >= 2.8)
			{
				if (m_pTransform.lock()->GetPosition().y >= -0.1f)
					m_pTransform.lock()->Translate({ 0.f, -0.04f, 0.f });
			}
			break;
		}
	}

	
}

void Em5300Rain::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5300Rain::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}


void Em5300Rain::RenderDebug(const DrawInfo& _Info)
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

};


HRESULT Em5300Rain::Ready()
{
	GameObject::Ready();
	m_nTag = MonsterWeapon;

	RenderInit();
	// Ʈ������ �ʱ�ȭ .. 
	m_pTransform = GetComponent<ENGINE::Transform>();
	m_pTransform.lock()->SetPosition(Vector3{/* -12.f,-0.9f,-638.f*/0.f,10.f,0.f });

	PushEditEntity(m_pTransform.lock().get());
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.



	return S_OK;
};

HRESULT Em5300Rain::Awake()
{
	GameObject::Awake();

	m_pEm5300Trasform = m_pEm5300.lock()->GetComponent<Transform>();
	

	if (m_iRainPos == 0)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_UpperArm_02_02");
	else if(m_iRainPos == 1)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_UpperArm_02_03");
	else if (m_iRainPos == 2)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Forearm_02_04");
	else if (m_iRainPos == 3)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Forearm_02_05");
	else if (m_iRainPos == 4)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Forearm_02_06");
	else if (m_iRainPos == 5)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Hand_02_07");
	else if (m_iRainPos == 6)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_UpperArm_02_02");
	else if (m_iRainPos == 7)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_UpperArm_02_03");
	else if (m_iRainPos == 8)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Forearm_02_04");
	else if (m_iRainPos == 9)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Forearm_02_05");
	else if (m_iRainPos == 10)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Forearm_02_06");
	else if (m_iRainPos == 11)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Hand_02_07");

	

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetGravity(false);;
	PushEditEntity(m_pCollider.lock().get());


	m_pCollider.lock()->SetRadius(0.015f);
	m_pTransform.lock()->SetScale({0.00015f,0.00015f,0.00015f});

	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();

	return S_OK;
}

HRESULT Em5300Rain::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT Em5300Rain::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (m_bReadyRain == false)
	{
		m_ParentWorld = m_pEm5300Trasform.lock()->GetWorldMatrix();
		m_Result = (*m_pParentBone * m_ParentWorld);
		m_pTransform.lock()->SetPosition({ m_Result._41, m_Result._42, m_Result._43 });
	}

	if (m_bReadyRain)
		Rain(_fDeltaTime);
	return 0;
}

UINT Em5300Rain::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void Em5300Rain::Editor()
{
	GameObject::Editor();
}


void Em5300Rain::OnEnable()
{
	GameObject::OnEnable();
}

void Em5300Rain::OnDisable()
{
	GameObject::OnDisable();
}
