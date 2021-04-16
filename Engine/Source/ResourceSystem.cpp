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
	//���ҽ� ���纻 �����̳�
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
	for (auto iterType = m_Origin.begin(); iterType != m_Origin.end(); iterType++)
	{
		//���纻 �����̳ʿ��� �˻��� ������ TypeID
		nTypeID = iterType->first;
		for (auto iterResource = iterType->second.begin(); iterResource != iterType->second.end();)
		{
			//���纻 �����̳ʿ��� �˻��� ������ ���ҽ� Ű ��.
			sResourceKey = iterResource->first;

			//TypeID�� �ش��ϴ� ���纻 �����̳ʰ� �ִ��� Ȯ��.
			auto iterCloneType = m_Clone.find(nTypeID);
			//TypeID�� �ش��ϴ� ���纻 �����̳ʰ� ������ ���� ���ҽ� ����.
			if (m_Clone.end() == iterCloneType)
			{
				iterResource = iterType->second.erase(iterResource);
				continue;
			}
			//TypeID�� �ش��ϴ� ���纻 �����̳ʰ� �ִ� ���
			else
			{
				//���ҽ� Ű���� �ش��ϴ� ���纻 ���� �����̳ʰ� �ִ��� Ȯ��.
				auto iterCloneVector = iterCloneType->second.find(sResourceKey);
				//���ҽ� Ű���� �ش��ϴ� ���纻 ���� �����̳ʰ� ���� ��� ���� ���ҽ� ����.
				if (iterCloneType->second.end() == iterCloneVector)
				{
					iterResource = iterType->second.erase(iterResource);
					continue;
				}
				//���ҽ� Ű���� �ش��ϴ� ���纻 ���� �����̳ʰ� �ִ� ���
				else
				{
					//���纻 ���� �����̳ʸ� ��ȸ�ϸ� ��븸��� ���纻(nullptr)�� ���� �����̳ʿ��� ����.
					for (auto iterCloneResource = iterCloneVector->second.begin(); iterCloneResource != iterCloneVector->second.end(); )
					{
						if (nullptr == *iterCloneResource)
							iterCloneResource = iterCloneVector->second.erase(iterCloneResource);
						else
							iterCloneResource++;
					}
					//���纻 ���� �����̳ʿ� �������� ���纻�� ���� ��� ���� ���ҽ� ����.
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
