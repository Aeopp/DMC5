#ifndef _FILEHELPER__H_
#define _FILEHELPER__H_
#include "EngineDefine.h"
#include <filesystem>
#include "EngineTypedef.h"

BEGIN(ENGINE)
class ENGINE_DLL FileHelper
{
	public:
		static std::filesystem::path OpenDialogBox();
};
END

#endif 


