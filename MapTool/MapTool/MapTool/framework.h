// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <set>

#include <commdlg.h>

#include <filesystem>

#pragma region DIRECTX3D
#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma endregion

#pragma region MAPTOOL
#include "Extern.h"
#include "Enum.h"
#include "Function.h"
#include "Struct.h"
#pragma endregion

#pragma region IMGUI 
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "Inspector.h"
#include "GameObjectManager.h"
#include "ResourceManager.h"
#include "Input.h"
#include "Grid.h"
#include "Camera.h"

#pragma endregion


#pragma warning(disable : 4251)

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif //UNICODE
#endif //_DEBUG

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#ifndef DBG_NEW 
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//#endif // DBG_NEW
//#endif // _DEBUG

using namespace std;