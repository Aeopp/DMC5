#include "stdafx.h"
#include "Monster.h"

#include <iostream>

#include "Nero.h"
#include "RedQueen.h"

void Monster::Free()
{
	Unit::Free();
}

std::string Monster::GetName()
{
	return "Monster";
}


void Monster::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void Monster::Set_Snatch(bool _bSnatch)
{
	m_bSnatch = _bSnatch;
}

void Monster::AddRankScore(float _fRankScore)
{
	//넘겨주는 값은 들어온 공격력 만큼인데 지금 너무낮아서 올려버림
	return m_pPlayer.lock()->AddRankScore(_fRankScore * 5.f);
}

Vector3 Monster::GetMonsterBoneWorldPos(std::string _BoneName)
{
	Vector3 WorldPos;
	memcpy(&WorldPos, (*m_pMesh->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix()).m[3], sizeof(Vector3));
	return WorldPos;
}
