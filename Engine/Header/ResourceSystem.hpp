#ifndef __RESOURCE_SYSTEM_HPP__
#define __RESOURCE_SYSTEM_HPP__
BEGIN(ENGINE)
template<typename TYPE>
inline std::shared_ptr<TYPE> ResourceSystem::Load(const std::filesystem::path _Path ,
													const std::any& InitParams)
{
	size_t nTypeID = typeid(TYPE).hash_code();

	//TypeID�� Ű ������ ���� pair�� �ִ��� Ȯ��.
	auto iterOriginType = m_Origin.find(nTypeID);
	//TypeID�� Ű ������ ���� pair�� ���� ��� pair �߰�.
	if (m_Origin.end() == iterOriginType)
		iterOriginType = m_Origin.emplace(nTypeID, std::unordered_map<std::wstring, std::shared_ptr<Resource>>()).first;
	//TypeID�� �ش��ϴ� �����̳ʿ� _Path�� Ű������ ���� ���� ���ҽ��� �ִ��� Ȯ��
	auto iterOriginResource = iterOriginType->second.find(_Path.wstring());
	//���� ���ҽ��� ���� ��� ���� ���ҽ� ���� �� ���� ���ҽ� �����̳ʿ� ����.
	if (iterOriginType->second.end() == iterOriginResource)
	{
		//���� ���ҽ� ����.
		Resource* pResource = TYPE::Create(m_pDevice, _Path , InitParams);
		if (nullptr == pResource)
		{
			Log("Failed to Load Resource.");
			PRINT_LOG(TEXT("Warning"), TEXT("Failed to Load Resource."));
			return std::shared_ptr<TYPE>();
		}
		//�����̳ʿ� ���� ���ҽ� ����.
		iterOriginResource = iterOriginType->second.emplace(_Path.wstring(), std::shared_ptr<Resource>(pResource, Deleter<Object>())).first;
	}

	//������ ���纻�� ����
	TYPE* pClone = (TYPE*)iterOriginResource->second->Clone();
	pClone->m_pOrigin = iterOriginResource->second;
	pClone->m_pOrigin.lock()->AddRef();
	////nTypeID�� Ű ������ ���� pair�� �ִ��� Ȯ��.
	//auto iterCloneType = m_Clone.find(nTypeID);
	////nTypeID�� Ű ������ ���� pair�� ���� ��� pair�߰�.
	//if (m_Clone.end() == iterCloneType)
	//	iterCloneType = m_Clone.emplace(nTypeID, std::unordered_map<std::wstring, std::vector<std::shared_ptr<Resource>>>()).first;
	////TypeID�� �ش��ϴ� �����̳ʿ� _Path�� Ű������ ���� pair�� �ִ��� Ȯ��.
	//auto iterCloneVector = iterCloneType->second.find(_Path.wstring());
	////TypeID�� �ش��ϴ� �����̳ʿ� _Path�� Ű������ ���� pair�� ���� ��� pair �߰�
	//if (iterCloneType->second.end() == iterCloneVector)
	//	iterCloneVector = iterCloneType->second.emplace(_Path.wstring(), std::vector<std::shared_ptr<Resource>>()).first;
	////pair�� ���纻 ����
	std::shared_ptr<TYPE> pReturn;
	pReturn.reset(pClone, Deleter<Object>());
	//iterCloneVector->second.emplace_back(pReturn);

	return pReturn;
}
END
#endif // !__RESOURCE_SYSTEM_HPP__