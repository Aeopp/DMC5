#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

class ResourceBase;
class StaticMesh;
class Texture;
class ResourceManager
{
public:
	static ResourceManager Instance;
public:
	std::unordered_map<std::string, StaticMesh*>	m_MeshContainer;
	std::unordered_map<std::string, Texture*>		m_TexContainer;

	std::string m_sCurrSel;
public:
	ResourceManager();
	~ResourceManager();
public:
	void Update();
	void Render(std::string _sMesh);
private:
	void MenuBar();
	void OpenFileDialog();
	void ShowResourceList();
public:
	Texture* LoadTexture(std::filesystem::path _Path);
	HRESULT LoadMesh(std::filesystem::path _Path);

	void RayCast();
};

#endif // !__RESOURCE_MANAGER_H__