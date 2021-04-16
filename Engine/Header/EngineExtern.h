#ifndef __ENGINE_EXTERN_H__
#define __ENGINE_EXTERN_H__
#include <vector>
#include <string>
#include <d3dx9.h>
#include <d3dx9mesh.h>

extern ENGINE_DLL HINSTANCE				g_hInstance;
extern ENGINE_DLL HWND					g_hWnd;
extern ENGINE_DLL int					g_nWndCX;
extern ENGINE_DLL int					g_nWndCY;
extern ENGINE_DLL bool					g_bWindowed;
extern ENGINE_DLL bool                  g_bDebugMode;
extern ENGINE_DLL bool                  g_bEditMode;
extern ENGINE_DLL bool                  g_bCollisionVisible;
extern ENGINE_DLL bool                  g_bRenderTargetVisible;
extern ENGINE_DLL bool					g_bDebugRender;
extern ENGINE_DLL bool                  g_bDebugBoneToRoot;
extern ENGINE_DLL bool					g_bRenderEdit;
extern ENGINE_DLL bool					g_bRenderPtLightScissorTest;
extern ENGINE_DLL LPDIRECT3D9			g_pSDK;
extern ENGINE_DLL LPDIRECT3DDEVICE9		g_pDevice;
extern ENGINE_DLL std::vector<std::string> g_Logs;
extern ENGINE_DLL ID3DXMesh*			g_pSphereMesh;

extern void ENGINE_DLL Log(const std::string& PushLog);
#endif // !__ENGINE_EXTERN_H__