#include "inspector.hpp"

#include "editor.hpp"
#include "imgui_helper.h"
#include <ImguiNodeEditor/imgui_node_editor.h>

using namespace PC_EDITOR_CORE;


Inspector::~Inspector()
{
}

void Inspector::Update()
{
	EditorWindow::Update();


	if (m_Editor->m_SelectedEntityId == PC_CORE::INVALID_ENTITY_ID)
		return;

	Show();
	OnInput();

}

Inspector::Inspector(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
	m_ReflectedTypes = PC_CORE::Reflector::GetAllTypesFrom<PC_CORE::Component>();
}

void Inspector::Show()
{
	PC_CORE::EntityManager& entityManager = PC_CORE::World::GetWorld()->entityManager;
	PC_CORE::EntityId selectedId = m_Editor->m_SelectedEntityId;


	std::string* string = &entityManager.GetEntityName(selectedId);
	ImGui::PushID("EntityNameInput");
	ImGui::InputText("##EntityName", string->data(), string->size());
	ImGui::PopID();
	

	
	for (size_t i = 0; i < m_ReflectedTypes.size(); i++)
	{
		const uint32_t currentKeyComponent = m_ReflectedTypes[i]->HashKey;

		if (!entityManager.HasComponent(currentKeyComponent, selectedId))
			continue;

		PC_CORE::Component* component = static_cast<PC_CORE::Component*>(entityManager.GetComponent(currentKeyComponent, selectedId));
		auto reflectedMember = PC_CORE::Reflector::GetType(currentKeyComponent);

		const char* componentName = m_ReflectedTypes[i]->name.c_str();
		ImGui::Text(componentName);
		ImGui::Spacing();

		ImGui::PushID(static_cast<int>(currentKeyComponent));

		ImGui::Spacing();

		for (size_t j = 0; j < reflectedMember.members.size(); j++)
		{
			PC_CORE::Members& m = reflectedMember.members[j];
			ImGui::PushID(static_cast<int>(currentKeyComponent + j));
			ShowReflectedType(component, m);
			ImGui::PopID();
			ImGui::Spacing();
			
		}

		DeleteButton(m_Editor->m_SelectedEntityId, m_ReflectedTypes[i]->HashKey);

		ImGui::PopID();
	}

}

void Inspector::OnInput()
{

	if (ButtonCenteredOnLine("Add Component"))
	{
		ImGui::OpenPopup("Components");
	}


	ImGui::SameLine();
	if (ImGui::BeginPopup("Components"))
	{

		ImGui::SeparatorText("Component");
		for (auto& type : m_ReflectedTypes)
		{
			if (ImGui::Selectable(type->name.c_str()))
			{
				if (PC_CORE::World::GetWorld() == nullptr)
					continue;

				PC_CORE::World::GetWorld()->entityManager.AddComponent(type->HashKey, m_Editor->m_SelectedEntityId);
				
			}
		}

		ImGui::EndPopup();
	}

}
void Inspector::ShowReflectedType(void* begin, const PC_CORE::Members& _members)
{

	void* dataPosition = static_cast<char*>(begin) + _members.offset;
	const char* membersName = _members.membersName.c_str();
	const PC_CORE::ReflectedType& type = PC_CORE::Reflector::GetType(_members.typeKey);


	if (type.typeInfo.typeInfoFlags & PC_CORE::TypeFlag::COMPOSITE)
	{
		for (const PC_CORE::Members& m : type.members)
		{
			void* dataPosMember = static_cast<uint8_t*>(dataPosition) + m.offset;
			ImGui::PushID((m.membersName).c_str());
			ShowReflectedType(dataPosMember, m);
			ImGui::PopID();
			ImGui::Spacing();
		}
	}
	else if (type.typeInfo.typeInfoFlags & PC_CORE::TypeFlag::ARRAY)
	{

	}
	else
	{
		switch (type.typeInfo.dataNature)
		{
		case PC_CORE::DataNature::UNKNOWN:
			break;
		case PC_CORE::DataNature::BOOL:
			ImGui::Checkbox(membersName, static_cast<bool*>(dataPosition));
			break;
		case PC_CORE::DataNature::INT:
			ImGui::DragInt(membersName, static_cast<int*>(dataPosition));
			break;
		case PC_CORE::DataNature::UINT:
			ImGui::DragInt(membersName, static_cast<int*>(dataPosition), 0.1, 0);
			break;
		case PC_CORE::DataNature::FLOAT:
			ImGui::DragFloat(membersName, static_cast<float*>(dataPosition), 0.1, 0);
			break;
		case PC_CORE::DataNature::DOUBLE:
			ImGui::InputDouble(membersName, static_cast<double*>(dataPosition), 0.0);
			//ImGui::DragFloat(membersName, static_cast<double*>(dataPosition),1, 0);
			break;
		case PC_CORE::DataNature::VEC2:
			ImGui::DragFloat2(membersName, static_cast<float*>(dataPosition), 0.1, 0);
			break;
		case PC_CORE::DataNature::VEC3:
			if (_members.enumFlag & PC_CORE::MemberEnumFlag::COLOR)
			{
				ImGui::ColorPicker3(membersName, static_cast<float*>(dataPosition), ImGuiColorEditFlags_PickerHueWheel);
			}
			else
			{
				ImGui::DragFloat3(membersName, static_cast<float*>(dataPosition), 0.1, 0);
			}
			break;
		case PC_CORE::DataNature::VEC4:

			if (_members.enumFlag & PC_CORE::MemberEnumFlag::COLOR)
			{
				ImGui::ColorPicker4(membersName, static_cast<float*>(dataPosition), ImGuiColorEditFlags_PickerHueWheel);
			}
			else
			{
				ImGui::DragFloat4(membersName, static_cast<float*>(dataPosition), 0.1, 0);
			}
			break;

		case PC_CORE::DataNature::QUAT:

			if (_members.enumFlag & PC_CORE::MemberEnumFlag::EULER_ANGLES)
			{
				ImGui::DragFloat4(membersName, static_cast<float*>(dataPosition), 0.1, 0);
			}
			else
			{

				Tbx::Quaternionf& q = *static_cast<Tbx::Quaternionf*>(dataPosition);
				Tbx::Vector3f euler = q.ToEulerAngles() * Rad2Deg;

				ImGui::DragFloat3("Rotation", euler.GetPtr(), 0.1f);
				if (ImGui::IsItemEdited())
				{
					Tbx::Vector4f euler4 = {euler.x, euler.y, euler.z, 0.f};
					q = Tbx::Quaternionf::FromEuler(euler4 * Deg2Rad).Normalize();
				}
			}
			break;
		case PC_CORE::DataNature::STRING:
		{

			std::string* string = static_cast<std::string*>(dataPosition);
			ImGui::InputText(membersName, string->data(), string->size());
			break;
		}
		case PC_CORE::DataNature::COUNT:
		{

		}
		break;
		default:;
		}

	}

}


void Inspector::DeleteButton(PC_CORE::EntityId _entityId, uint32_t _componentId)
{

	if (ImGui::SmallButton("Delete Component"))
	{
		m_Editor->world.entityManager.RemoveComponent(_componentId, _entityId);
		//m_Editor->world.scene.RemoveComponent(_entity, _componentId);
	}
}

void Inspector::PrintArray(void* begin, const PC_CORE::Members& _members)
{

}



