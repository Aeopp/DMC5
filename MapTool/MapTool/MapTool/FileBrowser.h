#ifndef __FILE_BROWSER_H__
#define __FILE_BROWSER_H__

class FileBrowser
{
public:
	static FileBrowser Instance;
	string m_sBasePath;
	vector<string> m_vecPath;
public:
	FileBrowser();
	~FileBrowser();
public:
	void Update();
private:
	void ShowPath();
};
#endif // !__FILE_BROWSER_H__