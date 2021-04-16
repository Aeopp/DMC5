#include "RenderInterface.h"
#include "Renderer.h"
#include "Resources.h"
#include "Shader.h"

USING(ENGINE)
void RenderInterface::Initialize(const RenderProperty& _RenderProp)&
{
	this->_RenderProperty = _RenderProp;
};
//
//void  RenderInterface::RenderVariableBind(const UpdateInfo& CurUpdateInfo)
//{
//	_UpdateInfo = CurUpdateInfo;
//}
//
//void RenderInterface::RenderForwardAlphaBlendImplementation(const ImplementationInfo& _ImplInfo) {};
//
////void RenderInterface::RenderForwardAlphaBlend()
////{
////	const auto& _CurRenderInfo = Renderer::GetInstance()->CurrentRenderInfo;
////
////	if (auto SpShader = _ShaderInfo.GetShader(RenderProperty::Order::ForwardAlphaBlend) ;
////		SpShader)
////	{
////		auto Fx = SpShader->GetEffect();
////		Fx->SetMatrix("World", &_UpdateInfo.World);
////		Fx->SetMatrix("View",&_CurRenderInfo.CameraView);
////		Fx->SetMatrix("Projection", &_CurRenderInfo.CameraProjection);
////		/// <summary>
////		UINT Passes{ 0u };
////		if (FAILED(Fx->Begin(&Passes, NULL)))
////		{
////			PRINT_LOG(L"Failed!!", __FUNCTIONW__);
////		}
////		else
////		{
////			for (uint32 PassIdx = 0u; PassIdx < Passes; ++PassIdx)
////			{
////				Fx->BeginPass(PassIdx);
////				ImplementationInfo _ImplInfo{};
////				_ImplInfo.Fx = Fx;
////				RenderForwardAlphaBlendImplementation(_ImplInfo);
////				Fx->EndPass();
////			}
////			Fx->End();
////		}
////	};
////};
//void RenderInterface::RenderGBufferImplementation(const ImplementationInfo& _ImplInfo) {}
//
//void RenderInterface::RenderShadowImplementation(const ImplementationInfo& _ImplInfo)
//{
//
//};
////void RenderInterface::RenderGBuffer()
////{
////	const auto& _CurRenderInfo = Renderer::GetInstance()->CurrentRenderInfo;
////	
////	if (auto SpShader = _ShaderInfo.GetShader(RenderProperty::Order::GBuffer); 
////		SpShader)
////	{
////		auto Fx = SpShader->GetEffect();
////		Fx->SetMatrix("World", &_UpdateInfo.World);
////		Fx->SetMatrix("View", &_CurRenderInfo.CameraView);
////		Fx->SetMatrix("Projection", &_CurRenderInfo.CameraProjection);
////		/// <summary>
////		UINT Passes{ 0u };
////		if (FAILED(Fx->Begin(&Passes, NULL)))
////		{
////			PRINT_LOG(L"Failed!!",__FUNCTIONW__);
////		}
////		else
////		{
////			for (uint32 PassIdx = 0u; PassIdx < Passes; ++PassIdx)
////			{
////				Fx->BeginPass(PassIdx);
////				ImplementationInfo _ImplInfo{};
////				_ImplInfo.Fx = Fx;
////				RenderGBufferImplementation(_ImplInfo);
////				Fx->EndPass();
////			}
////			Fx->End();
////		}
////	};
////};
//
//void RenderInterface::RenderAlphaBlendEffectImplementation(const ImplementationInfo& _ImplInfo) {}
//
////void RenderInterface::RenderAlphaBlendEffect()
////{
////	const auto& _CurRenderInfo = Renderer::GetInstance()->CurrentRenderInfo;
////	
////	if (auto SpShader = _ShaderInfo.GetShader(RenderProperty::Order::AlphaBlendEffect);
////		SpShader)
////	{
////		auto Fx = SpShader->GetEffect();
////		Fx->SetMatrix("World", &_UpdateInfo.World);
////		Fx->SetMatrix("View", &_CurRenderInfo.CameraView);
////		Fx->SetMatrix("Projection", &_CurRenderInfo.CameraProjection);
////	
////		UINT Passes{ 0u };
////		if (FAILED(Fx->Begin(&Passes, NULL)))
////		{
////			PRINT_LOG(L"Failed!!", __FUNCTIONW__);
////		}
////		else
////		{
////			ImplementationInfo _ImplInfo{};
////			_ImplInfo.Fx = Fx;
////			RenderAlphaBlendEffectImplementation(_ImplInfo);
////			Fx->End();
////		}
////	};
////}
//
//void RenderInterface::RenderUIImplementation(const ImplementationInfo& _ImplInfo) {}
//
////void RenderInterface::RenderUI()
////{
////	const auto& _CurRenderInfo = Renderer::GetInstance()->CurrentRenderInfo;
////
////	if (auto SpShader = _ShaderInfo.GetShader(RenderProperty::Order::UI);
////		SpShader)
////	{
////		auto Fx = SpShader->GetEffect();
////		Fx->SetMatrix("Ortho", &_CurRenderInfo.Ortho);
////
////		UINT Passes{ 0u };
////		if (FAILED(Fx->Begin(&Passes, NULL)))
////		{
////			PRINT_LOG(L"Failed!!", __FUNCTIONW__);
////		}
////		else
////		{
////			ImplementationInfo _ImplInfo{};
////			_ImplInfo.Fx = Fx;
////			RenderUIImplementation(_ImplInfo);
////
////			Fx->End();
////		}
////	}
////}
//
////void RenderInterface::RenderDebug()
////{
////	const auto& _CurRenderInfo = Renderer::GetInstance()->CurrentRenderInfo;
////
////	if (auto SpShader = _ShaderInfo.GetShader(RenderProperty::Order::Debug);
////		SpShader)
////	{
////		auto Fx = SpShader->GetEffect();
////		Fx->SetMatrix("World", &_UpdateInfo.World);
////		Fx->SetMatrix("View", &_CurRenderInfo.CameraView);
////		Fx->SetMatrix("Projection", &_CurRenderInfo.CameraProjection);
////
////		UINT Passes{ 0u };
////		if (FAILED(Fx->Begin(&Passes, NULL)))
////		{
////			PRINT_LOG(L"Failed!!", __FUNCTIONW__);
////		}
////		else
////		{
////			for (uint32 i = 0; i < Passes; ++i)
////			{
////				Fx->BeginPass(i);
////				{
////					ImplementationInfo _ImplInfo{};
////					_ImplInfo.Fx = Fx;
////					RenderDebugImplementation(_ImplInfo);
////				}
////				Fx->EndPass();
////			}
////			Fx->End();
////		}
////	};
////}
//
//void RenderInterface::RenderDebugImplementation(const ImplementationInfo& _ImplInfo) {};
//
////void RenderInterface::RenderDebugBone()
////{
////	const auto& _CurRenderInfo = Renderer::GetInstance()->CurrentRenderInfo;
////
////	if (auto SpShader = _ShaderInfo.GetShader(RenderProperty::Order::DebugBone);
////		SpShader)
////	{
////		auto Fx = SpShader->GetEffect();
////		Fx->SetMatrix("World", &_UpdateInfo.World);
////		Fx->SetMatrix("View", &_CurRenderInfo.CameraView);
////		Fx->SetMatrix("Projection", &_CurRenderInfo.CameraProjection);
////
////		UINT Passes{ 0u };
////		if (FAILED(Fx->Begin(&Passes, NULL)))
////		{
////			PRINT_LOG(L"Failed!!", __FUNCTIONW__);
////		}
////		else
////		{
////			for (uint32 i = 0; i < Passes; ++i)
////			{
////				Fx->BeginPass(i);
////				{
////					ImplementationInfo _ImplInfo{};
////					_ImplInfo.Fx = Fx;
////					RenderDebugBoneImplementation(_ImplInfo);
////				}
////				Fx->EndPass();
////			}
////			Fx->End();
////		}
////	};
////
////}
//
//
//void RenderInterface::RenderDebugBoneImplementation(const ImplementationInfo& _ImplInfo)
//{
//
//};
//
//void RenderInterface::ShaderInfo::RegistShader(const RenderProperty::Order& RegistOrder,
//	const std::filesystem::path& LoadPath, const std::any& InitParams)&
//{
//	_Shaders[RegistOrder] = Resources::Load<ENGINE::Shader>(LoadPath, InitParams);
//}
//
//std::shared_ptr<ENGINE::Shader> RenderInterface::ShaderInfo::GetShader
//(const RenderProperty::Order& _RegistOrder) const&
//{
//	if (auto iter = _Shaders.find(_RegistOrder);
//		iter != std::end(_Shaders))
//	{
//		return iter->second;
//	}
//
//	Log("Failed to get shader Check whether the loading path and loading path are the same.");
//
//	return nullptr;
//}