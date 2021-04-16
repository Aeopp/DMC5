#ifndef __ENGINE_TYPEDEF_H__
#define __ENGINE_TYPEDEF_H__
#include <d3dx9math.h>
#include <string>
#pragma region TSTRING

#ifdef UNICODE
#ifndef __TSTRING_DEFINED__
#define __TSTRING_DEFINED__
typedef std::wstring TSTRING;
#endif // !__TSTRING_DEFINED__
#else
#ifndef __TSTRING_DEFINED__
#define __TSTRING_DEFINED__
typedef std::string TSTRING;
#endif // !__TSTRING_DEFINED__
#endif

#pragma endregion
#pragma region Math
using Vector2    = D3DXVECTOR2;
using Vector4    = D3DXVECTOR4;
using Vector3    = D3DXVECTOR3;
using Matrix     = D3DXMATRIX;
using Quaternion = D3DXQUATERNION;
#pragma endregion
#pragma region Builtin
using int64 = long long;
using uint64 = unsigned long long;
using int32 = int;
using uint32 = unsigned int;
using int16 = short;
using uint16 = unsigned short;
using int8 = char;
using uint8 = unsigned char;
#pragma endregion

#endif // !__ENGINE_TYPEDEF_H__