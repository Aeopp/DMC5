#include "stdafx.h"

#include "..\Header\CbsMidTrail.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "Cbs_Short.h"
#include "ParticleSystem.h"
#include "Cbs_Middle.h"
#include "Thunderbolt.h"
#include "ThunderboltSecond.h"
#include "ElectricBranch.h"
#include "ElectricOccur.h"
#include "ElectricVortex.h"

CbsMidTrail::CbsMidTrail()
{
	VtxBuffers.fill(nullptr);
	IdxBuffers.fill(nullptr);
}

void CbsMidTrail::Free()
{
	GameObject::Free();

	for (auto& _VtxBuf : VtxBuffers)
	{
		if (_VtxBuf)
		{
			_VtxBuf->Release();
		}
	}
	for (auto& _IdxBuf : IdxBuffers)
	{
		if (_IdxBuf)
		{
			_IdxBuf->Release();
		}
	}

	if (VtxDecl)
	{
		VtxDecl->Release();
	}
};

std::string CbsMidTrail::GetName()
{
	return "CbsMidTrail";
};

CbsMidTrail* CbsMidTrail::Create()
{
	return new CbsMidTrail{};
};

void CbsMidTrail::RenderReady()
{
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� .
	_RenderUpdateInfo.World = FMath::Identity();
};

void CbsMidTrail::RenderInit()
{
	m_nTag = Eff_CbsMidTrail;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"CbsMidTrail",[this](const DrawInfo& _Info)
			{
				this->RenderTrail(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	const int32 TriCnt = 36;

	_Desc.VtxSize = sizeof(Vertex::TrailVertex);
	_Desc.VtxCnt = TriCnt + 2;
	// �ݵ�� ¦���� ��Ī . 
	_Desc.TriCnt = TriCnt;
	_Desc.DrawTriCnt = TriCnt;
	_Desc.IdxSize = sizeof(Vertex::Index32);
	_Desc.IdxFmt = D3DFMT_INDEX32;

	_Desc.UpdateCycle = 0.016f;
	_Desc.NewVtxCnt = 0;
	UV0Multiply = 1.f;

	Device = g_pDevice;
	VtxDecl = Vertex::TrailVertex::GetVertexDecl(Device);

	for (uint32 i = 0; i < BoneCnt; ++i)
	{
		Device->CreateVertexBuffer
		(_Desc.VtxSize * _Desc.VtxCnt, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			NULL,
			D3DPOOL_DEFAULT,
			&VtxBuffers[i], nullptr);

		Device->CreateIndexBuffer
		(_Desc.IdxSize * _Desc.TriCnt
			, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, _Desc.IdxFmt,
			D3DPOOL_DEFAULT, &IdxBuffers[i], nullptr);
	}

	TrailMap = Resources::Load<Texture>(
		"..\\..\\Usable\\tex_03_common_000_0002_alpg.tga");
	IceMap = Resources::Load<Texture >(
		  "..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");
	IceTrailMap = Resources::Load<Texture >(
		"..\\..\\Usable\\tex_03_common_000_0000_alpg.tga");
	EmissiveMskMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\emissive_msk.tga");
	NoiseMap = Resources::Load<Texture>(
		"..\\..\\Usable\\noisesample_msk.tga");
};

void CbsMidTrail::PlayStart(const Mode _Mode,
	const std::optional<Vector3>& Location)
{
	if (Location)
	{
		GetComponent<Transform>().lock()->SetPosition(Location.value());
	}

	if (auto _GameObject = FindGameObjectWithTag(Tag_Cbs_Middle).lock();
		_GameObject)
	{
		if (auto _CbsMiddle = std::dynamic_pointer_cast<Cbs_Middle>(_GameObject);
			_CbsMiddle)
		{
			const auto _CbsWorld = _CbsMiddle->GetComponent<Transform>().lock()->GetWorldMatrix();

			for (int32 i = 0; i < BoneCnt; ++i)
			{
				Vertex::TrailVertex* VtxPtr{};
				auto& VtxBuffer = VtxBuffers[i];
				VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);

				auto Low = _CbsMiddle->Get_BoneMatrixPtr(BoneNames[i]);
				LatelyOffsets[i].first = FMath::Mul(Offset[CurMode].first, *Low * _CbsWorld);

				auto High = Low;
				LatelyOffsets[i].second = FMath::Mul(Offset[CurMode].second, *High * _CbsWorld);

				for (int32 j = 0; j  < _Desc.VtxCnt; j+=2)
				{
					VtxPtr[j + 1].Location = LatelyOffsets[i].second;
					VtxPtr[j].Location = LatelyOffsets[i].first;
				};

				VtxBuffer->Unlock();
			}
		}
	};

	for (auto& _IdxBuffer : IdxBuffers)
	{
		void* IdxPtr{ nullptr };
		_IdxBuffer->Lock(0, 0, (void**)&IdxPtr, NULL);
		ZeroMemory(IdxPtr, _Desc.IdxSize * _Desc.TriCnt);
		_IdxBuffer->Unlock();
	}

	CurMode = static_cast<uint32>(_Mode);
	_RenderProperty.bRender = true;
	T = 0.0f;
	_Desc.NewVtxCnt = 0;
	_Desc.UpdateCycle = _Desc.CurVtxUpdateCycle = 0.016f;

	CurEffectParticleIdx = 0u;
	CurEffectParticleCycle = 0.0f;

};

void CbsMidTrail::PlayEnd()
{
	_RenderProperty.bRender = false;
};

void CbsMidTrail::RenderTrail(const DrawInfo& _Info)
{
	if (T <= 0.25f)
		return;

	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	if (CurMode == Mode::Non)
	{
		_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
		_Info.Fx->SetTexture("TrailMap", IceTrailMap->GetTexture());
		_Info.Fx->SetTexture("AlbmMap", IceMap->GetTexture());

		_Info.Fx->SetTexture("NoiseMap", NoiseMap->GetTexture());
		_Info.Fx->SetFloatArray("ScrollSpeed", ScrollSpeed, 3);
		_Info.Fx->SetFloatArray("Scale", Scale, 3);

		_Info.Fx->SetFloatArray("NoiseDistortion0", NoiseDistortion0, 2u);
		_Info.Fx->SetFloatArray("NoiseDistortion1", NoiseDistortion1, 2u);
		_Info.Fx->SetFloatArray("NoiseDistortion2", NoiseDistortion2, 2u);

		_Info.Fx->SetFloat("EmissiveIntencity", EmissiveIntencity);
	}

	_Info.Fx->SetTexture("EmissiveMskMap", EmissiveMskMap->GetTexture());
	_Info.Fx->SetVector("_Color", &_Color);
	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);

	for (int32 i = 0; i < BoneCnt; ++i)
	{
		Device->SetStreamSource(0, VtxBuffers[i], 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffers[i]);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.NewVtxCnt);
	}
}

void CbsMidTrail::BufferUpdate(const float DeltaTime)
{
	_Desc.CurVtxUpdateCycle -= DeltaTime;

	if (_Desc.CurVtxUpdateCycle < 0.0f)
	{
		_Desc.CurVtxUpdateCycle += _Desc.UpdateCycle;

		IndexBufUpdate();
		VertexBufUpdate();
	}
}
void CbsMidTrail::ParticleUpdate(const float DeltaTime)
{
	CurParticleCycle -= DeltaTime;

	if (CurParticleCycle)
	{
		CurParticleCycle += ParticleCycle;

		if (auto SpTransform = m_pTransform.lock();
			SpTransform)
		{
			auto _PlayableParticle =
				ParticleSystem::GetInstance()->PlayParticle("CbsMid", 300ul, true);
			// ���� �Ѹ���

			for (int32 i = 0; i < _PlayableParticle.size(); ++i)
			{
				auto& _PlayInstance = _PlayableParticle[i];

				const uint32 BoneIdx = FMath::Random(0u, BoneCnt - 1);

				const Vector3 WorldLocation =
					FMath::Lerp(LatelyOffsets[BoneIdx].first,
						LatelyOffsets[BoneIdx].second,
						FMath::Random(0.5f, 1.f));

				const Vector3 Scale = SpTransform->GetScale();
				const Vector3 Rotation = Vector3{ 0.f,0.f,0.f };

				_PlayInstance->PlayDescBind(
					FMath::WorldMatrix(Scale, Rotation, WorldLocation));
			}
		}
	}
};

void CbsMidTrail::EffectParticleUpdate(const float DeltaTime)
{
	CurEffectParticleCycle -= DeltaTime;

	if (CurEffectParticleCycle < 0.0f)
	{
		CurEffectParticleCycle += EffectParticleCycle;
		
		
		int32 Count = 0;
		while (true)
		{
			auto& RefEffectDesc = _PlayEffectDescs[CurEffectParticleIdx];

			if (RefEffectDesc.bPlayEnd)
			{
				RefEffectDesc.bPlayEnd = false;
				RefEffectDesc.T = 0.0f;

				if (Eff_ElectricBranch == RefEffectDesc._Tag)
				{
					if (
						auto SpEffect = 
						std::dynamic_pointer_cast<ElectricBranch>
						(RefEffectDesc._Effect.lock()))
					{
						const uint32 TargetBoneIdx =
							FMath::Random(0u, BoneCnt - 1u);

						const uint32 TargetIdx =
							FMath::Random(0ull, 
								_TrailVtxWorldLocations[TargetBoneIdx].size()-1u);

						const Vector3 TargetLocation =
							_TrailVtxWorldLocations[TargetBoneIdx][TargetIdx].Location;

						SpEffect->PlayStart(RefEffectDesc.LocationOffset
						 + TargetLocation);
					}
				}
				else if (Eff_ElectricOccur == RefEffectDesc._Tag)
				{
					if (
						auto SpEffect =
						std::dynamic_pointer_cast<ElectricOccur>
						(RefEffectDesc._Effect.lock()))
					{
						const uint32 TargetBoneIdx =
							FMath::Random(0u, BoneCnt - 1u);

						const uint32 TargetIdx =
							FMath::Random(0ull,
								_TrailVtxWorldLocations[TargetBoneIdx].size() - 1u);

						const Vector3 TargetLocation =
							_TrailVtxWorldLocations[TargetBoneIdx][TargetIdx].Location;

						SpEffect->PlayStart(RefEffectDesc.LocationOffset
							+ TargetLocation);
					}
				}
				else if (Eff_ElectricVortex == RefEffectDesc._Tag)
				{
					if (
						auto SpEffect =
						std::dynamic_pointer_cast<ElectricVortex>
						(RefEffectDesc._Effect.lock()))
					{
						const uint32 TargetBoneIdx =
							FMath::Random(0u, BoneCnt - 1u);

						const uint32 TargetIdx =
							FMath::Random(0ull,
								_TrailVtxWorldLocations[TargetBoneIdx].size() - 1u);

						const Vector3 TargetLocation =
							_TrailVtxWorldLocations[TargetBoneIdx][TargetIdx].Location;

						SpEffect->PlayStart(RefEffectDesc.LocationOffset
							+ TargetLocation);
					}
				}
				else if (Eff_ThunderBolt == RefEffectDesc._Tag)
				{
					if (
						auto SpEffect =
						std::dynamic_pointer_cast<ThunderBolt>
						(RefEffectDesc._Effect.lock()))
					{
						const uint32 TargetBoneIdx =
							FMath::Random(0u, BoneCnt - 1u);

						const uint32 TargetIdx =
							FMath::Random(0ull,
								_TrailVtxWorldLocations[TargetBoneIdx].size() - 1u);

						const Vector3 TargetLocation =
							_TrailVtxWorldLocations[TargetBoneIdx][TargetIdx].Location;

						SpEffect->PlayStart(RefEffectDesc.LocationOffset
							+ TargetLocation);
					}
				}
				else if (Eff_ThunderBoltSecond == RefEffectDesc._Tag)
				{
					if (
						auto SpEffect =
						std::dynamic_pointer_cast<ThunderBoltSecond>
						(RefEffectDesc._Effect.lock()))
					{
						const uint32 TargetBoneIdx =
							FMath::Random(0u, BoneCnt - 1u);

						const uint32 TargetIdx =
							FMath::Random(0ull,
								_TrailVtxWorldLocations[TargetBoneIdx].size() - 1u);

						const Vector3 TargetLocation =
							_TrailVtxWorldLocations[TargetBoneIdx][TargetIdx].Location;

						SpEffect->PlayStart(RefEffectDesc.LocationOffset
							+ TargetLocation);
					}
				}

				break;
			}
			else
			{

				++CurEffectParticleIdx;
				CurEffectParticleIdx %= EffectParticleCount;
			}

			++Count;

			if (Count >= EffectParticleCount)
				break;
		}	
	}


	for (auto& _Element : _PlayEffectDescs)
	{
		if (_Element.bPlayEnd == false)
		{
			_Element.T += DeltaTime;
			if (_Element.T >= _Element.PlayTime)
			{
				_Element.bPlayEnd = true;
			}
		}
	}
};

void CbsMidTrail::VtxSplineInterpolation(Vertex::TrailVertex* const VtxPtr)
{
	// � ���� .....
	for (int32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		{
			Vector3 VtxPt{};

			const int32 p0 = std::clamp<int32>(i - 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p1 = std::clamp<int32>(i, 0, _Desc.NewVtxCnt - 1);
			const int32 p2 = std::clamp<int32>(i + 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p3 = std::clamp<int32>(i + 4, 0, _Desc.NewVtxCnt - 1);

			D3DXVec3CatmullRom(
				&VtxPt,
				&VtxPtr[p0].Location,
				&VtxPtr[p1].Location,
				&VtxPtr[p2].Location,
				&VtxPtr[p3].Location,
				CurveT);

			VtxPtr[i].Location = VtxPt;
		}

		{
			Vector3 VtxPt{};

			const int32 p0 = std::clamp<int32>((i + 1) - 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p1 = std::clamp<int32>((i + 1), 0, _Desc.NewVtxCnt - 1);
			const int32 p2 = std::clamp<int32>((i + 1) + 2, 0, _Desc.NewVtxCnt - 1);
			const int32 p3 = std::clamp<int32>((i + 1) + 4, 0, _Desc.NewVtxCnt - 1);

			D3DXVec3CatmullRom(
				&VtxPt,
				&VtxPtr[p0].Location,
				&VtxPtr[p1].Location,
				&VtxPtr[p2].Location,
				&VtxPtr[p3].Location,
				CurveT);

			VtxPtr[i + 1].Location = VtxPt;
		}
	}
};

void CbsMidTrail::VertexBufUpdate()
{
	for (int32 i = 0; i < BoneCnt; ++i)
	{
		auto& VtxBuffer = VtxBuffers[i];

		Vertex::TrailVertex* VtxPtr{};
		VtxBuffer->Lock(0, 0, (void**)&VtxPtr, 0);
		// �ִ� ���ؽ� ī��Ʈ�� �ʰ��� ��� .

		if (_Desc.NewVtxCnt > _Desc.VtxCnt)
		{
			static constexpr uint32 RemoveCount = 2;
			_Desc.NewVtxCnt -= RemoveCount;

			for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
			{
				VtxPtr[i].Location = VtxPtr[RemoveCount + i].Location;
				VtxPtr[i + 1].Location = VtxPtr[RemoveCount + i + 1].Location;
			}
		}

		if (auto _GameObject = FindGameObjectWithTag(Tag_Cbs_Middle).lock();
			_GameObject)
		{
			if (auto _CbsShort = std::dynamic_pointer_cast<Cbs_Middle>(_GameObject);
				_CbsShort)
			{
				const auto SwordWorld = _CbsShort->GetComponent<Transform>().lock()->GetWorldMatrix();

				auto Low = _CbsShort->Get_BoneMatrixPtr(BoneNames[i]);
				
				LatelyOffsets[i].first = FMath::Mul(Offset[CurMode].first, *Low * SwordWorld);

				auto High = Low;
				LatelyOffsets[i].second = FMath::Mul(Offset[CurMode].second, *High * SwordWorld);

				VtxPtr[_Desc.NewVtxCnt + 1].Location = LatelyOffsets[i].second;
				VtxPtr[_Desc.NewVtxCnt].Location = LatelyOffsets[i].first;
			}
		};

		_Desc.NewVtxCnt += 2;
		VtxUVCalc(VtxPtr);
		VtxSplineInterpolation(VtxPtr);

		
			for (int32 i = 0; i < BoneCnt; ++i)
			{
				auto& _CurVtxLog = _TrailVtxWorldLocations[i];
				_CurVtxLog.resize(_Desc.NewVtxCnt);
				for (size_t j = 0; j < _CurVtxLog.size(); ++j)
				{
					_CurVtxLog[j] = VtxPtr[j];
				}
			}

		VtxBuffer->Unlock();
	};

};

void CbsMidTrail::IndexBufUpdate()
{
	/*
		1 3 5 7 9 11.......
		0 2 4 6 8 10 .....

		1�� �ﰢ��
		1 ��  3
		�� ��
		0

		2�� �ﰢ��
		   3
		 �� ��
		0 �� 2
		*/
	for (uint32 i = 0; i < BoneCnt; ++i)
	{
		Vertex::Index32* IdxPtr{ nullptr };
		auto& IdxBuffer = IdxBuffers[i];
		IdxBuffer->Lock(0, 0, (void**)&IdxPtr, 0);

		for (uint32 i = 0; i < _Desc.TriCnt; i += 2)
		{
			IdxPtr[i]._0 = i;
			IdxPtr[i]._1 = i + 1;
			IdxPtr[i]._2 = i + 3;

			IdxPtr[i + 1]._0 = i;
			IdxPtr[i + 1]._1 = i + 3;
			IdxPtr[i + 1]._2 = i + 2;
		}

		if (bEdit)
		{
			auto& _CurIdxLog = _IdxLog[i];
			_CurIdxLog.resize(_Desc.TriCnt);
			for (size_t j = 0; j < _CurIdxLog.size(); ++j)
			{
				_CurIdxLog[j] = IdxPtr[j];
			}
		}

		IdxBuffer->Unlock();
	}
};

void CbsMidTrail::VtxUVCalc(Vertex::TrailVertex* const VtxPtr)
{
	for (uint32 i = 0; i < _Desc.NewVtxCnt; i += 2)
	{
		VtxPtr[i + 1].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)) ,0.f };
		VtxPtr[i].UV0 = { ((float)i / ((float)_Desc.NewVtxCnt - 2)),1.f };

		VtxPtr[i + 1].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2),0.f };
		VtxPtr[i].UV1 = { (float)i / ((float)_Desc.NewVtxCnt - 2) ,1.f };
	}
};

void CbsMidTrail::RenderDebug(const DrawInfo& _Info)
{
	if (T <= 0.25f)
		return;

	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	for (int32 i = 0; i < BoneCnt; ++i)
	{
		auto& VtxBuffer = VtxBuffers[i];
		auto& IdxBuffer = IdxBuffers[i];

		Device->SetStreamSource(0, VtxBuffer, 0, _Desc.VtxSize);
		Device->SetVertexDeclaration(VtxDecl);
		Device->SetIndices(IdxBuffer);
		_Info.Fx->CommitChanges();
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _Desc.VtxCnt, 0, _Desc.DrawTriCnt);
	}
};


HRESULT CbsMidTrail::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	PushEditEntity(InitTransform.lock().get());
	RenderInit();

	for (auto& _DescElement : _PlayEffectDescs)
	{
		CbsMidTrail::EffectDesc _Desc{};
		_Desc.LocationOffset = FMath::Random(
			Vector3{ -0.001f,-0.001f,-0.001f }, Vector3{ 0.001f,0.001f,0.001f });
		_Desc.bPlayEnd = true;

		const uint32 Dice = FMath::Random(1u, 4u);

		 if (Dice == 1u)
		{
			_Desc._Effect = AddGameObject<ThunderBolt>();
			_Desc._Tag = GAMEOBJECTTAG::Eff_ThunderBolt;
			if (auto _TargetEffect = std::dynamic_pointer_cast<ThunderBolt>(_Desc._Effect.lock());
				_TargetEffect)
			{
				_Desc.PlayTime = _TargetEffect->GetPlayTime();
				_TargetEffect->SetEditable(false);
				auto TargetTransform = _TargetEffect->GetComponent<Transform>().lock();
				TargetTransform->
					SetScale(Vector3{
						GScale * 0.01f,
						GScale * 0.01f,
						GScale * 0.01f });
				TargetTransform->SetRotation(FMath::RandomEuler(1.f));
				_TargetEffect->PtLightFlux = -1.f;
			}
		}
		else if (Dice == 2u)
		{
			_Desc._Effect = AddGameObject<ElectricOccur>();
			_Desc._Tag = GAMEOBJECTTAG::Eff_ElectricOccur;
			if (auto _TargetEffect = std::dynamic_pointer_cast<ElectricOccur>(_Desc._Effect.lock());
				_TargetEffect)
			{
				_Desc.PlayTime = _TargetEffect->GetPlayTime();
				_TargetEffect->SetEditable(false);
				auto TargetTransform = _TargetEffect->GetComponent<Transform>().lock();
				TargetTransform->
					SetScale(Vector3{
						GScale * 0.01f,
						GScale * 0.01f,
						GScale * 0.01f });
				TargetTransform->SetRotation(FMath::RandomEuler(1.f));
				_TargetEffect->PtLightFlux = -1.f;
			}
		}
		else if (Dice == 3u)
		{
			_Desc._Effect = AddGameObject<ElectricBranch>();
			_Desc._Tag = GAMEOBJECTTAG::Eff_ElectricBranch;
			if (auto _TargetEffect = std::dynamic_pointer_cast<ElectricBranch>(_Desc._Effect.lock());
				_TargetEffect)
			{
				_Desc.PlayTime = _TargetEffect->GetPlayTime();
				_TargetEffect->SetEditable(false);
				auto TargetTransform = _TargetEffect->GetComponent<Transform>().lock();
				TargetTransform->
					SetScale(Vector3{
						GScale * 0.01f,
						GScale * 0.01f,
						GScale * 0.01f });
				TargetTransform->SetRotation(FMath::RandomEuler(1.f));
				_TargetEffect->PtLightFlux = -1.f;
			}
		}
		else if (Dice == 4u)
		{
			_Desc._Effect = AddGameObject<ElectricVortex>();
			_Desc._Tag = GAMEOBJECTTAG::Eff_ElectricVortex;
			if (auto _TargetEffect = std::dynamic_pointer_cast<ElectricVortex>(_Desc._Effect.lock());
				_TargetEffect)
			{
				_Desc.PlayTime = _TargetEffect->GetPlayTime();
				_TargetEffect->SetEditable(false);
				auto TargetTransform = _TargetEffect->GetComponent<Transform>().lock();
				TargetTransform->
					SetScale(Vector3{
						GScale * 0.01f,
						GScale * 0.01f,
						GScale * 0.01f });
				TargetTransform->SetRotation(FMath::RandomEuler(1.f));
				_TargetEffect->PtLightFlux = -1.f;
			}
		}

		_DescElement = _Desc;
	}


	return S_OK;
};

HRESULT CbsMidTrail::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 1.f,1.f,1.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT CbsMidTrail::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT CbsMidTrail::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	BufferUpdate(_fDeltaTime);
	ParticleUpdate(_fDeltaTime);
	EffectParticleUpdate(_fDeltaTime);

	return 0;
}

UINT CbsMidTrail::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void CbsMidTrail::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			static int32 _Mode;
			ImGui::InputInt("Mode", &_Mode);
			if (ImGui::SmallButton("Play"))
			{
				PlayStart(static_cast<CbsMidTrail::Mode>(_Mode));
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}


			for (auto& _Vtx : _TrailVtxWorldLocations)
			{
				for (auto& _CurVtx : _Vtx)
				{
					ImGui::Text("Location x %9.6f y %9.6f z %9.6f",
						_CurVtx.Location.x, _CurVtx.Location.y, _CurVtx.Location.z);
					ImGui::Text(" UV0 %9.6f ,%9.6f", _CurVtx.UV0.x, _CurVtx.UV0.y);
					ImGui::Text(" UV1 %9.6f ,%9.6f", _CurVtx.UV1.x, _CurVtx.UV1.y);
				}
			}


			ImGui::SliderFloat3("LowOffset", Offset[CurMode].first, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat3("HighOffset", Offset[CurMode].second, -300.f, 300.f, "%9.6f");
			ImGui::SliderFloat("UpdateCycle", &_Desc.UpdateCycle, FLT_MIN, 10.f, "%9.6f");
			ImGui::SliderInt("DrawTriCnt", &_Desc.DrawTriCnt, 0, _Desc.TriCnt);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In DistortionIntencity", &DistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat("NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In NonDistortionIntencity", &NonDistortionIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::SliderFloat3("Noise Scale", Scale, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat3("Noise ScrollSpeed", ScrollSpeed, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion0", NoiseDistortion0, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion0", NoiseDistortion0, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion1", NoiseDistortion1, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion1", NoiseDistortion1, FLT_MIN, 100.f, "%9.6f");

			ImGui::SliderFloat2("NoiseDistortion2", NoiseDistortion2, FLT_MIN, 100.f, "%9.6f");
			ImGui::SliderFloat2("In NoiseDistortion2", NoiseDistortion2, FLT_MIN, 100.f, "%9.6f");


			ImGui::InputFloat("ColoIntencity", &ColorIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("EmissiveIntencity", &EmissiveIntencity, FLT_MIN, 1.f, "%9.6f");

			ImGui::ColorEdit4("Color", _Color);
			ImGui::SliderFloat("UV0Multiply", &UV0Multiply, 0.f, 10.f, "%1.6f");
			ImGui::SliderFloat("CurveT", &CurveT, 0.f, 1.f);
		}
		ImGui::EndChild();
	}
};

void CbsMidTrail::OnEnable()
{
	GameObject::OnEnable();
}

void CbsMidTrail::OnDisable()
{
	GameObject::OnDisable();
};



