#ifndef _DxHelper_H_
#define _DxHelper_H_
#include "EngineDefine.h"
#include "EngineTypedef.h"
BEGIN(ENGINE)
class ENGINE_DLL DxHelper
{
public:
	static HRESULT  DXGenTangentFrame (LPDIRECT3DDEVICE9 device, LPD3DXMESH mesh, LPD3DXMESH* newmesh);
};

END
#endif // !DxHelper


