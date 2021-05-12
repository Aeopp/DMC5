#include "ResourceSystem.h"

USING(ENGINE)
IMPLEMENT_SINGLETON(ResourceSystem)

ResourceSystem::ResourceSystem()
	: m_pDevice(nullptr)
{
}

void ResourceSystem::Free()
{
	//리소스 원본 컨테이너
	for (auto& rTypeContainer : m_Origin)
	{
		for (auto& pResource : rTypeContainer.second)
		{
			pResource.second.reset();
		}
		rTypeContainer.second.clear();
	}
	m_Origin.clear();
	//장치 RefCount 감소.
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
	//원본 리소스가 복사되어 사용중인지 복사본 컨테이너를 통해 확인하고
	//원본 리소스가 복사되어 사용중이지 않은 경우 원본 리소스 해제.
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
