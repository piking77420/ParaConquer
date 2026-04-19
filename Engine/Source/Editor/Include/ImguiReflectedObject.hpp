#pragma once

#include "Imgui.h"
#include "EditorHeader.hpp"
#include "Reflection/Reflector.hpp"

namespace PC_EDITOR_CORE::ImGuiReflection
{
	template <typename T>
	requires (std::is_enum_v<T>)
	void DrawEnumStepper(T* enumValue)
	{
		assert(enumValue);

		static_assert(std::is_same_v<std::underlying_type_t<T>, uint8_t>,
			"Only support uint8_t for enum reflected");

		static const PC_CORE::ReflectedType& type = PC_CORE::Reflector::GetType<T>();

		const PC_CORE::ReflectedEnum& Renum = std::get<PC_CORE::ReflectedEnum>(type.metaData.data);

		std::string_view value{};
		uint8_t underlying = std::to_underlying(*enumValue);
		size_t currentIndex = 0ull;


		for (auto& en : Renum.members)
		{
			if (en.value == std::to_underlying(*enumValue)) // may do binary search
			{
				value = en.name;
				break;
			}
			currentIndex++;
		}

		if (!value.empty())
		{
			const uint8_t minValue = Renum.members.front().value;
			const uint8_t maxValue = Renum.members.back().value;

			ImGui::PushID(enumValue);
			if (ImGui::Button("< "))
			{
				const uint8_t old = static_cast<uint8_t>(*enumValue);

				size_t count = Renum.members.size();
				size_t prevIndex = (currentIndex + count - 1) % count;

				*enumValue = static_cast<T>(Renum.members[prevIndex % Renum.members.size()].value);
			}
			ImGui::SameLine();
			ImGui::TextUnformatted(value.data());
			ImGui::SameLine();

			if (ImGui::Button(" >"))
			{
				const uint8_t old = static_cast<uint8_t>(*enumValue);
				*enumValue = static_cast<T>(Renum.members[(currentIndex + 1) % Renum.members.size()].value);
			}

			ImGui::PopID();
		}

	}

	template <typename T>
	requires (std::is_enum_v<T>)
	bool DrawEnumButtonExclusive(T* enumValue)
	{
		assert(enumValue);

		static_assert(std::is_same_v<std::underlying_type_t<T>, uint8_t>,
			"Only support uint8_t for enum reflected");

		static const PC_CORE::ReflectedType& type = PC_CORE::Reflector::GetType<T>();

		const PC_CORE::ReflectedEnum& Renum = std::get<PC_CORE::ReflectedEnum>(type.metaData.data);

		uint8_t underlying = std::to_underlying(*enumValue);
		bool IsDirty = false;
		for (const auto& EnumV : Renum.members)
		{
			ImGui::PushID(EnumV.name.c_str());
			if (ImGui::RadioButton(EnumV.name.c_str(), underlying == EnumV.value))
			{
				IsDirty = *enumValue != static_cast<T>(EnumV.value);
				*enumValue = static_cast<T>(EnumV.value);
			}
			ImGui::PopID();
		}

		return IsDirty;
	}

	template <typename T>
	requires std::is_enum_v<T>
	void DrawEnumMenue(std::string_view _MenuName, T* _EnumValue)
	{
		assert(_EnumValue);

		static_assert(std::is_same_v<std::underlying_type_t<T>, uint8_t>,
			"Only support uint8_t for enum reflected");
		static const PC_CORE::ReflectedType& type = PC_CORE::Reflector::GetType<T>();
		const PC_CORE::ReflectedEnum& Renum = std::get<PC_CORE::ReflectedEnum>(type.metaData.data);
		const uint8_t underlying = std::to_underlying(*_EnumValue);

		if (ImGui::BeginMenu(_MenuName.data()))
		{
			for (const auto& EnumV : Renum.members)
			{
				ImGui::PushID(EnumV.name.c_str());
				if (ImGui::MenuItem(EnumV.name.c_str()))
				{
					*_EnumValue = static_cast<T>(EnumV.value);
				}
				ImGui::PopID();
			}
			ImGui::EndMenu();
		}
	}
}
