#include "framework.h"
#include "Inspector.h"
#include "GameObject.h"
#include "Transform.h"

Inspector Inspector::Instance;

void Inspector::Update()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(51.f / 255.f, 51.f / 255.f, 51.f / 255.f, 1.f));
	ImGui::Begin("Inspector",nullptr,ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);
	DrawTransform();
	ImGui::PopStyleColor();
	ImGui::End();
}

void Inspector::DrawTransform()
{
	if (nullptr == GameObjectManager::Instance.m_pSelected)
		return;

	static bool MyButton = true;
	static float ZeroDotOne = 10.f;

	if (ImGui::Checkbox("ZeroDotButton", &MyButton))
	{

	}

	if (ImGui::InputFloat("ZeroDotOne", &ZeroDotOne))
	{

	}

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

	D3DXVECTOR3 vBuf;
	vBuf = GameObjectManager::Instance.m_pSelected->m_pTransform->GetScale();
	ImGui::Text("Scale");
	if (ImGui::InputScalar("X##Scale", ImGuiDataType_Float, &vBuf.x, MyButton ? &ZeroDotOne : NULL))
	{

	}
	//ImGui::SameLine();
	if (ImGui::InputScalar("Y##Scale", ImGuiDataType_Float, &vBuf.y, MyButton ? &ZeroDotOne : NULL))
	{

	}
	//ImGui::SameLine();
	if (ImGui::InputScalar("Z##Scale", ImGuiDataType_Float, &vBuf.z, MyButton ? &ZeroDotOne : NULL))
	{

	}
	GameObjectManager::Instance.m_pSelected->m_pTransform->SetScale(vBuf);

	vBuf = GameObjectManager::Instance.m_pSelected->m_pTransform->GetRotation();
	ImGui::Text("Rotation");
	if (ImGui::InputScalar("X##Rotation", ImGuiDataType_Float, &vBuf.x, MyButton ? &ZeroDotOne : NULL))
	{

	}
	//ImGui::SameLine();
	if (ImGui::InputScalar("Y##Rotation", ImGuiDataType_Float, &vBuf.y, MyButton ? &ZeroDotOne : NULL))
	{

	}
	//ImGui::SameLine();
	if (ImGui::InputScalar("Z##Rotation", ImGuiDataType_Float, &vBuf.z, MyButton ? &ZeroDotOne : NULL))
	{

	}
	GameObjectManager::Instance.m_pSelected->m_pTransform->SetRotation(vBuf);

	vBuf = GameObjectManager::Instance.m_pSelected->m_pTransform->GetPosition();
	ImGui::Text("Position");
	if (ImGui::InputScalar("X##Position", ImGuiDataType_Float, &vBuf.x, MyButton ? &ZeroDotOne : NULL))
	{

	}
	//ImGui::SameLine();
	if (ImGui::InputScalar("Y##Position", ImGuiDataType_Float, &vBuf.y, MyButton ? &ZeroDotOne : NULL))
	{

	}
	//ImGui::SameLine();
	if (ImGui::InputScalar("Z##Position", ImGuiDataType_Float, &vBuf.z, MyButton ? &ZeroDotOne : NULL))
	{

	}
	GameObjectManager::Instance.m_pSelected->m_pTransform->SetPosition(vBuf);

	/*if (ImGui::InputFloat3(" Scale", vBuf))
	{
		GameObjectManager::Instance.m_pSelected->m_pTransform->SetScale(vBuf);
	}

	vBuf = GameObjectManager::Instance.m_pSelected->m_pTransform->GetRotation();
	if (ImGui::InputFloat3(" Rotation", vBuf))
	{
		GameObjectManager::Instance.m_pSelected->m_pTransform->SetRotation(vBuf);
	}

	vBuf = GameObjectManager::Instance.m_pSelected->m_pTransform->GetPosition();
	if (ImGui::InputFloat3(" Position", vBuf))
	{
		GameObjectManager::Instance.m_pSelected->m_pTransform->SetPosition(vBuf);
	}*/
	ImGui::PopStyleVar();
}
