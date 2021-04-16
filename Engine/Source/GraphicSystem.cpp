#include "GraphicSystem.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(GraphicSystem)

GraphicSystem::GraphicSystem()
	: m_pSDK(nullptr)
	, m_pDevice(nullptr)
{
}

void GraphicSystem::Free()
{
	SafeRelease(m_pDevice);
	SafeRelease(m_pSDK);
}

HRESULT GraphicSystem::ReadyGraphicSystem(
	const bool bWindowed ,
	const bool bMultiSample)
{
	g_pSDK =m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	g_bWindowed = bWindowed;

	if (nullptr == m_pSDK)
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to Direct3DCreate9"));
		return E_FAIL;
	}

	D3DCAPS9 tD3DCaps;
	ZeroMemory(&tD3DCaps, sizeof(D3DCAPS9));

	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &tD3DCaps)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to GetDeviceCaps"));
		SafeRelease(m_pSDK);
		return E_FAIL;
	}

	DWORD dwVertexProcessing = 0;
	if ((D3DDEVCAPS_HWTRANSFORMANDLIGHT & tD3DCaps.DevCaps))
	{
		dwVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}	
	else
	{
		dwVertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		PRINT_LOG(L"Warning!", L"SOFTWARE_VERTEXPROCESSING!");
	}
		

	dwVertexProcessing |= D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS tD3DPP;
	ZeroMemory(&tD3DPP, sizeof(D3DPRESENT_PARAMETERS));

	tD3DPP.BackBufferWidth = g_nWndCX;
	tD3DPP.BackBufferHeight = g_nWndCY;
	tD3DPP.BackBufferFormat = D3DFMT_A8R8G8B8;
	tD3DPP.BackBufferCount = 1;
	tD3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;
	tD3DPP.MultiSampleQuality = 0;

	if (bMultiSample)
	{
		// 가장 높은 퀼리티의 안티엘리어싱 수준과 레벨을 찾는다 .
		for (uint32 i = 2; i <= 16; ++i)
		{
			unsigned long QualtyLevel{ 0u };

			if (SUCCEEDED(m_pSDK->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL, tD3DPP.BackBufferFormat, g_bWindowed,
				static_cast<D3DMULTISAMPLE_TYPE>(i), &QualtyLevel)))
			{
				tD3DPP.MultiSampleType = static_cast<D3DMULTISAMPLE_TYPE>(i);
				tD3DPP.MultiSampleQuality = QualtyLevel - 1;
			}
		}
	}
	

	tD3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	tD3DPP.hDeviceWindow = g_hWnd;

	tD3DPP.Windowed = g_bWindowed;
	tD3DPP.EnableAutoDepthStencil = TRUE;
	tD3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;
	tD3DPP.Flags = 0;
	tD3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	tD3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		g_hWnd, dwVertexProcessing, &tD3DPP, &m_pDevice)))
	{
		PRINT_LOG(TEXT("Warning"), TEXT("Failed to CreateDevice"));
		SafeRelease(m_pSDK);
		return E_FAIL;
	}

	g_pDevice = m_pDevice;

	// HDR 렌더링이 가능한 상태인지 체크.
	D3DDISPLAYMODE Mode;

	if (FAILED(g_pSDK->GetAdapterDisplayMode(0, &Mode))) {
		PRINT_LOG(L"Could not get adapter mode", L"Could not get adapter mode");
		return E_FAIL;
	}

	if (FAILED(g_pSDK->CheckDeviceFormat(0, D3DDEVTYPE_HAL, Mode.Format, D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F))) {
		PRINT_LOG(L"No floating point rendertarget support", L"No floating point rendertarget support");
		return E_FAIL;
	}

	if (FAILED(g_pSDK->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, Mode.Format, D3DFMT_A16B16G16R16F, D3DFMT_D24S8))) {
		PRINT_LOG(L"D3DFMT_A16B16G16R16F does not support D3DFMT_D24S8", L"D3DFMT_A16B16G16R16F does not support D3DFMT_D24S8");
		return E_FAIL;
	}
	return S_OK;
}

void GraphicSystem::Begin()&
{
	/*m_pDevice->Clear(0, nullptr,
		D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DXCOLOR(0, 0, 100, 255), 1.f, 0);*/
	m_pDevice->BeginScene();
}

void GraphicSystem::End()&
{
	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, g_hWnd, nullptr);
}

LPDIRECT3DDEVICE9 GraphicSystem::GetDevice()
{
	return m_pDevice;
}
