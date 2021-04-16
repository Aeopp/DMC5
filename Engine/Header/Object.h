#ifndef __OBJECT_H__
#define __OBJECT_H__
#include "EngineStdafx.h"
BEGIN(ENGINE)
class ENGINE_DLL Object abstract
{
	friend Deleter<Object>;
protected:
	explicit Object();
	explicit Object(const Object& rOther);
	virtual ~Object() = default;
	virtual void Free()	PURE;
public:
	virtual void Editor();
	virtual std::string GetName();
	bool         bEdit = false;
	const uint32 UniqueID;
	std::string  EditName{};

	UINT32 m_nTag;
};
END
#endif // !__OBJECT_H__

