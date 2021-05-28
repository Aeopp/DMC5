#include "stdafx.h"
#include "Monster.h"

#include <iostream>

#include "Nero.h"
#include "RedQueen.h"
#include "Subset.h"
#include "StoneDebris.h"
#include "SpriteEffect.h"
#include "ShockWave.h"
#include "SoundSystem.h"
#include "StoneDebrisMulti.h"
void Monster::Free()
{
	//for (auto& Element : m_pStoneDebrisVec)
	//{
	//	if (!Element.expired())
	//		Destroy(Element);
	//}
	m_pStoneDebrisVec.clear();
	m_pStoneDebrisVec.shrink_to_fit();

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

	if (GAMEOBJECTTAG::Player == _pOther.lock()->m_nTag)
		return;
	if (!m_bBusterStoneStart)
		return;
	m_bBusterStoneStart = false;
	UINT _PlayerAnimationIndex = m_pPlayer.lock()->Get_CurAnimationIndex();

	switch (_PlayerAnimationIndex)
	{
	case Nero::ANI_BUSTER_STRIKE_COMMON_AIR:
	case Nero::ANI_EM0000_BUSTER_AIR:
	case Nero::ANI_EM200_BUSTER_AIR_FINISH:
		PlayBusterEffect();
		break;
	}
}

void Monster::HitEffectPlay(std::weak_ptr<GameObject> _pOther)
{
	switch (_pOther.lock()->m_nTag)
	{
	case TAG_RedQueen:
		m_pEffect[3].lock()->PlayStart(0, m_pTransform.lock()->GetPosition());
		m_pHitWave[m_iWaveIndex].lock()->PlayStart(m_pTransform.lock()->GetPosition(), ShockWave::Option::Hit, true);
		++m_iWaveIndex;
		m_iWaveIndex %= 3;
		m_pPlayer.lock()->Add_ExGauge(0.25f);
		{
			int RandomPlaySound = rand() % 5;
			switch (RandomPlaySound)
			{
			case 0:
				SoundSystem::GetInstance()->RandSoundKeyPlay("RedQueenHit1", { 1,4 }, 0.2f, false);
				break;
			case 1:
				SoundSystem::GetInstance()->RandSoundKeyPlay("RedQueenHit2", { 1,4 }, 0.2f, false);
				break;
			case 2:
				SoundSystem::GetInstance()->RandSoundKeyPlay("RedQueenHit3", { 1,4 }, 0.2f, false);
				break;
			case 3:
				SoundSystem::GetInstance()->RandSoundKeyPlay("RedQueenHit4", { 1,4 }, 0.2f, false);
				break;
			case 4:
				SoundSystem::GetInstance()->RandSoundKeyPlay("RedQueenHit5", { 1,4 }, 0.2f, false);
				break;
			}
		}
		break;
	case Tag_Cbs_Long:
	{
		int RandomPlaySound = rand() % 11 + 1;
		string temp("CbsHit" + std::to_string(RandomPlaySound));
		SoundSystem::GetInstance()->RandSoundKeyPlay(temp, { 1,4 }, 0.2f, false);
	}
		m_pEffect[0].lock()->PlayStart(0, m_pTransform.lock()->GetPosition());

		m_pHitWave[m_iWaveIndex].lock()->PlayStart(m_pTransform.lock()->GetPosition(), ShockWave::Option::Hit, true);
		++m_iWaveIndex;
		m_iWaveIndex %= 3;
		break;
	case Tag_Cbs_Middle:
	{
		int RandomPlaySound = rand() % 11 + 1;
		string temp("CbsHit" + std::to_string(RandomPlaySound));
		SoundSystem::GetInstance()->RandSoundKeyPlay(temp, { 1,4 }, 0.2f, false);
	}
		m_pEffect[4].lock()->PlayStart(0, m_pTransform.lock()->GetPosition());
		m_pHitWave[m_iWaveIndex].lock()->PlayStart(m_pTransform.lock()->GetPosition(), ShockWave::Option::Hit, true);
		++m_iWaveIndex;
		m_iWaveIndex %= 3;
		break;
	case Tag_Cbs_Short:
	{
		int RandomPlaySound = rand() % 11 + 1;
		string temp("CbsHit" + std::to_string(RandomPlaySound));
		SoundSystem::GetInstance()->RandSoundKeyPlay(temp, { 1,4 }, 0.2f, false);
	}
		m_pEffect[5].lock()->PlayStart(0, m_pTransform.lock()->GetPosition());
		m_pHitWave[m_iWaveIndex].lock()->PlayStart(m_pTransform.lock()->GetPosition(), ShockWave::Option::Hit, true);
		++m_iWaveIndex;
		m_iWaveIndex %= 3;
		break;
	case Overture:
	{
		int RandomPlaySound = rand() % 11 + 1;
		string temp("CbsHit" + std::to_string(RandomPlaySound));
		SoundSystem::GetInstance()->RandSoundKeyPlay(temp, { 1,4 }, 0.2f, false);
	}
		m_pEffect[1].lock()->PlayStart(0, m_pTransform.lock()->GetPosition());
		m_pHitWave[m_iWaveIndex].lock()->PlayStart(m_pTransform.lock()->GetPosition(), ShockWave::Option::Hit, true);
		++m_iWaveIndex;
		m_iWaveIndex %= 3;
		break;
	case TAG_BusterArm_Right:
		m_pEffect[3].lock()->PlayStart(0, m_pTransform.lock()->GetPosition());
		m_pHitWave[m_iWaveIndex].lock()->PlayStart(m_pTransform.lock()->GetPosition(), ShockWave::Option::Hit, true);
		++m_iWaveIndex;
		m_iWaveIndex %= 3;
		break;
	default:

		break;
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
	//넘겨주는 값은 들어온 공격력 만큼인데 지금 너무낮아서 올려버림
	return m_pPlayer.lock()->AddRankScore(_fRankScore * 1.f);
}

void Monster::StoneDebrisInit()
{
	for (auto& Element : m_pStoneDebrisVec)
	{
		if (!Element.expired())
			Destroy(Element);
	}
	m_pStoneDebrisVec.clear();

	m_pStoneDebrisVec.reserve(16u);

	uint32 StoneDebrisCnt = FMath::Random<uint32>(8u, 12u);
	for (uint32 i = 0u; i < StoneDebrisCnt; ++i)
	{
		weak_ptr<StoneDebris> p = AddGameObject<StoneDebris>();
		p.lock()->SetVariationIdx((StoneDebris::VARIATION)FMath::Random<uint32>((uint32)StoneDebris::REDORB_0, (uint32)StoneDebris::REDORB_3));
		p.lock()->SetScale(FMath::Random<float>(0.002f, 0.004f));
		p.lock()->SetRotation(FMath::Random<Vector3>(Vector3(0.f, 0.f, 0.f), Vector3(180.f, 180.f, 180.f)));
		// position은 죽을 때 위치
		p.lock()->SetVelocity(FMath::Random<Vector3>(Vector3(-0.12f, 0.145f, -0.12f), Vector3(0.12f, 0.16f, 0.12f)));
		p.lock()->SetActive(false);
		m_pStoneDebrisVec.push_back(p);
	}
	StoneDebrisCnt = FMath::Random<uint32>(0u, 2u);
	for (uint32 i = 0u; i < StoneDebrisCnt; ++i)
	{
		weak_ptr<StoneDebris> p = AddGameObject<StoneDebris>();
		p.lock()->SetVariationIdx((StoneDebris::VARIATION)FMath::Random<uint32>((uint32)StoneDebris::GREENORB_0, (uint32)StoneDebris::GREENORB_3));
		p.lock()->SetScale(FMath::Random<float>(0.002f, 0.004f));
		p.lock()->SetRotation(FMath::Random<Vector3>(Vector3(0.f, 0.f, 0.f), Vector3(180.f, 180.f, 180.f)));
		// position은 죽을 때 위치
		p.lock()->SetVelocity(FMath::Random<Vector3>(Vector3(-0.12f, 0.145f, -0.12f), Vector3(0.12f, 0.16f, 0.12f)));
		p.lock()->SetActive(false);
		m_pStoneDebrisVec.push_back(p);
	}
	StoneDebrisCnt = FMath::Random<uint32>(0u, 2u);
	for (uint32 i = 0u; i < StoneDebrisCnt; ++i)
	{
		weak_ptr<StoneDebris> p = AddGameObject<StoneDebris>();
		p.lock()->SetVariationIdx((StoneDebris::VARIATION)FMath::Random<uint32>((uint32)StoneDebris::WHITEORB_0, (uint32)StoneDebris::WHITEORB_3));
		p.lock()->SetScale(FMath::Random<float>(0.002f, 0.003f));
		p.lock()->SetRotation(FMath::Random<Vector3>(Vector3(0.f, 0.f, 0.f), Vector3(180.f, 180.f, 180.f)));
		// position은 죽을 때 위치
		p.lock()->SetVelocity(FMath::Random<Vector3>(Vector3(-0.12f, 0.145f, -0.12f), Vector3(0.12f, 0.16f, 0.12f)));
		p.lock()->SetActive(false);
		m_pStoneDebrisVec.push_back(p);
	}

	m_bStoneDebrisPlayStart = false;
}

void Monster::StoneDebrisPlayStart()
{
	if (m_bStoneDebrisPlayStart)
		return;

	Vector3 CurPos = m_pTransform.lock()->GetPosition();
	for (auto& Element : m_pStoneDebrisVec)
	{
		Element.lock()->SetPosition(CurPos);
		Element.lock()->SetActive(true);
		Element.lock()->PlayStart();
	}

	m_bStoneDebrisPlayStart = true;
}

void Monster::CalcEffectPos()
{
	m_vEffectPos = FMath::RandomVector(FMath::Random(0.008f, 0.016f));
}

void Monster::PlayBusterEffect()
{
	//BusterEffect
	for (int i = 0; i < 6; ++i)
	{
		float fRandom = FMath::Random<float>(0.0005f, 0.001f);
		Vector3 vRot2 = FMath::Random<Vector3>(Vector3{ 0.f,0.f,0.f }, Vector3{ 180.f,180.f,180.f });
		m_pBusterStone[i].lock()->SetPosition(m_pTransform.lock()->GetPosition());
		m_pBusterStone[i].lock()->SetScale(fRandom);
		m_pBusterStone[i].lock()->PlayStart(40.f);
		m_pBusterStone[i].lock()->SetRotation(vRot2);
	}
	m_pBusterDust.lock()->PlayStart(0.0f, m_pTransform.lock()->GetPosition());
	m_pEffect[3].lock()->PlayStart(0, m_pTransform.lock()->GetPosition());
	m_pHitWave[m_iWaveIndex].lock()->PlayStart(m_pTransform.lock()->GetPosition(), ShockWave::Option::Buster, true);
	++m_iWaveIndex;
	m_iWaveIndex %= 3;
}

Vector3 Monster::GetMonsterBoneWorldPos(std::string _BoneName)
{
	Vector3 WorldPos;
	memcpy(&WorldPos, (*m_pMesh->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix()).m[3], sizeof(Vector3));
	return WorldPos;
}

HRESULT Monster::Ready()
{
	Unit::Ready();
	for (int i = 0; i < 6; ++i)
	{
		m_pEffect[i] = AddGameObject<SpriteEffect>();
		m_pEffect[i].lock()->InitializeFromOption(i);
	}
	for (int i = 0; i < 3; ++i)
		m_pHitWave[i] = AddGameObject<ShockWave>();

	m_pBusterDust = AddGameObject<SpriteEffect>();
	m_pBusterDust.lock()->InitializeFromOption(6);

	for (int i = 0; i < 6; ++i)
		m_pBusterStone[i] = AddGameObject<StoneDebrisMulti>();

	return E_NOTIMPL;
}
