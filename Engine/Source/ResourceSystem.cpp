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
	//리소스 복사본 컨테이너
	for (auto& rTypeContainer : m_Clone)
	{
		for (auto& rResourceContainer : rTypeContainer.second)
		{
			rResourceContainer.second.clear();
			rResourceContainer.second.shrink_to_fit();
		}
		rTypeContainer.second.clear();
	}
	m_Clone.clear();
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
	for (auto iterType = m_Origin.begin(); iterType != m_Origin.end(); iterType++)
	{
		//복사본 컨테이너에서 검색할 원본의 TypeID
		nTypeID = iterType->first;
		for (auto iterResource = iterType->second.begin(); iterResource != iterType->second.end();)
		{
			//복사본 컨테이너에서 검색할 원본의 리소스 키 값.
			sResourceKey = iterResource->first;

			//TypeID에 해당하는 복사본 컨테이너가 있는지 확인.
			auto iterCloneType = m_Clone.find(nTypeID);
			//TypeID에 해당하는 복사본 컨테이너가 없으면 원본 리소스 해제.
			if (m_Clone.end() == iterCloneType)
			{
				iterResource = iterType->second.erase(iterResource);
				continue;
			}
			//TypeID에 해당하는 복사본 컨테이너가 있는 경우
			else
			{
				//리소스 키값에 해당하는 복사본 벡터 컨테이너가 있는지 확인.
				auto iterCloneVector = iterCloneType->second.find(sResourceKey);
				//리소스 키값에 해당하는 복사본 벡터 컨테이너가 없는 경우 원본 리소스 해제.
				if (iterCloneType->second.end() == iterCloneVector)
				{
					iterResource = iterType->second.erase(iterResource);
					continue;
				}
				//리소스 키값에 해당하는 복사본 벡터 컨테이너가 있는 경우
				else
				{
					//복사본 벡터 컨테이너를 순회하며 사용만료된 복사본(nullptr)을 벡터 컨테이너에서 제거.
					for (auto iterCloneResource = iterCloneVector->second.begin(); iterCloneResource != iterCloneVector->second.end(); )
					{
						if (nullptr == *iterCloneResource)
							iterCloneResource = iterCloneVector->second.erase(iterCloneResource);
						else
							iterCloneResource++;
					}
					//복사본 벡터 컨테이너에 보관중인 복사본이 없는 경우 원본 리소스 해제.
					if (0 == iterCloneVector->second.size())
					{
						iterResource = iterType->second.erase(iterResource);
						continue;
					}
				}
			}
			iterResource++;
		}//Resource Key
	}//TypeID 
}
