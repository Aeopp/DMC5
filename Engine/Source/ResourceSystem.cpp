#include "ResourceSystem.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(ResourceSystem)

ResourceSystem::ResourceSystem()
	: m_pDevice(nullptr)
{
}

void ResourceSystem::Free()
{
	//���ҽ� ���� �����̳�
	for (auto& rTypeContainer : m_Origin)
	{
		for (auto& pResource : rTypeContainer.second)
		{
			pResource.second.reset();
		}
		rTypeContainer.second.clear();
	}
	m_Origin.clear();
	//��ġ RefCount ����.
	SafeRelease(m_pDevice);
	Object::Free();
}

HRESULT ResourceSystem::ReadyResourceSystem(LPDIRECT3DDEVICE9 const _pDevice)
{
	m_pDevice = _pDevice;
	SafeAddRef(m_pDevice);
	return S_OK;
}

void ResourceSystem::Clear()
{
	//���� ���ҽ��� ����Ǿ� ��������� ���纻 �����̳ʸ� ���� Ȯ���ϰ�
	//���� ���ҽ��� ����Ǿ� ��������� ���� ��� ���� ���ҽ� ����.
	size_t nTypeID = 0;
	std::wstring sResourceKey = L"";

	for (auto& rMapType : m_Origin)
	{
		for (auto iterResource = rMapType.second.begin(); iterResource != rMapType.second.end(); )
		{
			if (0 == (*iterResource).second->GetRefCount())
			{
				(*iterResource).second.reset();
				iterResource = rMapType.second.erase(iterResource);
				continue;
			}
			++iterResource;
		}
	}
}
