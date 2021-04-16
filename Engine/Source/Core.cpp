#include "Core.h"
#include "CoreSystem.h"
#include <d3dx9.h>
#include <d3dx9mesh.h>


USING(ENGINE)

ENGINE_DLL HINSTANCE			g_hInstance;
ENGINE_DLL HWND					g_hWnd;
ENGINE_DLL int					g_nWndCX;
ENGINE_DLL int					g_nWndCY;
ENGINE_DLL bool 				g_bWindowed;
ENGINE_DLL LPDIRECT3D9			g_pSDK;
ENGINE_DLL LPDIRECT3DDEVICE9	g_pDevice;
ENGINE_DLL bool					g_bDebugMode;
ENGINE_DLL bool                 g_bEditMode;
ENGINE_DLL bool                 g_bCollisionVisible;
ENGINE_DLL bool					g_bRenderTargetVisible;
ENGINE_DLL bool					g_bDebugRender;
ENGINE_DLL bool		            g_bDebugBoneToRoot;
ENGINE_DLL bool                 g_bRenderEdit;
ENGINE_DLL bool                 g_bRenderPtLightScissorTest;
ENGINE_DLL ID3DXMesh*		    g_pSphereMesh;

ENGINE_DLL std::vector<std::string> g_Logs;

void ENGINE_DLL Log(const std::string& PushLog)
{
	if (g_bEditMode)
	{
		// g_Logs.push_back(PushLog);
	}
}

std::weak_ptr<CoreSystem> Engine::m_pCoreSystem = CoreSystem::GetInstance();

HRESULT Engine::ReadyEngine(const bool bWindowed,
							const bool bMultiSample)
{
	if (nullptr == m_pCoreSystem.lock() || m_pCoreSystem.expired())
		return E_FAIL;
	return m_pCoreSystem.lock()->ReadyEngine(bWindowed, bMultiSample);
}

HRESULT Engine::UpdateEngine()
{
	if (nullptr == m_pCoreSystem.lock() || m_pCoreSystem.expired())
		return E_FAIL;
	return m_pCoreSystem.lock()->UpdateEngine();
}

HRESULT Engine::ReleaseEngine()
{
	CoreSystem::DeleteInstance();
	return S_OK;
}
