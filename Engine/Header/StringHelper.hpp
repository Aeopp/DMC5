#ifndef __STRINGHELPER_H__
#define __STRINGHELPER_H__

#include "EngineDefine.h"
#include "EngineTypedef.h"
#include <string>


BEGIN(ENGINE ) 
static inline std::wstring ToW(const std::string& StrA)
{
	std::wstring Convert;
	Convert.assign(std::begin(StrA), std::end(StrA));
	return Convert;
}
static inline std::string ToA(const std::wstring& StrW)
{
	std::string Convert;
	Convert.assign(std::begin(StrW), std::end(StrW));
	return Convert;
}
END

#endif // !__STRINGHELPER_H__