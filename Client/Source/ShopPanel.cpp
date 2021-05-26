#include "stdafx.h"
#include "..\Header\ShopPanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Font.h"
#include "BtlPanel.h"
#include "Nero.h"


// static var
ShopPanel::UpgradeDesc ShopPanel::_UpgradeDesc =
{ 
	765u,		// _ExgaugeUpCost
	3900u,		// _BatteryCost
	2830u,		// _TransformCost
	100000u,	// _RebellionCost

	1u,			// _ExgaugeUpUpgradeCount
	1u,			// _BatteryUpgradeCount
	1u,			// _TransformUpgradeCount
	0u			// _RebellionUpgradeCount
};

void ShopPanel::Free()
{
	SafeDeleteArray(_UIDescs);

	for (auto& Element : _FontVec)
		Destroy(Element);
	_FontVec.clear();
	_FontVec.shrink_to_fit();

	GameObject::Free();
}

std::string ShopPanel::GetName()
{
	return "ShopPanel";
}

ShopPanel* ShopPanel::Create()
{
	return new ShopPanel{};
}


void ShopPanel::RenderUI(const DrawInfo& _ImplInfo)
{
	UI_DESC_ID CurID = DESC_END;
	Matrix ScreenMat;
	Vector3 ExtraColor = Vector3(0.f, 0.f, 0.f);

	// 노출도 고정
	Renderer::GetInstance()->FixedExposure = 1.f;

	auto PlaneSubset = _PlaneMesh->GetSubset(0u);

	_ImplInfo.Fx->SetMatrix("Perspective", &_PerspectiveProjMatrix);
	_ImplInfo.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
	_ImplInfo.Fx->SetFloatArray("LightDirection", _LightDir, 3u);

	//
	CurID = STATUE_BG;
	if (!_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
	{
		if (auto SharedSubset = PlaneSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _NullBlackTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 0.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

			Create_ScreenMat(CurID, ScreenMat, 1);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		auto Pipe0Subset = _Pipe0Mesh->GetSubset(0u);
		if (auto SharedSubset = Pipe0Subset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _StatueATOSTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_AccumulationTexU", _TotalAccumulateTime * 0.01f);
			_ImplInfo.Fx->SetFloat("_BrightScale", 0.001f);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(2);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		if (auto SharedSubset = PlaneSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _GlintAlp0Tex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 0.3f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", (sinf(_TotalAccumulateTime * 2.f)) * 0.002f + 0.997f);

			Create_ScreenMat(CurID, ScreenMat, 2);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(3);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		// divinity statue
		_ImplInfo.Fx->SetFloatArray("_ExtraColor", Vector3(0.f, 0.f, 0.f), 3u);
		_ImplInfo.Fx->SetFloat("_BrightScale", 0.015f);
		_ImplInfo.Fx->SetFloatArray("LightDirection", Vector3(0.f, -1.f, -50.f), 3u);

		Create_ScreenMat(CurID, ScreenMat, 3);	// Perspective
		_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

		_ImplInfo.Fx->BeginPass(4);

		auto StatueSubset = _DivinityStatueMesh->GetSubset(0u);
		if (auto SharedSubset = StatueSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _DivinityStatueALBM0Tex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _DivinityStatueNRMR0Tex->GetTexture());

			SharedSubset->Render(_ImplInfo.Fx);
		}
		StatueSubset = _DivinityStatueMesh->GetSubset(1u);
		if (auto SharedSubset = StatueSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _DivinityStatueALBM1Tex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _DivinityStatueNRMR1Tex->GetTexture());

			SharedSubset->Render(_ImplInfo.Fx);
		}

		_ImplInfo.Fx->EndPass();
	}

	// 재지정
	_ImplInfo.Fx->SetFloatArray("LightDirection", _LightDir, 3u);

	if (auto SharedSubset = PlaneSubset.lock();
		SharedSubset)
	{
		//
		CurID = SELECT_GUIDE;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _SelectGuideTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

			Create_ScreenMat(CurID, ScreenMat, (int)_CategoryPreCmd);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = SELECT_CATEGORY;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

			// Font
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _SelectCategoryFontTex->GetTexture());

			Create_ScreenMat(CurID, ScreenMat, 0);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			// Cursor
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _SelectCategoryCursorTex->GetTexture());

			Create_ScreenMat(CurID, ScreenMat, (int)_CategoryPreCmd);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = SELECT_WEAPON;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _SelectWeaponTex0->GetTexture());
			_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponSliceAmount);

			// RedQueen
			Create_ScreenMat(CurID, ScreenMat, 0);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
			if (WP_REDQUEEN == _PreCmd)
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			else
				_ImplInfo.Fx->SetFloat("_BrightScale", 0.4f);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			// Overture
			Create_ScreenMat(CurID, ScreenMat, 1);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
			if (WP_OVERTURE == _PreCmd)
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			else
				_ImplInfo.Fx->SetFloat("_BrightScale", 0.4f);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _SelectWeaponTex1->GetTexture());
			
			// Cbs
			Create_ScreenMat(CurID, ScreenMat, 2);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
			if (WP_CBS == _PreCmd)
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			else
				_ImplInfo.Fx->SetFloat("_BrightScale", 0.4f);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			// Rebellion
			Create_ScreenMat(CurID, ScreenMat, 3);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
			if (WP_REBELLION == _PreCmd)
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			else
				_ImplInfo.Fx->SetFloat("_BrightScale", 0.4f);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			if (WP_REDQUEEN <= _PreCmd && WP_REBELLION >= _PreCmd)
			{
				uint32 CurRedOrbCount = BtlPanel::GetRedOrbCount();
				bool Buyable = false;
				int ScreenMatOpt = 0;
				int ProgressMaxCnt = 0;
				int ProgressCurCnt = 0;
				switch (_PreCmd)
				{
				case ShopPanel::WP_REDQUEEN:
					if (_UpgradeDesc._ExgaugeUpCost <= CurRedOrbCount)
						Buyable = true;
					ScreenMatOpt = 20;
					ProgressMaxCnt = 3;
					ProgressCurCnt = _UpgradeDesc._ExgaugeUpUpgradeCount;
					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _WeaponBgTex0->GetTexture());
					break;
				case ShopPanel::WP_OVERTURE:
					if (_UpgradeDesc._BatteryCost <= CurRedOrbCount)
						Buyable = true;
					ScreenMatOpt = 21;
					ProgressMaxCnt = 2;
					ProgressCurCnt = _UpgradeDesc._BatteryUpgradeCount;
					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _WeaponBgTex1->GetTexture());
					break;
				case ShopPanel::WP_CBS:
					if (_UpgradeDesc._TransformCost <= CurRedOrbCount)
						Buyable = true;
					ScreenMatOpt = 22;
					ProgressMaxCnt = 3;
					ProgressCurCnt = _UpgradeDesc._TransformUpgradeCount;
					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _WeaponBgTex2->GetTexture());
					break;
				case ShopPanel::WP_REBELLION:
					if (_UpgradeDesc._RebellionCost <= CurRedOrbCount)
						Buyable = true;
					ScreenMatOpt = 23;
					ProgressMaxCnt = 1;
					ProgressCurCnt = _UpgradeDesc._RebellionUpgradeCount;
					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _WeaponBgTex3->GetTexture());
					break;
				}

				Create_ScreenMat(CurID, ScreenMat, 27);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				//
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _SelectWeaponCursorTex->GetTexture());

				Create_ScreenMat(CurID, ScreenMat, (int)_PreCmd + 1);	// 4 ~ 7
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f * _CategoryWeaponBrightOffset);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				//
				Create_ScreenMat(CurID, ScreenMat, (int)_PreCmd + 5);	// 8 ~ 11
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f * _CategoryWeaponBrightOffset);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponSliceAmount + 0.5f);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				//
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _SelectWeaponInfoBaseTex->GetTexture());

				Create_ScreenMat(CurID, ScreenMat, 12);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.3f);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount + 0.2f);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				//
				Create_ScreenMat(CurID, ScreenMat, 13);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->SetFloat("_BrightScale", 0.7f);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount + 0.2f);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				// Button
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonBaseTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount);

				if (Buyable)
				{
					_ImplInfo.Fx->SetFloat("_BrightScale", _ButtonBright * _ButtonBlinkBrightOffset);

					Create_ScreenMat(CurID, ScreenMat, 16);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					Create_ScreenMat(CurID, ScreenMat, 17);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();
				}
				else
				{
					_ImplInfo.Fx->SetFloat("_BrightScale", 0.9f * _ButtonBlinkBrightOffset);

					Create_ScreenMat(CurID, ScreenMat, 18);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();
				}

				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonFontTex->GetTexture());
				if (Buyable)
					_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
				else
					_ImplInfo.Fx->SetFloat("_BrightScale", 0.6f);
				//_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount);
				
				Create_ScreenMat(CurID, ScreenMat, 19);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				//
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ItemAndInfoBaseTex->GetTexture());

				Create_ScreenMat(CurID, ScreenMat, 14);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount + 0.2f);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				//
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _RedOrbALBMTex->GetTexture());
				_ImplInfo.Fx->SetTexture("ATOS0Map", _RedOrbATOSTex->GetTexture());
				_ImplInfo.Fx->SetTexture("NRMR0Map", _RedOrbNRMRTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount);

				Create_ScreenMat(CurID, ScreenMat, 15);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

				_ImplInfo.Fx->BeginPass(0);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				// info
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _WeaponUpgradeInfoFontTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount);

				Create_ScreenMat(CurID, ScreenMat, ScreenMatOpt);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
				
				// upgrade progress
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _WeaponUpgradeProgressTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_BrightScale", _UpgradeProgressBright);
				_ImplInfo.Fx->SetFloat("_SliceAmount", _CategoryWeaponInfoSliceAmount);

				for (int i = 0; i < ProgressMaxCnt; ++i)
				{
					Create_ScreenMat(CurID, ScreenMat, 24 + i);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

					if (ProgressCurCnt > i)
					{
						_MinTexUV = Vector2(0.f, 0.688f);
						_MaxTexUV = Vector2(0.129f, 0.875f);
					}
					else
					{
						_MinTexUV = Vector2(0.130f, 0.688f);
						_MaxTexUV = Vector2(0.259f, 0.875f);
					}

					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();
				}
			}
		}

		//
		CurID = REDORB;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _RedOrbALBMTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ATOS0Map", _RedOrbATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _RedOrbNRMRTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(0);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = POPUP;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[POPUP].Using)
		{
			//
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _NullBlackTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.3f);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			//
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _PopupBaseTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

			Create_ScreenMat(CurID, ScreenMat, 1);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			//
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _PopupLineTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

			Create_ScreenMat(CurID, ScreenMat, 6);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			//
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _PopupTextTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);
			switch (_PrePopupDepth)
			{
			case ShopPanel::POPUP_DEPTH_NOREDORB:
				Create_ScreenMat(CurID, ScreenMat, 5);
				break;
			case ShopPanel::POPUP_DEPTH_ISUPGRADE:
				Create_ScreenMat(CurID, ScreenMat, 7);
				break;
			}
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
			_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();

			switch (_PrePopupDepth)
			{
			case ShopPanel::POPUP_DEPTH_NOREDORB:
				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonBaseTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_BrightScale", _ButtonBright);
				_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

				Create_ScreenMat(CurID, ScreenMat, 2);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				Create_ScreenMat(CurID, ScreenMat, 3);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();

				_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonFontTex->GetTexture());
				_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
				_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

				Create_ScreenMat(CurID, ScreenMat, 4);
				_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
				_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
				_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
				_ImplInfo.Fx->BeginPass(1);
				SharedSubset->Render(_ImplInfo.Fx);
				_ImplInfo.Fx->EndPass();
				break;

			case ShopPanel::POPUP_DEPTH_ISUPGRADE:
				if (POPUP_CMD_OK == _PrePopupCmd)
				{
					_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonBaseTex->GetTexture());
					_ImplInfo.Fx->SetFloat("_BrightScale", _ButtonBright);

					Create_ScreenMat(CurID, ScreenMat, 8);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					Create_ScreenMat(CurID, ScreenMat, 9);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					_ImplInfo.Fx->SetFloat("_BrightScale", 0.9f);

					Create_ScreenMat(CurID, ScreenMat, 11);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonFontTex->GetTexture());
					_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);

					Create_ScreenMat(CurID, ScreenMat, 10);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					_ImplInfo.Fx->SetFloat("_BrightScale", 0.6f);

					Create_ScreenMat(CurID, ScreenMat, 12);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();
				}
				else
				{
					_ImplInfo.Fx->SetFloat("_SliceAmount", 0.f);

					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonBaseTex->GetTexture());
					_ImplInfo.Fx->SetFloat("_BrightScale", 0.9f);

					Create_ScreenMat(CurID, ScreenMat, 13);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					_ImplInfo.Fx->SetFloat("_BrightScale", _ButtonBright);

					Create_ScreenMat(CurID, ScreenMat, 14);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					Create_ScreenMat(CurID, ScreenMat, 15);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _ButtonFontTex->GetTexture());
					_ImplInfo.Fx->SetFloat("_BrightScale", 0.6f);

					Create_ScreenMat(CurID, ScreenMat, 10);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();

					_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);

					Create_ScreenMat(CurID, ScreenMat, 12);
					_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);
					_ImplInfo.Fx->SetFloatArray("_MinTexUV", _MinTexUV, 2u);
					_ImplInfo.Fx->SetFloatArray("_MaxTexUV", _MaxTexUV, 2u);
					_ImplInfo.Fx->BeginPass(1);
					SharedSubset->Render(_ImplInfo.Fx);
					_ImplInfo.Fx->EndPass();
				}
				break;
			}
		}
	}

	// 노출도 고정 해제
	Renderer::GetInstance()->FixedExposure = std::nullopt;
}

void ShopPanel::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto SpTransform = _WeakTransform.lock();
		SpTransform)
	{
		_RenderUpdateInfo.World = SpTransform->GetRenderMatrix();
	}
}

HRESULT ShopPanel::Ready()
{
	//
	SetRenderEnable(true);

	//
	m_nTag = GAMEOBJECTTAG::UI_ShopPanel;

	//
	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"ShopPanel",
		[this](const DrawInfo& _Info)
			{
				this->RenderUI(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	//
	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_Pipe0Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx");
	
	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\noiseInput_ATOS.tga");
	_NullBlackTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\NullBlack.tga");
	
	_StatueATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\statue_atos.tga");
	_GlintAlp0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Light\\tex_capcom_light_glint_0007_alpg.tga");
	_DivinityStatueMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\Jikuushinzou.fbx");
	_DivinityStatueALBM0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_A_ALBM.dds");
	_DivinityStatueNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_A_NRMR.dds");
	_DivinityStatueALBM1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_B_ALBM.dds");
	_DivinityStatueNRMR1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_B_NRMR.dds");
	
	_RedOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_albm.tga");
	_RedOrbATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_atos.tga");
	_RedOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_nrmr.tga");

	_SelectGuideTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\SELECTGUIDE.png");
	_SelectCategoryFontTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\SELECT_CATEGORY_FONT.png");
	_SelectCategoryCursorTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\ui0021_iam.tga");
	_SelectWeaponTex0 = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui_2020_02_iam.tga");
	_SelectWeaponTex1 = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui_2020_03_iam.tga");
	_SelectWeaponCursorTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui2013_iam.tga");
	_SelectWeaponInfoBaseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui2016_00_iam.tga");
	_ItemAndInfoBaseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui2010_iam.tga");
	_ButtonBaseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui2011_00_iam.tga");
	_ButtonFontTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\buttontext.png");
	_WeaponUpgradeInfoFontTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\WeaponUpgradeInfo.png");
	_WeaponUpgradeProgressTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\ui4000_iam.tga");

	_WeaponBgTex0 = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\wp_00_iam.tga");
	_WeaponBgTex1 = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\wp_02_iam.tga");
	_WeaponBgTex2 = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\wp_15_iam.tga");
	_WeaponBgTex3 = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\wp_10_iam.tga");

	_PopupBaseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui3006_iam.tga");
	_PopupLineTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\ui0002_00_iam.tga");
	_PopupTextTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\popuptext.png");

	//
	D3DXMatrixPerspectiveFovLH(&_PerspectiveProjMatrix, D3DXToRadian(0.5f), (float)g_nWndCX / g_nWndCY, 0.001f, 1.f);

	//
	Init_UIDescs();

	//
	Init_Font();

	return S_OK;
}

HRESULT ShopPanel::Awake()
{
	return S_OK;
}

HRESULT ShopPanel::Start()
{
	return S_OK;
}

void ShopPanel::Init_Font()
{
	_FontVec.reserve(FONT_END);
	for (uint32 i = 0u; i < FONT_END; ++i)
		_FontVec.push_back(static_pointer_cast<Font>(AddGameObject<Font>().lock()));
	
	//
	_FontVec[FT_REDORBCOUNT].lock()->SetRenderFlag(true);
	
	//
	_FontVec[FT_CUSTOMIZE].lock()->SetText(
		"C U S T O  M I  Z E",
		Font::TEX_ID::DMC5_WHITE,
		{ _UIDescs[CUSTOMIZE].Pos.x, _UIDescs[CUSTOMIZE].Pos.y },
		{ 0.5f, 0.5f },
		Vector3(0.537f, 0.494f, 0.408f),
		false);
	_FontVec[FT_CUSTOMIZE].lock()->SetRenderFlag(true);

	//
	_FontVec[FT_WP_REDQUEEN].lock()->SetRenderFlag(true);
	_FontVec[FT_WP_OVERTURE].lock()->SetRenderFlag(true);
	_FontVec[FT_WP_CBS].lock()->SetRenderFlag(true);
	_FontVec[FT_WP_REBELLION].lock()->SetRenderFlag(true);

	//
	_FontVec[FT_WP_RQ_EXGAUGEUP].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_OT_BATTERY].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_CBS_TRANSFORM].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_RB_REBELLION].lock()->SetRenderFlag(false);

	_FontVec[FT_WP_RQ_EXGAUGEUP_COST].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_OT_BATTERY_COST].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_CBS_TRANSFORM_COST].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_RB_REBELLION_COST].lock()->SetRenderFlag(false);

	//
	_FontVec[FT_ATTENTION].lock()->SetText(
		"A T T E N T I O N",
		Font::TEX_ID::DMC5_WHITE,
		{ _UIDescs[POPUP].Pos.x + 560.f, _UIDescs[POPUP].Pos.y + 230.f },
		{ 0.5f, 0.5f },
		Vector3(0.537f, 0.494f, 0.408f),
		false);
	_FontVec[FT_ATTENTION].lock()->SetRenderFlag(false);
}

void ShopPanel::ResetOffset()
{
	if (CATEGORY_WEAPON == _CategoryPreCmd)
	{
		_CategoryCursorXPos = 194.f;
	}
	else if (CATEGORY_ITEM == _CategoryPreCmd)
	{
		_CategoryCursorXPos = 394.f;
	}
		
	_CategoryCursorXScaleOffset = 1.f;

	_CategoryWeaponBrightOffset = 1.f;

	_FontVec[FT_REDORBCOUNT].lock()->SetRenderFlag(true);
	_FontVec[FT_CUSTOMIZE].lock()->SetRenderFlag(true);

	_FontVec[FT_WP_REDQUEEN].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_OVERTURE].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_CBS].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_REBELLION].lock()->SetRenderFlag(false);

	_FontVec[FT_WP_RQ_EXGAUGEUP].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_OT_BATTERY].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_CBS_TRANSFORM].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_RB_REBELLION].lock()->SetRenderFlag(false);

	_FontVec[FT_WP_RQ_EXGAUGEUP_COST].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_OT_BATTERY_COST].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_CBS_TRANSFORM_COST].lock()->SetRenderFlag(false);
	_FontVec[FT_WP_RB_REBELLION_COST].lock()->SetRenderFlag(false);

	_BrightAccTime = 0.f;
	_ButtonBright = 1.f;
	_UpgradeProgressBright = 1.f;

	_ButtonBlinkStart = false;
	_ButtonBlinkBrightOffset = 1.f;
	_ButtonBlinkAccTime = 0.f;
	_ButtonBlinkCnt = 0;

	_FontVec[FT_ATTENTION].lock()->SetRenderFlag(false);
}

void ShopPanel::ResetCmd()
{
	_CurDepth = CATEGORY;
	_PreDepth = NONE;
	_CurCmd = CATEGORY_WEAPON;
	_PreCmd = CATEGORY_WEAPON;
	_SavedCmd = CMD_NONE;	// 팝업창때 폰트 날려야 해서 시발...
	_CategoryPreCmd = CATEGORY_WEAPON;
	_CurPopupDepth = POPUP_DEPTH_NONE;
	_PrePopupDepth = POPUP_DEPTH_NONE;
	_CurPopupCmd = POPUP_CMD_NONE;
	_PrePopupCmd = POPUP_CMD_NONE;

	ResetOffset();

	//
	_FontVec[FT_REDORBCOUNT].lock()->SetRenderFlag(false);
	_FontVec[FT_CUSTOMIZE].lock()->SetRenderFlag(false);
}

UINT ShopPanel::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	_TotalAccumulateTime += _fDeltaTime;

	//
	Check_KeyInput(_fDeltaTime);

	// 상태가 바뀌면 갱신되야 할 것들
	if ((_PreDepth != _CurDepth) || (_PreCmd != _CurCmd))
	{
		ResetOffset();

		switch (_CurDepth)
		{
		case CATEGORY:
			switch (_CurCmd)
			{
			case CATEGORY_WEAPON:
				_CategoryCursorXPos = 194.f;
				_FontVec[FT_WP_REDQUEEN].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_OVERTURE].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_CBS].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_REBELLION].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				break;
			case CATEGORY_ITEM:
				_CategoryCursorXPos = 394.f;
				_FontVec[FT_WP_REDQUEEN].lock()->SetRenderFlag(false, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_OVERTURE].lock()->SetRenderFlag(false, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_CBS].lock()->SetRenderFlag(false, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_REBELLION].lock()->SetRenderFlag(false, Font::FADE_ID::ALPHA_LINEAR);
				break;
			}
			_CategoryCursorXScaleOffset = 1.35f;
			_CategoryPreCmd = _CurCmd;
			break;
			
		case WEAPON_SELECT:
			_CategoryWeaponBrightOffset = 2.5f;
			_FontVec[FT_WP_REDQUEEN].lock()->SetRenderFlag(true);
			_FontVec[FT_WP_OVERTURE].lock()->SetRenderFlag(true);
			_FontVec[FT_WP_CBS].lock()->SetRenderFlag(true);
			_FontVec[FT_WP_REBELLION].lock()->SetRenderFlag(true);
			_CategoryWeaponInfoXPos = 210.f;
			_CategoryWeaponInfoSliceAmount = 1.f;

			switch (_CurCmd)
			{
			case ShopPanel::WP_REDQUEEN:
				_FontVec[FT_WP_RQ_EXGAUGEUP].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_RQ_EXGAUGEUP_COST].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				break;
			case ShopPanel::WP_OVERTURE:
				_FontVec[FT_WP_OT_BATTERY].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_OT_BATTERY_COST].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				break;
			case ShopPanel::WP_CBS:
				_FontVec[FT_WP_CBS_TRANSFORM].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_CBS_TRANSFORM_COST].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				break;
			case ShopPanel::WP_REBELLION:
				_FontVec[FT_WP_RB_REBELLION].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				_FontVec[FT_WP_RB_REBELLION_COST].lock()->SetRenderFlag(true, Font::FADE_ID::ALPHA_LINEAR);
				break;
			}
			break;

		default:
			break;
		}

		_PreDepth = _CurDepth;
		_PreCmd = _CurCmd;
	}

	// 계속 업데이트 되어야 할 것들
	switch (_PreDepth)
	{
	case CATEGORY:
		switch (_PreCmd)
		{
		case CATEGORY_WEAPON:
			if (1.f < _CategoryCursorXScaleOffset)
			{
				_CategoryCursorXScaleOffset -= 4.f * _fDeltaTime;
				if (1.f > _CategoryCursorXScaleOffset)
					_CategoryCursorXScaleOffset = 1.f;
			}
			if (0.f < _CategoryWeaponSliceAmount)
			{
				_CategoryWeaponSliceAmount -= 6.f * _fDeltaTime;
				if (0.f > _CategoryWeaponSliceAmount)
					_CategoryWeaponSliceAmount = 0.f;
			}
			if (0.f > _UIDescs[SELECT_WEAPON].Pos.x)
			{
				_UIDescs[SELECT_WEAPON].Pos.x += 800.f * _fDeltaTime;
				if (0.f < _UIDescs[SELECT_WEAPON].Pos.x)
					_UIDescs[SELECT_WEAPON].Pos.x = 0.f;
			}
			break;
		case CATEGORY_ITEM:
			if (1.f < _CategoryCursorXScaleOffset)
			{
				_CategoryCursorXScaleOffset -= 4.f * _fDeltaTime;
				if (1.f > _CategoryCursorXScaleOffset)
					_CategoryCursorXScaleOffset = 1.f;
			}
			if (1.f > _CategoryWeaponSliceAmount)
			{
				_CategoryWeaponSliceAmount += 6.f * _fDeltaTime;
				if (1.f < _CategoryWeaponSliceAmount)
					_CategoryWeaponSliceAmount = 1.f;
			}
			if (-180.f < _UIDescs[SELECT_WEAPON].Pos.x)
			{
				_UIDescs[SELECT_WEAPON].Pos.x -= 800.f * _fDeltaTime;
				if (-180.f > _UIDescs[SELECT_WEAPON].Pos.x)
					_UIDescs[SELECT_WEAPON].Pos.x = -180.f;
			}
			break;
		}
		break;

	case WEAPON_SELECT:
		if (1.f < _CategoryWeaponBrightOffset)
		{
			_CategoryWeaponBrightOffset -= 7.5f * _fDeltaTime;
			if (1.f > _CategoryWeaponBrightOffset)
				_CategoryWeaponBrightOffset = 1.f;
		}
		if (410.f > _CategoryWeaponInfoXPos)
		{
			_CategoryWeaponInfoXPos += 1100.f * _fDeltaTime;
			if (410.f < _CategoryWeaponInfoXPos)
				_CategoryWeaponInfoXPos = 410.f;
		}
		if (0.f < _CategoryWeaponInfoSliceAmount)
		{
			_CategoryWeaponInfoSliceAmount -= 2.f * _fDeltaTime;
			if (0.f > _CategoryWeaponInfoSliceAmount)
				_CategoryWeaponInfoSliceAmount = 0.f;
		}

		_BrightAccTime += _fDeltaTime;
		_ButtonBright = 1.f + 0.15f * sinf(_BrightAccTime * 2.5f);
		_UpgradeProgressBright = 1.f + 0.5f * sinf(_BrightAccTime * 2.5f);

		if (_ButtonBlinkStart)
		{
			_ButtonBlinkAccTime += _fDeltaTime;

			if (4u <= _ButtonBlinkCnt)
			{
				// + 새 팝업창
				_UIDescs[POPUP].Using = true;
				_CurPopupDepth = POPUP_DEPTH_NOREDORB;	// 임시

				if (WP_REDQUEEN <= _PreCmd && WP_REBELLION >= _PreCmd)
				{
					uint32 CurRedOrbCount = BtlPanel::GetRedOrbCount();
					switch (_PreCmd)
					{
					case ShopPanel::WP_REDQUEEN:
						if (_UpgradeDesc._ExgaugeUpCost <= CurRedOrbCount)
							_CurPopupDepth = POPUP_DEPTH_ISUPGRADE;
						break;
					case ShopPanel::WP_OVERTURE:
						if (_UpgradeDesc._BatteryCost <= CurRedOrbCount)
							_CurPopupDepth = POPUP_DEPTH_ISUPGRADE;
						break;
					case ShopPanel::WP_CBS:
						if (_UpgradeDesc._TransformCost <= CurRedOrbCount)
							_CurPopupDepth = POPUP_DEPTH_ISUPGRADE;
						break;
					case ShopPanel::WP_REBELLION:
						if (_UpgradeDesc._RebellionCost <= CurRedOrbCount)
							_CurPopupDepth = POPUP_DEPTH_ISUPGRADE;
						break;
					}
				}

				_CurPopupCmd = POPUP_CMD_OK;

				_ButtonBlinkStart = false;
				_ButtonBlinkBrightOffset = 1.f;
				_ButtonBlinkAccTime = 0.f;
				_ButtonBlinkCnt = 0;
			}
			else if (0.08f < _ButtonBlinkAccTime && 4u > _ButtonBlinkCnt)
			{
				if (1 == _ButtonBlinkCnt % 2)
					_ButtonBlinkBrightOffset = 1.f;
				else
					_ButtonBlinkBrightOffset = 0.4f;

				++_ButtonBlinkCnt;
				_ButtonBlinkAccTime = 0.f;
			}
		}
		break;

	default:
		break;
	}

	//
	if ((_PrePopupDepth != _CurPopupDepth) || (_PrePopupCmd != _CurPopupCmd))
	{
		switch (_CurPopupDepth)
		{
		case POPUP_DEPTH_NONE:
			_FontVec[FT_ATTENTION].lock()->SetRenderFlag(false);
			switch (_PreCmd)
			{
			case ShopPanel::WP_REDQUEEN:
				_FontVec[FT_WP_RQ_EXGAUGEUP].lock()->SetRenderFlag(true);
				_FontVec[FT_WP_RQ_EXGAUGEUP_COST].lock()->SetRenderFlag(true);
				break;
			case ShopPanel::WP_OVERTURE:
				_FontVec[FT_WP_OT_BATTERY].lock()->SetRenderFlag(true);
				_FontVec[FT_WP_OT_BATTERY_COST].lock()->SetRenderFlag(true);
				break;
			case ShopPanel::WP_CBS:
				_FontVec[FT_WP_CBS_TRANSFORM].lock()->SetRenderFlag(true);
				_FontVec[FT_WP_CBS_TRANSFORM_COST].lock()->SetRenderFlag(true);
				break;
			case ShopPanel::WP_REBELLION:
				_FontVec[FT_WP_RB_REBELLION].lock()->SetRenderFlag(true);
				_FontVec[FT_WP_RB_REBELLION_COST].lock()->SetRenderFlag(true);
				break;
			}
			break;

		case POPUP_DEPTH_NOREDORB:
		case POPUP_DEPTH_ISUPGRADE:
			_FontVec[FT_ATTENTION].lock()->SetRenderFlag(true);
			switch (_PreCmd)
			{
			case ShopPanel::WP_REDQUEEN:
				_FontVec[FT_WP_RQ_EXGAUGEUP].lock()->SetRenderFlag(false);
				_FontVec[FT_WP_RQ_EXGAUGEUP_COST].lock()->SetRenderFlag(false);
				break;
			case ShopPanel::WP_OVERTURE:
				_FontVec[FT_WP_OT_BATTERY].lock()->SetRenderFlag(false);
				_FontVec[FT_WP_OT_BATTERY_COST].lock()->SetRenderFlag(false);
				break;
			case ShopPanel::WP_CBS:
				_FontVec[FT_WP_CBS_TRANSFORM].lock()->SetRenderFlag(false);
				_FontVec[FT_WP_CBS_TRANSFORM_COST].lock()->SetRenderFlag(false);
				break;
			case ShopPanel::WP_REBELLION:
				_FontVec[FT_WP_RB_REBELLION].lock()->SetRenderFlag(false);
				_FontVec[FT_WP_RB_REBELLION_COST].lock()->SetRenderFlag(false);
				break;
			}
			break;
		}

		_PrePopupDepth = _CurPopupDepth;
		_PrePopupCmd = _CurPopupCmd;
	}

	//
	Update_Font(_fDeltaTime);

	//
	//POINT pt{};
	//GetCursorPos(&pt);
	//ScreenToClient(g_hWnd, &pt);
	//Vector2 TargetPos = Vector2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	//std::cout << ScreenPosToOrtho(TargetPos.x, TargetPos.y).x << std::endl;

	return 0;
}

UINT ShopPanel::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void ShopPanel::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		Imgui_ModifyUI();
	}
}

void ShopPanel::OnEnable()
{
	GameObject::OnEnable();
}

void ShopPanel::OnDisable()
{
	GameObject::OnDisable();
}

void ShopPanel::Init_UIDescs()
{
	if (!_UIDescs)
		_UIDescs = new UI_DESC[DESC_END];

	// Using, Pos, Scale, Rot
	_UIDescs[STATUE_BG] = { true, Vector3(640.f, 360.f, 0.9f), Vector3(8.f, 8.f, 1.f) };
	_UIDescs[REDORB] = { true, Vector3(1070.f, 50.f, 0.5f), Vector3(0.55f, 0.55f, 1.f) };
	_UIDescs[CUSTOMIZE] = { true, Vector3(50.f, 40.f, 0.5f), Vector3(0.6f, 0.6f, 1.f) };
	_UIDescs[SELECT_GUIDE] = { true, Vector3(285.f, 655.f, 0.5f), Vector3(5.12f, 0.32f, 1.f) };
	_UIDescs[SELECT_WEAPON] = { true, Vector3(0.f, 0.f, 0.5f), Vector3(1.f, 1.f, 1.f) };
	_UIDescs[SELECT_CATEGORY] = { true, Vector3(552.f, 100.f, 0.5f), Vector3(10.24f, 0.32f, 1.f) };
	_UIDescs[POPUP] = { false, Vector3(0.f, 0.f, 0.3f), Vector3(1.f, 1.f, 1.f) };
}

void ShopPanel::Create_ScreenMat(UI_DESC_ID _ID, Matrix& _Out, int _Opt/*= 0*/)
{
	Matrix RotMat;

	D3DXMatrixIdentity(&_Out);

	switch (_ID)
	{
	case STATUE_BG:
		switch (_Opt)
		{
		case 1:
			_Out._11 = 12.8f;
			_Out._22 = 7.2f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 0.f;
			_Out._42 = 0.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		case 2:
			_Out._11 = 16.f;
			_Out._22 = 16.f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 0.f;
			_Out._42 = 0.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		case 3:	// Perspective
			_Out._11 = 0.002f;
			_Out._22 = 0.002f;
			_Out._33 = FLT_MIN; // ㅠㅠ
			_Out._41 = 0.003f;
			_Out._42 = -0.18f;
			_Out._43 = 9.f;

			//_Out._11 = _DebugScale.x;
			//_Out._22 = _DebugScale.y;
			//_Out._33 = _DebugScale.z;
			//D3DXMatrixRotationX(&RotMat, D3DXToRadian(_DebugRot.x));
			//_Out *= RotMat;
			//D3DXMatrixRotationY(&RotMat, D3DXToRadian(_DebugRot.y));
			//_Out *= RotMat;
			//D3DXMatrixRotationZ(&RotMat, D3DXToRadian(_DebugRot.z));
			//_Out *= RotMat;
			//_Out._41 = _DebugPos.x;
			//_Out._42 = _DebugPos.y;
			//_Out._43 = _DebugPos.z;

			break;

		case 0: default:
			goto DEFAULT;
			break;
		}
		break;

	case REDORB:
		if (1 == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x * 1.f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.5f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 55.f) - (g_nWndCX >> 1);
			_Out._42 = -((_UIDescs[_ID].Pos.y) - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
		}
		else
		{
			goto DEFAULT;
		}
		break;

	case SELECT_GUIDE:
		if ((int)CATEGORY_WEAPON == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x) - (g_nWndCX >> 1);
			_Out._42 = -((_UIDescs[_ID].Pos.y) - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 0.5f);
		}
		else if ((int)CATEGORY_ITEM == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x;
			_Out._22 = _UIDescs[_ID].Scale.y;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x) - (g_nWndCX >> 1);
			_Out._42 = -((_UIDescs[_ID].Pos.y + 1.f) - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.5f);
			_MaxTexUV = Vector2(1.f, 1.f);
		}
		break;

	case SELECT_WEAPON:
		switch (_Opt)
		{
		// base
		case 0:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 1.f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 190.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.5f, 0.f);
			_MaxTexUV = Vector2(1.f, 0.25f);
			break;
		case 1:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 1.f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 300.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.5f);
			_MaxTexUV = Vector2(0.5f, 0.75f);
			break;
		case 2:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 1.f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 410.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(0.25f, 0.5f);
			break;
		case 3:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 1.f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 520.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.0f);
			_MaxTexUV = Vector2(0.25f, 0.25f);
			break;
		// cursor 
		case 4:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.55f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 190.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(1.f, 0.53f);
			break;
		case 5:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.55f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 300.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(1.f, 0.53f);
			break;
		case 6:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.55f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 410.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(1.f, 0.53f);
			break;
		case 7:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.2f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.55f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 140.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 520.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(1.f, 0.53f);
			break;
		// font base 
		case 8:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.022f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.485f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 142.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 239.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(0.9f, 0.25f);
			break;
		case 9:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.022f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.485f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 142.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 349.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(0.9f, 0.25f);
			break;
		case 10:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.022f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.485f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 142.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 459.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(0.9f, 0.25f);
			break;
		case 11:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.022f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.485f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 142.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 569.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(0.9f, 0.25f);
			break;
		// info base
		case 12:
			_Out._11 = _UIDescs[_ID].Scale.x * 3.f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.7f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 200.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.332f);
			_MaxTexUV = Vector2(1.f, 0.707f);
			break;
		case 13:
			_Out._11 = _UIDescs[_ID].Scale.x * 3.f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.4f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 3.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 248.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.43f, 0.879f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		case 14:
			_Out._11 = _UIDescs[_ID].Scale.x * 3.3f;
			_Out._22 = _UIDescs[_ID].Scale.y * 1.8f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 5.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 380.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.41f);
			_MaxTexUV = Vector2(1.f, 0.72f);
			break;
		// redorb
		case 15:
			_Out._11 = _UIDescs[_ID].Scale.x * 0.36f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.36f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos - 120.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 250.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		// button
		case 16:
			_Out._11 = _UIDescs[_ID].Scale.x * 1.6f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 66.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 258.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.619f);
			_MaxTexUV = Vector2(0.616f, 0.675f);
			break;
		case 17:
			_Out._11 = _UIDescs[_ID].Scale.x * 1.6f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 66.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 258.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.736f);
			_MaxTexUV = Vector2(0.616f, 0.795f);
			break;
		case 18:
			_Out._11 = _UIDescs[_ID].Scale.x * 1.6f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 66.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 258.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.677f);
			_MaxTexUV = Vector2(0.616f, 0.734f);
			break;
		case 19:
			_Out._11 = _UIDescs[_ID].Scale.x * 1.1f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.39f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 68.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 265.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 0.25f);
			break;
		// Weapon Info
		case 20:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.8f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.8f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 20.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 350.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 0.25f);
			break;
		case 21:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.8f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.8f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 20.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 350.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(1.f, 0.5f);
			break;
		case 22:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.8f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.8f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 20.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 350.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.5f);
			_MaxTexUV = Vector2(1.f, 0.75f);
			break;
		case 23:
			_Out._11 = _UIDescs[_ID].Scale.x * 2.8f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.8f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 20.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 350.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.75f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		// Upgrade Progress
		case 24:
			_Out._11 = _UIDescs[_ID].Scale.x * 0.6f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.3f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos - 85.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 420.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			//_MinTexUV = Vector2(0.f, 0.688f);
			//_MaxTexUV = Vector2(0.129f, 0.875f);
			break;
		case 25:
			_Out._11 = _UIDescs[_ID].Scale.x * 0.6f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.3f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos - 20.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 420.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			//_MinTexUV = Vector2(0.f, 0.688f);
			//_MaxTexUV = Vector2(0.129f, 0.875f);
			break;
		case 26:
			_Out._11 = _UIDescs[_ID].Scale.x * 0.6f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.3f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + 45.f) - (g_nWndCX >> 1);
			_Out._42 = -(_UIDescs[_ID].Pos.y + 420.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			//_MinTexUV = Vector2(0.f, 0.688f);
			//_MaxTexUV = Vector2(0.129f, 0.875f);
			break;
		case 27:
			_Out._11 = 9.351f;
			_Out._22 = 7.2f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 810.f - (g_nWndCX >> 1);
			_Out._42 = -(360.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.359f, 0.001f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		default:
			goto DEFAULT;
		}
		break;

	case SELECT_CATEGORY:
		if ((int)CATEGORY_WEAPON == _Opt || (int)CATEGORY_ITEM == _Opt)
		{
			_Out._11 = 2.f * _CategoryCursorXScaleOffset;
			_Out._22 = 0.6f;
			_Out._33 = 1.f;
			_Out._41 = _CategoryCursorXPos - (g_nWndCX >> 1);	// 194.f ~ 394.f
			_Out._42 = -(98.f - (g_nWndCY >> 1));
			_Out._43 = 0.4f;
			_MinTexUV = Vector2(0.f, 0.405f);
			_MaxTexUV = Vector2(0.133f, 0.505f);
		}
		else
		{
			goto DEFAULT;
		}
		break;

	case POPUP:
		switch (_Opt)
		{
		case 0:		
			_Out._11 = 12.8f;
			_Out._22 = 72.f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 0.f;
			_Out._42 = 0.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		case 1:
			_Out._11 = 7.6f;
			_Out._22 = 4.2f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 0.f;
			_Out._42 = 0.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.493f, 0.146f);
			_MaxTexUV = Vector2(0.906f, 0.566f);
			break;
		case 2:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 35.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.619f);
			_MaxTexUV = Vector2(0.616f, 0.675f);
			break;
		case 3:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 35.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.736f);
			_MaxTexUV = Vector2(0.616f, 0.795f);
			break;
		case 4:
			_Out._11 = 1.6f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 672.f - (g_nWndCX >> 1);
			_Out._42 = -(502.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.5f);
			_MaxTexUV = Vector2(1.f, 0.75f);
			break;
		case 5:
			_Out._11 = 4.4f;
			_Out._22 = 0.8f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 750.f - (g_nWndCX >> 1);
			_Out._42 = -(350.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 0.25f);
			break;
		case 6:
			_Out._11 = 7.f;
			_Out._22 = 0.2f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 660.f - (g_nWndCX >> 1);
			_Out._42 = -(255.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 0.0625f);
			break;
		case 7:
			_Out._11 = 4.4f;
			_Out._22 = 0.8f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 710.f - (g_nWndCX >> 1);
			_Out._42 = -(350.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.25f);
			_MaxTexUV = Vector2(1.f, 0.5f);
			break;
		case 8:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = -60.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.619f);
			_MaxTexUV = Vector2(0.616f, 0.675f);
			break;
		case 9:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = -60.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.736f);
			_MaxTexUV = Vector2(0.616f, 0.795f);
			break;
		case 10:
			_Out._11 = 1.6f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (-60.f + 637.f) - (g_nWndCX >> 1);
			_Out._42 = -(502.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.0f);
			_MaxTexUV = Vector2(1.f, 0.25f);
			break;
		case 11:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 130.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.677f);
			_MaxTexUV = Vector2(0.616f, 0.734f);
			break;
		case 12:
			_Out._11 = 1.6f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (130.f + 637.f) - (g_nWndCX >> 1);
			_Out._42 = -(502.f - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.5f);
			_MaxTexUV = Vector2(1.f, 0.75f);
			break;
		case 13:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = -60.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.677f);
			_MaxTexUV = Vector2(0.616f, 0.734f);
			break;
		case 14:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 130.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.619f);
			_MaxTexUV = Vector2(0.616f, 0.675f);
			break;
		case 15:
			_Out._11 = 2.5f;
			_Out._22 = 0.35f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 130.f;
			_Out._42 = -135.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.279f, 0.736f);
			_MaxTexUV = Vector2(0.616f, 0.795f);
			break;
		default:
			goto DEFAULT;

			//_Out._11 = _UIDescs[_ID].Scale.x * _DebugScale.x;
			//_Out._22 = _UIDescs[_ID].Scale.y * _DebugScale.y;
			//_Out._33 = _UIDescs[_ID].Scale.z;
			//_Out._41 = (_UIDescs[_ID].Pos.x + _CategoryWeaponInfoXPos + _DebugPos.x) - (g_nWndCX >> 1);
			//_Out._42 = -(_UIDescs[_ID].Pos.y + _DebugPos.y - (g_nWndCY >> 1));
			//_Out._43 = _UIDescs[_ID].Pos.z;
			//_MinTexUV = _DebugMinUV;
			//_MaxTexUV = _DebugMaxUV;

			break;
		}

		break;

	default: DEFAULT:
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.y;
		_Out._33 = _UIDescs[_ID].Scale.z;
		_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		_MinTexUV = Vector2(0.f, 0.f);
		_MaxTexUV = Vector2(1.f, 1.f);
		break;
	}
}

void ShopPanel::Update_Font(const float _fDeltaTime)
{
	// RedOrb
	if (auto pFont = _FontVec[FT_REDORBCOUNT].lock();
		pFont)
	{
		if (_UIDescs[REDORB].Using)
		{
			pFont->SetText(
				std::to_string(BtlPanel::GetRedOrbCount()),
				Font::TEX_ID::DMC5_BLACK_GRAD,
				{ _UIDescs[REDORB].Pos.x + 35.f, _UIDescs[REDORB].Pos.y },
				{ 0.8f, 0.8f },
				Vector3(1.f, 1.f, 1.f),
				true);

			pFont->SetRenderFlag(true);
		}
		else
		{
			pFont->SetRenderFlag(false);
		}
	}

	// Customize
	if (auto pFont = _FontVec[FT_CUSTOMIZE].lock();
		pFont)
	{
		if (_UIDescs[CUSTOMIZE].Using)
			pFont->SetRenderFlag(true);
		else
			pFont->SetRenderFlag(false);
	}

	// SELECT_WEAPON
	const Vector3 BaseColor = { 1.3f, 1.3f, 1.3f };
	const Vector3 DarkColor = { 0.3f, 0.3f, 0.3f };
	const Vector3 RedColor = { 1.1f, 0.f, 0.f };
	Vector3 CurCostColor = BaseColor;
	uint32 CurRedOrbCount = BtlPanel::GetRedOrbCount();

	if (WP_REDQUEEN == _PreCmd && !_UIDescs[POPUP].Using)
	{
		_FontVec[FT_WP_REDQUEEN].lock()->SetText(
			"RED QUEEN",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 230.f },
			{ 0.45f, 0.45f }
		);

		_FontVec[FT_WP_RQ_EXGAUGEUP].lock()->SetText(
			"EX GAUGE UP",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 127.f, _UIDescs[SELECT_WEAPON].Pos.y + 200.f },
			{ 0.7f, 0.7f },
			BaseColor
		);

		if (_UpgradeDesc._ExgaugeUpCost <= CurRedOrbCount)
			CurCostColor = BaseColor;
		else
			CurCostColor = RedColor;

		_FontVec[FT_WP_RQ_EXGAUGEUP_COST].lock()->SetText(
			std::to_string(_UpgradeDesc._ExgaugeUpCost),
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 94.f, _UIDescs[SELECT_WEAPON].Pos.y + 252.f },
			{ 0.55f, 0.55f },
			CurCostColor
		);
	}
	else
	{
		_FontVec[FT_WP_REDQUEEN].lock()->SetText(
			"RED QUEEN",
			Font::TEX_ID::DMC5_WHITE,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 230.f },
			{ 0.45f, 0.45f },
			DarkColor
		);
	}

	if (WP_OVERTURE == _PreCmd && !_UIDescs[POPUP].Using)
	{
		_FontVec[FT_WP_OVERTURE].lock()->SetText(
			"OVERTURE",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 340.f },
			{ 0.45f, 0.45f }
		);

		_FontVec[FT_WP_OT_BATTERY].lock()->SetText(
			"BATTERY",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 127.f, _UIDescs[SELECT_WEAPON].Pos.y + 200.f },
			{ 0.7f, 0.7f },
			BaseColor
		);

		if (_UpgradeDesc._BatteryCost <= CurRedOrbCount)
			CurCostColor = BaseColor;
		else
			CurCostColor = RedColor;

		_FontVec[FT_WP_OT_BATTERY_COST].lock()->SetText(
			std::to_string(_UpgradeDesc._BatteryCost),
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 94.f, _UIDescs[SELECT_WEAPON].Pos.y + 252.f },
			{ 0.55f, 0.55f },
			CurCostColor
		);
	}
	else
	{
		_FontVec[FT_WP_OVERTURE].lock()->SetText(
			"OVERTURE",
			Font::TEX_ID::DMC5_WHITE,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 340.f },
			{ 0.45f, 0.45f },
			DarkColor
		);
	}

	if (WP_CBS == _PreCmd && !_UIDescs[POPUP].Using)
	{
		_FontVec[FT_WP_CBS].lock()->SetText(
			"CERBERUS",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 450.f },
			{ 0.45f, 0.45f }
		);

		_FontVec[FT_WP_CBS_TRANSFORM].lock()->SetText(
			"TRANSFORM",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 127.f, _UIDescs[SELECT_WEAPON].Pos.y + 200.f },
			{ 0.7f, 0.7f },
			BaseColor
		);

		if (_UpgradeDesc._TransformCost <= CurRedOrbCount)
			CurCostColor = BaseColor;
		else
			CurCostColor = RedColor;

		_FontVec[FT_WP_CBS_TRANSFORM_COST].lock()->SetText(
			std::to_string(_UpgradeDesc._TransformCost),
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 94.f, _UIDescs[SELECT_WEAPON].Pos.y + 252.f },
			{ 0.55f, 0.55f },
			CurCostColor
		);
	}
	else
	{
		_FontVec[FT_WP_CBS].lock()->SetText(
			"CERBERUS",
			Font::TEX_ID::DMC5_WHITE,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 450.f },
			{ 0.45f, 0.45f },
			DarkColor
		);
	}

	if (WP_REBELLION == _PreCmd && !_UIDescs[POPUP].Using)
	{
		_FontVec[FT_WP_REBELLION].lock()->SetText(
			"REBELLION",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 560.f },
			{ 0.45f, 0.45f }
		);

		_FontVec[FT_WP_RB_REBELLION].lock()->SetText(
			"REBELLION",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 127.f, _UIDescs[SELECT_WEAPON].Pos.y + 200.f },
			{ 0.7f, 0.7f },
			BaseColor
		);

		if (_UpgradeDesc._RebellionCost <= CurRedOrbCount)
			CurCostColor = BaseColor;
		else
			CurCostColor = RedColor;

		_FontVec[FT_WP_RB_REBELLION_COST].lock()->SetText(
			std::to_string(_UpgradeDesc._RebellionCost),
			Font::TEX_ID::DMC5_GREEN_GRAD,
			{ _UIDescs[SELECT_WEAPON].Pos.x + _CategoryWeaponInfoXPos - 94.f, _UIDescs[SELECT_WEAPON].Pos.y + 252.f },
			{ 0.55f, 0.55f },
			CurCostColor
		);
	}
	else
	{
		_FontVec[FT_WP_REBELLION].lock()->SetText(
			"REBELLION",
			Font::TEX_ID::DMC5_WHITE,
			{ _UIDescs[SELECT_WEAPON].Pos.x + 52.f, _UIDescs[SELECT_WEAPON].Pos.y + 560.f },
			{ 0.45f, 0.45f },
			DarkColor
		);
	}
}

Vector2 ShopPanel::WorldPosToScreenPos(const Vector3& WorldPos)
{
	const ENGINE::RenderInformation& info = Renderer::GetInstance()->_RenderInfo;

	Vector4 Pos = Vector4(WorldPos.x, WorldPos.y, WorldPos.z, 1.f);
	D3DXVec4Transform(&Pos, &Pos, &info.View);
	D3DXVec4Transform(&Pos, &Pos, &info.Projection);

	float x = (Pos.x / Pos.w + 1.f) * (g_nWndCX >> 1);
	float y = (-Pos.y / Pos.w + 1.f) * (g_nWndCY >> 1);

	return Vector2(x, y);
}

Vector2 ShopPanel::ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY)
{
	Vector2 Ret = Vector2(_ScreenPosX - (g_nWndCX >> 1), -(_ScreenPosY - (g_nWndCY >> 1)));
	D3DXVec2TransformCoord(&Ret, &Ret, &Renderer::GetInstance()->_RenderInfo.Ortho);
	return Ret;
}

void ShopPanel::Check_KeyInput(const float _fDeltaTime)
{
	if (!_UIDescs[POPUP].Using)
	{
		switch (_PreDepth)
		{
		case CATEGORY:
			if (Input::GetKeyDown(DIK_TAB))
			{
				switch (_PreCmd)
				{
				case CATEGORY_WEAPON:
					_CurCmd = CATEGORY_ITEM;
					break;
				case CATEGORY_ITEM:
					_CurCmd = CATEGORY_WEAPON;
					break;
				}
			}
			else if (Input::GetKeyDown(DIK_DOWNARROW) || Input::GetKeyDown(DIK_RIGHTARROW))
			{
				_CurDepth = WEAPON_SELECT;
				_CurCmd = WP_REDQUEEN;
			}
			else if (Input::GetKeyDown(DIK_UPARROW) || Input::GetKeyDown(DIK_LEFTARROW))
			{
				_CurDepth = WEAPON_SELECT;
				_CurCmd = WP_REBELLION;
			}
			break;

		case WEAPON_SELECT:
			if (Input::GetKeyDown(DIK_TAB))
			{
				_CurDepth = CATEGORY;
				_CurCmd = CATEGORY_ITEM;
			}
			else if (Input::GetKeyDown(DIK_RETURN))
			{
				if (!_ButtonBlinkStart)
					_ButtonBlinkStart = true;
			}
			else if (Input::GetKeyDown(DIK_DOWNARROW) || Input::GetKeyDown(DIK_RIGHTARROW))
			{
				switch (_PreCmd)
				{
				case WP_REDQUEEN:
					_CurCmd = WP_OVERTURE;
					break;
				case WP_OVERTURE:
					_CurCmd = WP_CBS;
					break;
				case WP_CBS:
					_CurCmd = WP_REBELLION;
					break;
				case WP_REBELLION:
					_CurCmd = WP_REDQUEEN;
					break;
				}
			}
			else if (Input::GetKeyDown(DIK_UPARROW) || Input::GetKeyDown(DIK_LEFTARROW))
			{
				switch (_PreCmd)
				{
				case WP_REDQUEEN:
					_CurCmd = WP_REBELLION;
					break;
				case WP_OVERTURE:
					_CurCmd = WP_REDQUEEN;
					break;
				case WP_CBS:
					_CurCmd = WP_OVERTURE;
					break;
				case WP_REBELLION:
					_CurCmd = WP_CBS;
					break;
				}
			}
			break;
		}
	}
	else
	{
		switch (_PrePopupDepth)
		{
		case POPUP_DEPTH_ISUPGRADE:
			if (Input::GetKeyDown(DIK_RETURN))
			{
				switch (_PrePopupCmd)
				{
				case POPUP_CMD_OK:
					switch (_PreCmd)
					{
					case WP_REDQUEEN:
						++_UpgradeDesc._ExgaugeUpUpgradeCount;
						BtlPanel::ConsumeRedOrb(_UpgradeDesc._ExgaugeUpCost);
						
						if (3u == _UpgradeDesc._ExgaugeUpUpgradeCount)
							_UpgradeDesc._ExgaugeUpCost = 9999999u;
						else if (2u == _UpgradeDesc._ExgaugeUpUpgradeCount)
							_UpgradeDesc._ExgaugeUpCost = 3460u;

						break;

					case WP_OVERTURE:
						++_UpgradeDesc._BatteryUpgradeCount;
						BtlPanel::ConsumeRedOrb(_UpgradeDesc._BatteryCost);
						
						_UpgradeDesc._BatteryCost = 9999999u;

						break;

					case WP_CBS:
						++_UpgradeDesc._TransformUpgradeCount;
						BtlPanel::ConsumeRedOrb(_UpgradeDesc._TransformUpgradeCount);

						if (3u == _UpgradeDesc._TransformUpgradeCount)
							_UpgradeDesc._TransformCost = 9999999u;
						else if (2u == _UpgradeDesc._TransformUpgradeCount)
							_UpgradeDesc._TransformCost = 4120u;
						
						break;

					case WP_REBELLION:
						++_UpgradeDesc._RebellionUpgradeCount;
						BtlPanel::ConsumeRedOrb(_UpgradeDesc._RebellionCost);

						_UpgradeDesc._RebellionCost = 9999999u;
						
						break;
					}

					_CurPopupDepth = POPUP_DEPTH_NONE;
					_CurPopupCmd = POPUP_CMD_NONE;
					_UIDescs[POPUP].Using = false;
					break;

				case POPUP_CMD_CANCLE:
				default:
					_CurPopupDepth = POPUP_DEPTH_NONE;
					_CurPopupCmd = POPUP_CMD_NONE;
					_UIDescs[POPUP].Using = false;
					break;
				}
			}
			else if (Input::GetKeyDown(DIK_DOWNARROW)
				|| Input::GetKeyDown(DIK_RIGHTARROW)
				|| Input::GetKeyDown(DIK_UPARROW)
				|| Input::GetKeyDown(DIK_LEFTARROW))
			{
				switch (_PrePopupCmd)
				{
				case POPUP_CMD_OK:
					_CurPopupCmd = POPUP_CMD_CANCLE;
					break;
				case POPUP_CMD_CANCLE:
					_CurPopupCmd = POPUP_CMD_OK;
					break;
				}
			}
			break;

		case POPUP_DEPTH_NOREDORB:
		default:
			if (Input::GetKeyDown(DIK_RETURN))
			{
				_CurPopupDepth = POPUP_DEPTH_NONE;
				_CurPopupCmd = POPUP_CMD_NONE;
				_UIDescs[POPUP].Using = false;
			}
			break;
		}
	}
}

void ShopPanel::Imgui_ModifyUI()
{
	Vector3 Position = _DebugPos;
	ImGui::InputFloat3("Position##ShopPanel", Position);
	_DebugPos = Position;

	Vector3 Scale = _DebugScale;
	ImGui::InputFloat3("Scale##ShopPanel", Scale);
	_DebugScale = Scale;

	Vector3 Rot = _DebugRot;
	ImGui::InputFloat3("Rot##ShopPanel", Rot);
	_DebugRot = Rot;

	Vector2 MinUV = _DebugMinUV;
	ImGui::InputFloat2("MinUV##ShopPanel", MinUV);
	_DebugMinUV = MinUV;

	Vector2 MaxUV = _DebugMaxUV;
	ImGui::InputFloat2("MaxUV##ShopPanel", MaxUV);
	_DebugMaxUV = MaxUV;
}