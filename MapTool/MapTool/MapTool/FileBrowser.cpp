#include "framework.h"
#include "FileBrowser.h"
#include "imgui_internal.h"

FileBrowser FileBrowser::Instance;

FileBrowser::FileBrowser()
{
	m_vecPath.push_back("Resource");
	m_vecPath.push_back("Mesh");
	m_sBasePath = "..\\..\\";
}

FileBrowser::~FileBrowser()
{
	m_vecPath.clear();
	m_vecPath.shrink_to_fit();
}

void FileBrowser::Update()
{

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(51.f / 255.f, 51.f / 255.f, 51.f / 255.f, 1.f));
	ImGui::Begin("PATH", nullptr , ImGuiWindowFlags_MenuBar);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(64.f / 255.f, 64.f / 255.f, 64.f / 255.f, 1.f);

	//ShowPath();
	ImGui::PopStyleColor();

	ImGui::End();
}

void FileBrowser::ShowPath()
{
	//ImGuiStyle& style = ImGui::GetStyle();

	//float fFrameHeight = ImGui::GetFrameHeight();
	//float flistItemHeight = GImGui->FontSize + style.ItemSpacing.y;

	//ImVec2 contentSize = ImGui::GetWindowSize() - style.WindowPadding * 2.f;
	//ImVec2 swSize = ImVec2(ImGui::CalcTextSize("Random").x + 140, style.WindowPadding.y * 2.f + fFrameHeight);
	//ImVec2 swContentSize = swSize - style.WindowPadding * 2.f;
	//ImVec2 nwSize = ImVec2(contentSize.x - style.ItemSpacing.x - swSize.x, swSize.y);

	//ImGui::BeginChild("##Path", nwSize, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar);
	//
	ImGui::BeginMenuBar();

	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->Fonts[0]->;

	//Button / Text Color
	ImGuiStyle& style = ImGui::GetStyle();
	style.ItemSpacing = ImVec2(0.f, 0.f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(103.f / 255.f, 103.f / 255.f, 103.f / 255.f, 1.f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_Text] = ImVec4(194.f / 255.f, 194.f / 255.f, 194.f / 255.f, 1.f);
	//

	for (UINT i = 0; i < m_vecPath.size(); ++i)
	{
		ImGui::Button(m_vecPath[i].c_str());
		ImGui::SameLine();
		if (i != m_vecPath.size() - 1)
		{
			ImGui::SameLine();
			ImGui::Button(">>");
		}
	}
	ImGui::EndMenuBar();

	//ImGui::EndChild();
}
