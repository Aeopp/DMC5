#ifndef __ENGINE_STDAFX_H__
#define __ENGINE_STDAFX_H__

#pragma region RapidJson
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/reader.h> 
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#pragma endregion

#pragma region IMGUI 
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui.h"
#pragma endregion

#pragma warning(disable : 4251)

#pragma region STANDARD
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <array>
#include <set>

#include <algorithm>
#include <functional>

#include <tchar.h>
#include <string>

#include <memory>
#include <mutex>
#include <process.h>

#include <filesystem>
#pragma endregion

#pragma region DIRECTX3D
#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma endregion


#pragma region ENGINE_HEADERS
#include "EngineDefine.h"
#include "EngineEnum.h"
#include "EngineExtern.h"
#include "EngineFunction.h"
#include "EngineTypedef.h"
#include "EngineStruct.h"
#pragma endregion


#pragma region DETECT_MEMORY_LEAK

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif // DBG_NEW
#endif // _DEBUG

#pragma endregion

#pragma region DEBUG_CONSOLE

#ifndef ENGINE_EXPORT
#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif //UNICODE
#endif //_DEBUG
#endif // !ENGINE_EXPORT

#pragma endregion

#endif // !__ENGINE_STDAFX_H__