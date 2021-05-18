// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client.h"
#include "Application.h"
#include <chrono>

#define MAX_LOADSTRING 128

#pragma region GLOBAL VARIABLE

TCHAR		szTitle[MAX_LOADSTRING];			// 제목 표시줄 텍스트.
TCHAR		szWindowClass[MAX_LOADSTRING];		// 메인 윈도우 클래스 이름.

#pragma endregion

#pragma region FUNCTION PROTOTYPE
ATOM                MyRegisterClass(HINSTANCE hInstance);
HRESULT             InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
#pragma endregion


// 윈도우 or 풀 스크린
static constexpr bool bWindowed = true;
static constexpr bool bMultiSample = false;
// 풀스크린 일때는 기본적으로 작동 true 일시 창모드 일때도 보더리스로 작동 !! 
static constexpr bool bBorderless = false;
static const std::filesystem::path SoundDirectoryPath = "..\\..\\Resource\\Sound\\";


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdParam, int nCmdShow)
{
	srand((unsigned int)time(NULL));

	g_nWndCX = 1280;
	g_nWndCY = 720;

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG


	//전역 문자열 초기화.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);

	//윈도우 클래스 등록.
	MyRegisterClass(hInstance);

	//윈도우 생성 및 응용 프로그램 초기화.
	if (FAILED(InitInstance(hInstance, nCmdShow)))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	// 메시지 루프.
	MSG tMessage;
	tMessage.message = WM_NULL;

	Application* pApplication = new Application;



	pApplication->ReadyApplication(bWindowed,bMultiSample  , SoundDirectoryPath);

	static constexpr float TargetDelta = 1.0f / 60.f;
	std::chrono::time_point<std::chrono::high_resolution_clock> PrevTime;
	PrevTime = std::chrono::high_resolution_clock::now();

	while (true)
	{
		if (PeekMessage(&tMessage, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == tMessage.message)
				break;

			if (!TranslateAccelerator(tMessage.hwnd, hAccelTable, &tMessage))
			{
				TranslateMessage(&tMessage);
				DispatchMessage(&tMessage);
			}
		}
		else
		{
			auto CurTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float, std::ratio<1, 1000>> Delta = CurTime - PrevTime;
			const float CurDt = (Delta.count() * 0.001f);
			const float UpdateDelta = g_bFixedDeltaTime ? TargetDelta : CurDt;

			if (g_bFrameLimit)
			{
				if (CurDt >= TargetDelta)
				{
					PrevTime = CurTime;
					pApplication->UpdateApplication(UpdateDelta);
				}
			}
			else
			{
				PrevTime = CurTime;
				pApplication->UpdateApplication(UpdateDelta);
			}
		}
	}

	pApplication->ReleaseApplication();

	delete pApplication;

	return int(tMessage.wParam);
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);

	if (bWindowed == false)
	{
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	}
	else
	{
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	}
	

	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
HRESULT InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInstance = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

							 //윈도우 크기 보정.
	RECT tWndRect = { 0,0,g_nWndCX,g_nWndCY };
	AdjustWindowRect(&tWndRect, WS_OVERLAPPEDWINDOW, FALSE);

	//윈도우 생성.
	if (bWindowed == true)
	{
		// 창모드 .. 
		if (bBorderless == false)
		{
			g_hWnd = CreateWindow(
				szWindowClass,
				szTitle,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				0,
				tWndRect.right - tWndRect.left,
				tWndRect.bottom - tWndRect.top,
				nullptr,
				nullptr,
				hInstance,
				nullptr);
		}
		else
		{

			g_hWnd = CreateWindow(
				szWindowClass,
				szTitle,
				WS_EX_TOPMOST |
				WS_POPUP,
				0,
				0,
				g_nWndCX,
				g_nWndCY,
				NULL,
				NULL,
				hInstance,
				NULL);
		}
	}
	else
	{
		// 전체화면 ...
		
		g_hWnd = CreateWindow(
			szWindowClass,
			szTitle,
			WS_EX_TOPMOST |
			WS_POPUP,
			0,
			0,
			g_nWndCX,
			g_nWndCY,
			NULL,
			NULL,
			hInstance,
			NULL);
	};


	if (INVALID_HANDLE_VALUE == g_hWnd)
	{
		return E_FAIL;
	}
	//생성한 윈도우 표시.
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, nMessage, wParam, lParam))
		return true;

	switch (nMessage)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, nMessage, wParam, lParam);
		}
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:

			PostQuitMessage(0);
			break;
		}
		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, nMessage, wParam, lParam);
	}
	return 0;
}


