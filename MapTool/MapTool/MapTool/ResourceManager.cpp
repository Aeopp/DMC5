#include "framework.h"
#include "ResourceManager.h"
#include "StaticMesh.h"
#include "Texture.h"
#include <filesystem>

ResourceManager ResourceManager::Instance;

ResourceManager::ResourceManager()
{
	
}

ResourceManager::~ResourceManager()
{
	for (auto& rPair : m_MeshContainer)
		delete rPair.second;
	for (auto& rPair : m_TexContainer)
		delete rPair.second;
}

void ResourceManager::Update()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(51.f / 255.f, 51.f / 255.f, 51.f / 255.f, 1.f));
	ImGui::Begin("RESOURCE",nullptr,ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);
	
	MenuBar();
	ShowResourceList();

	ImGui::PopStyleColor();
	ImGui::End();
}

void ResourceManager::Render(std::string _sMesh)
{
	for (auto& rPair : m_MeshContainer)
	{
		if (_sMesh == rPair.first)
		{
			rPair.second->Render();
			return;
		}
	}
}

void ResourceManager::MenuBar()
{
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(64.f / 255.f, 64.f / 255.f, 64.f / 255.f, 1.f));
	ImGui::BeginMenuBar();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ItemSpacing = ImVec2(0.f, 0.f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(103.f / 255.f, 103.f / 255.f, 103.f / 255.f, 1.f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_Text] = ImVec4(194.f / 255.f, 194.f / 255.f, 194.f / 255.f, 1.f);

	ImGui::Button("LOAD");
	if (ImGui::IsItemClicked())
	{
		OpenFileDialog();
	}

	ImGui::Button("ADD");
	if (ImGui::IsItemClicked())
	{
		if ("" != m_sCurrSel)
		{
			for (auto& rPair : m_MeshContainer)
			{
				std::filesystem::path sKey;
				sKey = rPair.first;
				sKey = sKey.filename();

				if (m_sCurrSel == sKey.string())
				{
					GameObjectManager::Instance.AddGameObject(rPair.first, rPair.second->m_pTriangleMesh);
					break;
				}
			}
		}
	}

	ImGui::PopStyleColor();
	ImGui::EndMenuBar();
}

void ResourceManager::OpenFileDialog()
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
		//use ofn.lpstrFile here
		LoadMesh(ofn.lpstrFile);
		//LoadMesh(L"..\\..\\..\\Resource\\Mesh\\Static\\Sphere.fbx");
	}
}

void ResourceManager::ShowResourceList()
{
	static int nCurrItem = -1;
	

	char** ppItems;

	ppItems = new char* [m_MeshContainer.size()];

	UINT nIndex = 0;
	for (auto& rPair : m_MeshContainer)
	{
		std::filesystem::path fileName = rPair.first;
		fileName = fileName.filename();
		ppItems[nIndex] = new char[fileName.string().length() + 1];
		ZeroMemory(ppItems[nIndex], sizeof(char) * fileName.string().length() + 1);
		memcpy(ppItems[nIndex], fileName.string().c_str(), sizeof(char) * fileName.string().length());

		++nIndex;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
	ImGui::PushItemWidth(ImGui::GetWindowWidth() - 18);
	ImGui::ListBox("##Mesh", &nCurrItem, ppItems, m_MeshContainer.size(), m_MeshContainer.size());
	
	if (-1 != nCurrItem)
	{
		m_sCurrSel = ppItems[nCurrItem];
	}
	else
		m_sCurrSel = "";

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	for (UINT i = 0; i < m_MeshContainer.size(); ++i)
	{
		delete[] ppItems[i];
	}
	delete[] ppItems;
}

Texture* ResourceManager::LoadTexture(std::filesystem::path _Path)
{
	auto iterFind = m_TexContainer.find(_Path.string());

	if (iterFind != m_TexContainer.end())
		return iterFind->second->Clone();

	Texture* pTexture = new Texture;

	if (FAILED(pTexture->LoadTexture(_Path)))
	{
		delete pTexture;
		return nullptr;
	}

	m_TexContainer.emplace(_Path.string(), pTexture);

	return pTexture->Clone();
}

HRESULT ResourceManager::LoadMesh(std::filesystem::path _Path)
{
	auto iterFind = m_MeshContainer.find(_Path.string());

	if (iterFind == m_MeshContainer.end())
	{
		StaticMesh* pMesh = new StaticMesh;
		if (FAILED(pMesh->Load(_Path)))
		{
			delete pMesh;
			return E_FAIL;
		}

		string sPath = _Path.string();
		UINT nIndex = sPath.find("Resource");
		sPath = sPath.substr(nIndex, sPath.length() - nIndex);
		m_MeshContainer.emplace(sPath, pMesh);
	}
	return S_OK;
}

void ResourceManager::RayCast()
{
	POINT tCursor;
	GetCursorPos(&tCursor);
	ScreenToClient(g_hWnd, &tCursor);

	D3DVIEWPORT9 tViewPort;
	g_pDevice->GetViewport(&tViewPort);

	D3DXVECTOR3 vCursorPos(0.f, 0.f, 0.f);

	vCursorPos.x = tCursor.x / (tViewPort.Width * 0.5f) - 1.f;
	vCursorPos.y = tCursor.y / -(tViewPort.Height * 0.5f) + 1.f;
	vCursorPos.z = 1.f;

	D3DXMATRIX matProj;
	g_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMatrixInverse(&matProj, nullptr, &matProj);

	D3DXVec3TransformCoord(&vCursorPos, &vCursorPos, &matProj);

	D3DXVECTOR3 vRayOrigin(0.f, 0.f, 0.f);
	D3DXVECTOR3 vRayDir = vCursorPos - vRayOrigin;

	D3DXMATRIX matView;
	g_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	D3DXVec3TransformCoord(&vRayOrigin, &vRayOrigin, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	D3DXVec3Normalize(&vRayDir, &vRayDir);

	float fU, fV, fDist;

	for (auto& rPair : m_MeshContainer)
	{
		for (UINT i = 0; i < rPair.second->m_vecIndices.size(); i += 3)
		{
			D3DXVECTOR3 v1, v2, v3;
			memcpy_s(&v1, sizeof(D3DXVECTOR3), &rPair.second->m_vecVertices[rPair.second->m_vecIndices[i]], sizeof(D3DXVECTOR3));
			memcpy_s(&v2, sizeof(D3DXVECTOR3), &rPair.second->m_vecVertices[rPair.second->m_vecIndices[i + 1]], sizeof(D3DXVECTOR3));
			memcpy_s(&v3, sizeof(D3DXVECTOR3), &rPair.second->m_vecVertices[rPair.second->m_vecIndices[i + 2]], sizeof(D3DXVECTOR3));

			if (D3DXIntersectTri(&v1, &v2, &v3, &vRayOrigin, &vRayDir, &fU, &fV, &fDist))
			{
				int a = 0;
				cout << "IntersectTri" << endl;
			}
		}
	}
	PxVec3 vOrigin(vRayOrigin.x, vRayOrigin.y, vRayOrigin.z);
	PxVec3 vDir(vRayDir.x, vRayDir.y, vRayDir.z);
	PxRaycastBuffer rayCastBuf;
	bool bResult = Physics::Instance.m_pScene->raycast(vOrigin, vDir, 1000.f, rayCastBuf);
}
