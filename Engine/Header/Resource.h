#ifndef __RESOURCE_H__
#define __RESOURCE_H__
#include "Object.h"

BEGIN(ENGINE)
class ENGINE_DLL Resource abstract : public Object
{
protected:
	LPDIRECT3DDEVICE9		m_pDevice;
	int						m_nRefCount;
public:
	std::weak_ptr<Resource>	m_pOrigin;
protected:
	explicit Resource(LPDIRECT3DDEVICE9 const _pDevice);
	explicit Resource(const Resource& _rOther);
	virtual ~Resource() = default;
	// Object을(를) 통해 상속됨
	virtual void Free()			PURE;
	virtual void Editor()override;
public:
	std::filesystem::path ResourcePath{};
	bool              bClone = true;
	virtual Resource* Clone()	PURE;

	void AddRef();
	void Release();

	int GetRefCount();
};
END
#endif // !__RESOURCE_H__