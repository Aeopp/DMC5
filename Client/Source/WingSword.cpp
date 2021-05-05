#include "stdafx.h"
#include "WingSword.h"

WingSword::WingSword()
{
	m_fLookOffset = 55.f;
}

void WingSword::Free()
{
	Unit::Free();
}

HRESULT WingSword::Ready()
{
	Unit::Ready();

	
	return S_OK;
}

HRESULT WingSword::Awake()
{
	Unit::Awake();
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	//부모 뼈 설정 해주고
		//R_Forearm
	m_pParentMat = m_pNero.lock()->Get_BoneMatrixPtr("root");
	m_pMesh->PlayAnimation("Idle", true);
	return S_OK;
}

HRESULT WingSword::Start()
{
	Unit::Start();
	return S_OK;
}

UINT WingSword::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);

	if (m_pMesh->IsAnimationEnd())
	{
		//SetActive(false);
		m_pMesh->PlayAnimation("Idle", true);
	}

	return 0;
}

UINT WingSword::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);
	if ("Idle" != m_pMesh->AnimName)
	{
		Matrix								ParentWorldMatrix, FinalWorld, Scale;

		ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

		D3DXMatrixScaling(&Scale, 0.9f, 0.9f, 0.9f);
		if (nullptr != m_pParentMat)
		{
			FinalWorld = Scale * *m_pParentMat * ParentWorldMatrix;
			m_pTransform.lock()->SetWorldMatrix(FinalWorld);
		}
	}
	else
	{
		Matrix								ParentWorldMatrix, FinalWorld, Scale,RotX,RotZ;

		ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

		D3DXMatrixScaling(&Scale, 0.7f, 0.7f, 0.7f);
		Vector3 vPlayerLook,vPlayerRight;
		memcpy(&vPlayerLook, ParentWorldMatrix.m[2], sizeof(Vector3));
		memcpy(&vPlayerRight, ParentWorldMatrix.m[0], sizeof(Vector3));
		D3DXMatrixRotationX(&RotX, D3DXToRadian(-15.f));
		D3DXMatrixRotationZ(&RotZ, D3DXToRadian(m_fRotationZ));

		if (nullptr != m_pParentMat)
		{
			FinalWorld = Scale * RotX * RotZ * *m_pParentMat * ParentWorldMatrix;
			FinalWorld._41 += vPlayerLook.x * m_fLookOffset + vPlayerRight.x * m_fRightOffset;
			FinalWorld._42 += m_fUpOffset;
			FinalWorld._43 += vPlayerLook.z * m_fLookOffset + vPlayerRight.z * m_fRightOffset;
			m_pTransform.lock()->SetWorldMatrix(FinalWorld);
		}
	}
	return 0;
}

void WingSword::OnEnable()
{
	Unit::OnEnable();
}

void WingSword::OnDisable()
{
	Unit::OnDisable();
}

void WingSword::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void WingSword::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetWorldMatrix();
	}
}

void WingSword::RenderGBufferSK(const DrawInfo& _Info)
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

void WingSword::RenderShadowSK(const DrawInfo& _Info)
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

void WingSword::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void WingSword::RenderDebugSK(const DrawInfo& _Info)
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

void WingSword::RenderInit()
{
}

void WingSword::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

void WingSword::StopAnimation()
{
	m_pMesh->StopAnimation();
}

void WingSword::ContinueAnimation()
{
	m_pMesh->ContinueAnimation();
}
