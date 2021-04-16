#ifndef __RESOURCE_SYSTEM_H__
#define __RESOURCE_SYSTEM_H__
#include "Object.h"
#include "Resource.h"
#include <any>

BEGIN(ENGINE)
class Resource;
class ResourceSystem final : public Object
{
	DECLARE_SINGLETON(ResourceSystem)
private:
	//
	typedef std::unordered_map <size_t, std::unordered_map<std::wstring, std::shared_ptr<Resource>>> ORIGIN;
	typedef std::unordered_map <size_t, std::unordered_map<std::wstring, std::vector<std::shared_ptr<Resource>>>> CLONE;

	ORIGIN	m_Origin;
	CLONE	m_Clone;

	LPDIRECT3DDEVICE9	m_pDevice;
private:
	explicit ResourceSystem();
	virtual ~ResourceSystem() = default;
	// Object��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	HRESULT ReadyResourceSystem(LPDIRECT3DDEVICE9 const _pDevice);
public:
	//�� ��ȯ �� ���̻� ������ �ʴ� ���ҽ� ����.
	void Clear();
public:
	template <typename TYPE>
	std::shared_ptr<TYPE> Load(const std::filesystem::path _Path ,
								const std::any& InitParams);
};
END
#include"ResourceSystem.hpp"
#endif // !__RESOURCE_SYSTEM_H__

