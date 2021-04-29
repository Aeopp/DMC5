#include "stdafx.h"
#include "Monster.h"

#include <iostream>

#include "Nero.h"
#include "RedQueen.h"
#include "Subset.h"

void Monster::Free()
{
	Unit::Free();
}

std::string Monster::GetName()
{
	return "Monster";
};

void Monster::RenderReady()
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

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(
					_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
};

void Monster::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	if (GAMEOBJECTTAG::Player == _pOther.lock()->m_nTag)
	{
		static_pointer_cast<Nero>(_pOther.lock())->Set_GrabEnd(true);
	}
}

void Monster::SetGravity(bool _bActiveOrNot)
{
}

void Monster::Set_Snatch(bool _bSnatch)
{
	m_bSnatch = _bSnatch;
}

void Monster::AddRankScore(float _fRankScore)
{
	//�Ѱ��ִ� ���� ���� ���ݷ� ��ŭ�ε� ���� �ʹ����Ƽ� �÷�����
	return m_pPlayer.lock()->AddRankScore(_fRankScore * 5.f);
}

Vector3 Monster::GetMonsterBoneWorldPos(std::string _BoneName)
{
	Vector3 WorldPos;
	memcpy(&WorldPos, (*m_pMesh->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix()).m[3], sizeof(Vector3));
	return WorldPos;
}
