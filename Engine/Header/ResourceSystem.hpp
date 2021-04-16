#ifndef __RESOURCE_SYSTEM_HPP__
#define __RESOURCE_SYSTEM_HPP__
BEGIN(ENGINE)
template<typename TYPE>
inline std::shared_ptr<TYPE> ResourceSystem::Load(const std::filesystem::path _Path ,
													const std::any& InitParams)
{
	size_t nTypeID = typeid(TYPE).hash_code();

	//TypeID를 키 값으로 갖는 pair가 있는지 확인.
	auto iterOriginType = m_Origin.find(nTypeID);
	//TypeID를 키 값으로 갖는 pair가 없는 경우 pair 추가.
	if (m_Origin.end() == iterOriginType)
		iterOriginType = m_Origin.emplace(nTypeID, std::unordered_map<std::wstring, std::shared_ptr<Resource>>()).first;
	//TypeID에 해당하는 컨테이너에 _Path를 키값으로 갖는 원본 리소스가 있는지 확인
	auto iterOriginResource = iterOriginType->second.find(_Path.wstring());
	//원본 리소스가 없는 경우 원본 리소스 생성 및 원본 리소스 컨테이너에 저장.
	if (iterOriginType->second.end() == iterOriginResource)
	{
		//원본 리소스 생성.
		Resource* pResource = TYPE::Create(m_pDevice, _Path , InitParams);
		if (nullptr == pResource)
		{
			Log("Failed to Load Resource.");
			PRINT_LOG(TEXT("Warning"), TEXT("Failed to Load Resource."));
			return std::shared_ptr<TYPE>();
		}
		//컨테이너에 원본 리소스 저장.
		iterOriginResource = iterOriginType->second.emplace(_Path.wstring(), std::shared_ptr<Resource>(pResource, Deleter<Object>())).first;
	}

	//원본의 복사본을 생성
	Resource* pClone = iterOriginResource->second->Clone();
	//nTypeID를 키 값으로 갖는 pair가 있는지 확인.
	auto iterCloneType = m_Clone.find(nTypeID);
	//nTypeID를 키 값으로 갖는 pair가 없는 경우 pair추가.
	if (m_Clone.end() == iterCloneType)
		iterCloneType = m_Clone.emplace(nTypeID, std::unordered_map<std::wstring, std::vector<std::shared_ptr<Resource>>>()).first;
	//TypeID에 해당하는 컨테이너에 _Path를 키값으로 갖는 pair가 있는지 확인.
	auto iterCloneVector = iterCloneType->second.find(_Path.wstring());
	//TypeID에 해당하는 컨테이너에 _Path를 키값으로 갖는 pair가 없는 경우 pair 추가
	if (iterCloneType->second.end() == iterCloneVector)
		iterCloneVector = iterCloneType->second.emplace(_Path.wstring(), std::vector<std::shared_ptr<Resource>>()).first;
	//pair에 복사본 저장
	std::shared_ptr<Resource> pReturn;
	pReturn.reset(pClone, Deleter<Object>());
	iterCloneVector->second.emplace_back(pReturn);

	return std::static_pointer_cast<TYPE>(pReturn);
}
END
#endif // !__RESOURCE_SYSTEM_HPP__