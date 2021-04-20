#include "framework.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "StaticMesh.h"
#pragma region RapidJson
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/reader.h> 
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#pragma endregion

GameObjectManager GameObjectManager::Instance;

GameObjectManager::GameObjectManager()
	: m_pSelected(nullptr)
{
	m_nCurrItem = -1;
}

GameObjectManager::~GameObjectManager()
{
	for (auto rPair : m_Container)
	{
		for (UINT i = 0; i < rPair.second.size(); ++i)
		{
			delete rPair.second[i];
		}
		rPair.second.clear();
		rPair.second.shrink_to_fit();
	}
	m_Container.clear();
}

void GameObjectManager::Update()
{
	if (Input::GetMouseDown(DIM_R))
	{
		GameObject* pPicking = Physics::Instance.RayCast();
		if (nullptr != pPicking)
		{
			m_pSelected = pPicking;

			for (UINT i = 0; i < m_Container[m_pSelected->m_sMesh].size(); ++i)
			{
				if (m_pSelected == m_Container[m_pSelected->m_sMesh][i])
				{
					m_nCurrItem = m_pSelected->m_nListIdx;
					break;
				}
			}
		}
		//ResourceManager::Instance.RayCast();
	}

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(51.f / 255.f, 51.f / 255.f, 51.f / 255.f, 1.f));

	ImGui::Begin("GameObject",nullptr,ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);
	ShowMenuBar();
	ShowGameObjectList();
	ImGui::PopStyleColor();
	ImGui::End();
}

void GameObjectManager::Render()
{
	for (auto& rPair : m_Container)
	{
		for (UINT i = 0; i < rPair.second.size(); ++i)
		{
			rPair.second[i]->Update();
			D3DXMATRIX matWorld = rPair.second[i]->m_pTransform->GetWorldMatrix();
			g_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
			ResourceManager::Instance.Render(rPair.first);
		}
	}
}

void GameObjectManager::ShowMenuBar()
{
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(64.f / 255.f, 64.f / 255.f, 64.f / 255.f, 1.f));
	ImGui::BeginMenuBar();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ItemSpacing = ImVec2(0.f, 0.f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(103.f / 255.f, 103.f / 255.f, 103.f / 255.f, 1.f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_Text] = ImVec4(194.f / 255.f, 194.f / 255.f, 194.f / 255.f, 1.f);

	ImGui::Button("SAVE");
	if (ImGui::IsItemClicked())
	{
		Save();
	}

	ImGui::Button("LOAD");
	if (ImGui::IsItemClicked())
	{
		Load();
	}

	ImGui::Button("DELETE");
	if (ImGui::IsItemClicked())
	{
		DeleteGameObject();
	}

	ImGui::PopStyleColor();
	ImGui::EndMenuBar();
}

void GameObjectManager::ShowGameObjectList()
{
	int nItemCount = 0;
	for (auto& rPair : m_Container)
	{
		nItemCount += rPair.second.size();
	}

	char** ppIndex;
	ppIndex = new char* [nItemCount];

	int nIndex = 0;
	for (auto& rPair : m_Container)
	{
		for (int i = 0; i < rPair.second.size(); ++i)
		{
			ppIndex[nIndex] = new char[MAX_PATH];
			char szIndex[10] = {};
			_itoa_s(rPair.second[i]->m_nIndex, szIndex, 10, 10);

			std::filesystem::path sPath = rPair.first;
			sPath = sPath.filename();
			
			std::string sTemp = sPath.string();
			UINT nDot = sTemp.find('.');
			sTemp = sTemp.substr(0, nDot);

			sTemp += " / ";
			sTemp += szIndex;

			strcpy_s(ppIndex[nIndex], sizeof(char) * MAX_PATH, sTemp.c_str());

			rPair.second[i]->m_nListIdx = nIndex++;
		}
	}

	if (ImGui::ListBox("##GameObjectList", &m_nCurrItem, ppIndex, nItemCount, nItemCount))
	{
		if (-1 != m_nCurrItem/* && ImGui::IsItemClicked()*/)
		{
			std::string sSelected = ppIndex[m_nCurrItem];
			size_t nToken = sSelected.find('/');

			std::string sKey = sSelected.substr(0, nToken - 1);
			std::string sIndex = sSelected.substr(nToken + 2, sSelected.length() - 1);

			int nIndex = atoi(sIndex.c_str());


			for (auto& rPair : m_Container)
			{
				std::filesystem::path sPath = rPair.first;
				sPath = sPath.filename();

				std::string sTemp = sPath.string();
				UINT nDot = sTemp.find('.');
				sTemp = sTemp.substr(0, nDot);

				if (sKey == sTemp)
				{
					for (auto pGameObject : rPair.second)
					{
						if (pGameObject->m_nIndex == nIndex)
						{
							m_pSelected = pGameObject;
							break;
						}
					}
					break;
				}
			}

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				if (nullptr != m_pSelected)
				{
					Camera::SetCameraFocus(m_pSelected->m_pTransform->GetPosition());
				}
			}
		/*	for (auto pGameObject : m_Container[sKey])
			{

				if (pGameObject->m_nListIdx == nListIdx)
				{
					m_pSelected = pGameObject;
					break;
				}
			}*/
		}
	}
	else
	{
	/*	if (ImGui::IsItemClicked())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				if (nullptr != m_pSelected)
				{
					Camera::SetCameraFocus(m_pSelected->m_pTransform->GetPosition());
				}
			}
		}*/
	}

	for (int i = 0; i < nItemCount; ++i)
	{
		delete[] ppIndex[i];
	}
	delete[] ppIndex;
}

void GameObjectManager::AddGameObject(std::string _sMeshID, PxTriangleMesh* _pTriangleMesh)
{
	//게임 오브젝트 동적 생성 및 초기화.
	GameObject* pInstance = new GameObject;

	pInstance->m_sMesh = _sMeshID;
	pInstance->CreateRigidActor(_pTriangleMesh);
	//
	auto iterFind = m_Container.find(_sMeshID);

	if (m_Container.end() == iterFind)
	{
		iterFind = m_Container.emplace(_sMeshID, std::vector<GameObject*>()).first;
	}

	iterFind->second.push_back(pInstance);
}

void GameObjectManager::AddGameObject(GameObject* _pGameObject)
{
	//
	auto iterFind = m_Container.find(_pGameObject->m_sMesh);

	if (m_Container.end() == iterFind)
	{
		iterFind = m_Container.emplace(_pGameObject->m_sMesh, std::vector<GameObject*>()).first;
	}

	iterFind->second.push_back(_pGameObject);
}

void GameObjectManager::DeleteGameObject()
{
	if (nullptr == m_pSelected)
		return;

	std::string sKey = m_pSelected->m_sMesh;

	for (auto iter = m_Container[sKey].begin(); iter != m_Container[sKey].end(); ++iter)
	{
		if (*iter == m_pSelected)
		{
			delete* iter;
			m_Container[sKey].erase(iter);
			m_pSelected = nullptr;
			m_nCurrItem = -1;
			break;
		}
	}
}

void GameObjectManager::Save()
{
	OPENFILENAME ofn = {};
	TCHAR szFile[MAX_PATH] = { 0 };
	// Initialize remaining fields of OPENFILENAME structure
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("All\\0*.*\\0Text\\0*.TXT\\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = L"..\\..\\..\\Resource\\";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		ofn.lpstrFile;

		using namespace rapidjson;

		StringBuffer StrBuf{};
		PrettyWriter<StringBuffer> Writer(StrBuf);

		Writer.StartObject();
		Writer.Key("GameObject");
		Writer.StartArray();
		for (auto& rPair : m_Container)
		{
			Writer.StartObject();
			Writer.Key("Mesh");
			Writer.String(rPair.first.c_str());
			Writer.Key("List");
			Writer.StartArray();

			for (auto& pGameObject : rPair.second)
			{
				Writer.StartObject();
				
				Writer.Key("Scale");
				Writer.StartArray();
				Writer.Double(pGameObject->m_pTransform->m_vWorldScale.x);
				Writer.Double(pGameObject->m_pTransform->m_vWorldScale.y);
				Writer.Double(pGameObject->m_pTransform->m_vWorldScale.z);
				Writer.EndArray();
				Writer.Key("Rotation");
				Writer.StartArray();
				Writer.Double(pGameObject->m_pTransform->m_vWorldRotation.x);
				Writer.Double(pGameObject->m_pTransform->m_vWorldRotation.y);
				Writer.Double(pGameObject->m_pTransform->m_vWorldRotation.z);
				Writer.EndArray();
				Writer.Key("Position");
				Writer.StartArray();
				Writer.Double(pGameObject->m_pTransform->m_vWorldPosition.x);
				Writer.Double(pGameObject->m_pTransform->m_vWorldPosition.y);
				Writer.Double(pGameObject->m_pTransform->m_vWorldPosition.z);
				Writer.EndArray();
				Writer.EndObject();
			}

			Writer.EndArray();
			Writer.EndObject();

		}
		Writer.EndArray();
		Writer.EndObject();


		std::ofstream Of{ ofn.lpstrFile };
		Of << StrBuf.GetString();
	}
}

void GameObjectManager::Load()
{
	OPENFILENAME ofn = {};
	TCHAR szFile[260] = { 0 };
	// Initialize remaining fields of OPENFILENAME structure
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("All\\0*.*\\0Text\\0*.TXT\\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = g_BasePath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		using namespace rapidjson;

		std::filesystem::path sOpenFilePath = ofn.lpstrFile;

		std::ifstream inputStream{ sOpenFilePath };

		if (false == inputStream.is_open())
			return;

		IStreamWrapper inputSW(inputStream);
		Document docu{};
		docu.ParseStream(inputSW);

		if (docu.HasParseError())
			return;

		const Value& loadData = docu["GameObject"];

		std::filesystem::path sPath;
		for(auto iter = loadData.Begin(); iter != loadData.End(); ++iter)
		{
			sPath = iter->FindMember("Mesh")->value.GetString();

			string sMeshPath = g_BasePath.string();

			UINT sIndex = sMeshPath.find("Resource");

			sMeshPath = sMeshPath.substr(0, sIndex - 1) + "\\" + sPath.string();

			//경로 설정 및 메쉬 로딩.
			ResourceManager::Instance.LoadMesh(sMeshPath);

			PxTriangleMesh* pTriangleMesh = (ResourceManager::Instance.m_MeshContainer[sPath.string()])->m_pTriangleMesh;

			auto objArray = iter->FindMember("List")->value.GetArray();

			for (auto iterObj = objArray.begin(); iterObj != objArray.end(); iterObj++)
			{
				GameObject* pGameObject = new GameObject;

				D3DXVECTOR3 vScale;
				auto scale = iterObj->FindMember("Scale")->value.GetArray();
				vScale.x = scale[0].GetDouble();
				vScale.y = scale[1].GetDouble();
				vScale.z = scale[2].GetDouble();

				D3DXVECTOR3 vRotation;
				auto rotation = iterObj->FindMember("Rotation")->value.GetArray();
				vRotation.x = rotation[0].GetDouble();
				vRotation.y = rotation[1].GetDouble();
				vRotation.z = rotation[2].GetDouble();

				D3DXVECTOR3 vPosition;
				auto position = iterObj->FindMember("Position")->value.GetArray();
				vPosition.x = position[0].GetDouble();
				vPosition.y = position[1].GetDouble();
				vPosition.z = position[2].GetDouble();

				pGameObject->m_sMesh = sPath.string();
				pGameObject->m_pTransform->SetScale(vScale);
				pGameObject->m_pTransform->SetRotation(vRotation);
				pGameObject->m_pTransform->SetPosition(vPosition);


				pGameObject->CreateRigidActor(pTriangleMesh);
				AddGameObject(pGameObject);
			}

		}
		//use ofn.lpstrFile here
	}
}
