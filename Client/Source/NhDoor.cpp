#include "stdafx.h"
#include "..\Header\NhDoor.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "ParticleSystem.h"
#include <iostream>

void NhDoor::Free()
{
	GameObject::Free();
}

std::string NhDoor::GetName()
{
	return "NhDoor";
};

NhDoor* NhDoor::Create()
{
	return new NhDoor{};
};

void NhDoor::RenderReady()
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

			for (uint32 i = 0; i < Numsubset ;  ++i)
			{
				const auto& _Subset = _StaticMesh->GetSubset(i); 
				const auto& _CurBS =_Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i]= _CurBS.Transform(_RenderUpdateInfo.World, Scale.x); 
			}
		}
	}
}

void NhDoor::RenderInit()
{
	m_nTag = TAG_NhDoor;
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
		{"gbuffer_ds_dissolve",
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
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };


	/*�������� ���� ���� ������Ʈ�� �׸��ڸ� ĳ���� �Ѱ� �׷����ʴٸ� ���ּ���. */
	_InitRenderProp.bShadowCache = false;

	RenderInterface::Initialize(_InitRenderProp);

	// ����ƽ �޽� �ε�
	
	_StaticMesh = Resources::Load<ENGINE::StaticMesh>(
		"..\\..\\Resource\\Map\\Object\\Documents\\Nhdoor\\sm0382_nhbdoor.fbx");
	_DissolveMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Usable\\mesh_03_cs_noise_00_01.tga"
		);

	_BurnMap = Resources::Load<ENGINE::Texture>("..\\..\\Usable\\burn.tga");
	
	PushEditEntity(_StaticMesh.get());
};

void NhDoor::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;

	_Info.Fx->SetMatrix("matWorld", &World);
	_Info.Fx->SetTexture("DissolveMap", _DissolveMap->GetTexture());
	_Info.Fx->SetTexture("BurnMap", _BurnMap->GetTexture());

	_Info.Fx->SetFloatArray("BurnColor", BurnColor, 3u);
	_Info.Fx->SetFloat("BurnSize", BurnSize);
	_Info.Fx->SetFloat("EmissionAmount", EmissionAmount);
	_Info.Fx->SetFloat("SliceAmount", SliceAmount);
	

	const uint32 Numsubset =_StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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

void NhDoor::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
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
}



void NhDoor::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
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


HRESULT NhDoor::Ready()
{
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT NhDoor::Awake()
{
	//auto pCollider = AddComponent<CapsuleCollider>();
	//pCollider.lock()->ReadyCollider();
	//pCollider.lock()->SetRigid(true);
	//pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	//pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	//pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	//pCollider.lock()->SetRadius(100.f);
	//pCollider.lock()->SetHeight(200.f);
	m_pTransform.lock()->SetPosition(Vector3{-3328.f * GScale,1366.f * GScale,18028.f * GScale });
	m_pTransform.lock()->SetScale(Vector3{ 1.f * GScale,1.f * GScale,1.f * GScale });
	m_pTransform.lock()->SetRotation(Vector3{0.f,20.f,0.f});
	//PushEditEntity(pCollider.lock().get());
	return S_OK;
}

HRESULT NhDoor::Start()
{
	return S_OK;
}

UINT NhDoor::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	Vector3 vDir = m_pTransform.lock()->GetLook();

	D3DXVec3Normalize(&vDir, &vDir);
	if (Input::GetKey(DIK_W))
		m_pTransform.lock()->Translate(vDir * _fDeltaTime * 10.f);
	if (Input::GetKey(DIK_S))
		m_pTransform.lock()->Translate(-vDir * _fDeltaTime * 10.f);
	if (Input::GetKey(DIK_A))
		m_pTransform.lock()->Rotate({ 0.f, D3DXToRadian(180 * -_fDeltaTime * 50.f), 0.f });
	if (Input::GetKey(DIK_D))
		m_pTransform.lock()->Rotate({ 0.f, D3DXToRadian(180 * _fDeltaTime * 50.f), 0.f });
		
	std::cout << m_pTransform.lock()->GetPosition().y << std::endl;
	
	return 0;
}

UINT NhDoor::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void NhDoor::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		if (ImGui::Button("Dissolve Particle Play"))
		{
			DissolveParticle();
		}

		ImGui::SliderFloat("SliceAmount", &SliceAmount, 0.0f, 10.f);
		ImGui::SliderFloat("BurnSize", &BurnSize, 0.0f, 10.f);
		ImGui::SliderFloat("EmissionAmount", &EmissionAmount, 0.0f, 100.f);
		ImGui::ColorEdit3("BurnColor", BurnColor);
	};
}


void NhDoor::OnEnable()
{
	GameObject::OnEnable();
}

void NhDoor::OnDisable()
{
	GameObject::OnDisable();
}


void NhDoor::DissolveParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle("DissolveNhDoor", 1000ul, true);
			_Particle.empty() == false)
		{
			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
			}
		}
	};

}


