#include <Inspector.hpp>

#include "Editor.hpp"
#include "ImguiHelper.h"

#include "Command/EditorCommandAddComponent.hpp"
#include "Command/EditorCommandRemoveComponent.hpp"
#include "Resources/ResourceManager.hpp"

using namespace PC_EDITOR_CORE;

static float maxf = std::numeric_limits<float>::max();
static float minf = -maxf;
static double maxd = std::numeric_limits<double>::max();
static double mind = -maxd;

Inspector::~Inspector()
{
}

void Inspector::Update()
{
    EditorWindow::Update();
    PERF_REGION_SCOPED;

    Show();
}

Inspector::Inspector(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    m_ComponentReflected = PC_CORE::ReflectorInstance().GetAllTypesFrom<PC_CORE::Component>();
    m_ResourceReflected = PC_CORE::ReflectorInstance().GetAllTypesFrom<PC_CORE::Resource>();

    m_SpecialType =
    {
        &PC_CORE::ReflectorInstance().GetType<bool>(),
        &PC_CORE::ReflectorInstance().GetType<char>(),
        &PC_CORE::ReflectorInstance().GetType<wchar_t>(),

        &PC_CORE::ReflectorInstance().GetType<int>(),
        &PC_CORE::ReflectorInstance().GetType<float>(),
        &PC_CORE::ReflectorInstance().GetType<double>(),

        &PC_CORE::ReflectorInstance().GetType<uint16_t>(),
        &PC_CORE::ReflectorInstance().GetType<uint32_t>(),
        &PC_CORE::ReflectorInstance().GetType<uint64_t>(),

        &PC_CORE::ReflectorInstance().GetType<std::string>(),
        &PC_CORE::ReflectorInstance().GetType<std::wstring>(),

        &PC_CORE::ReflectorInstance().GetType<Tbx::Vector2f>(),
        &PC_CORE::ReflectorInstance().GetType<Tbx::Vector3f>(),
        &PC_CORE::ReflectorInstance().GetType<Tbx::Vector4f>(),

        &PC_CORE::ReflectorInstance().GetType<Tbx::Vector2<double>>(),
        &PC_CORE::ReflectorInstance().GetType<Tbx::Vector3<double>>(),
        &PC_CORE::ReflectorInstance().GetType<Tbx::Vector4<double>>(),

        &PC_CORE::ReflectorInstance().GetType<Tbx::Quaternionf>(),
        &PC_CORE::ReflectorInstance().GetType<Tbx::Quaterniond>(),
        &PC_CORE::ReflectorInstance().GetType<PC_CORE::Rotation>(),
    };

    PC_CORE::Level& lvl = PC_CORE::App::Instance->World.level;

    PC_CORE::ReflectorInstance().GetPtrToTypeField<PC_CORE::Level, PC_CORE::EntityManager>(
        &lvl, "m_EntityManager", &m_EntityManagerPtr);
    PC_CORE::ReflectorInstance().GetPtrToTypeField<PC_CORE::Level, PC_CORE::ComponentManager>(
        &lvl, "m_ComponentManager", &m_ComponentManagerPtr);
    PC_CORE::ReflectorInstance().GetPtrToTypeField<PC_CORE::ComponentManager, PC_CORE::ComponentArrayMap>(
        m_ComponentManagerPtr, "m_ComponentMapArray", &m_ComponentArrayMapPtr);
    PC_CORE::ReflectorInstance().GetPtrToTypeField<PC_CORE::ComponentManager, std::unordered_map<
                                              PC_CORE::ComponentTypeBit, PC_CORE::TypeId>>(
        m_ComponentManagerPtr, "m_ComponentBitFlagToComponentType", &m_ComponentTypeBitToTypeId);


    if (m_EntityManagerPtr == nullptr)
    {
        PC_LOGERROR("EntityManagerPtr == nullptr");
        return;
    }

    if (m_ComponentManagerPtr == nullptr)
    {
        PC_LOGERROR("ComponentManagerPtr == nullptr");
        return;
    }

    if (m_ComponentArrayMapPtr == nullptr)
    {
        PC_LOGERROR("ComponentArrayMapPtr == nullptr");
        return;
    }
}


void Inspector::Show()
{
    PERF_REGION_SCOPED;
    PC_CORE::World* w = PC_CORE::World::GetWorld();

    if (m_ComponentManagerPtr == nullptr)
    {
        PC_LOGERROR("ComponentManagerPtr == nullptr");
        return;
    }

    if (std::holds_alternative<std::monostate>(m_Editor->selectedObject))
        return;

    if (std::holds_alternative<PC_CORE::EntityId>(m_Editor->selectedObject))
    {
        ShowEntity(std::get<PC_CORE::EntityId>(m_Editor->selectedObject));
    }

    if (std::holds_alternative<PC_CORE::ObjectPtr<PC_CORE::Resource>>(m_Editor->selectedObject))
    {
        // TODO
    }
}

void Inspector::ShowEntity(PC_CORE::EntityId _id)
{
    if (_id == PC_CORE::INVALID_ENTITY_ID)
        return;

    const uint32_t ComponentCount = m_ComponentManagerPtr->GetComponentCount();
    std::string_view string = m_EntityManagerPtr->GetEntityName(_id);
    ImGui::PushID("EntityNameInput");
    ImGui::InputText("##EntityName", const_cast<char*>(string.data()), PC_CORE::MAX_ENTITY_NAME_LENGHT);
    ImGui::PopID();


    const auto& signature = m_EntityManagerPtr->GetSignature(_id);

    if (signature == nullptr)
        return;

    for (uint32_t i = 0; i < ComponentCount; ++i)
    {
        if (!signature->test(i))
            continue;

        PC_CORE::TypeId componentTypeId = m_ComponentTypeBitToTypeId->at(i);
        PC_CORE::ComponentArray* arr = &m_ComponentArrayMapPtr->at(componentTypeId);
        auto component = reinterpret_cast<PC_CORE::Component*>(&arr->Get(_id));

        const char* componentName = m_ComponentReflected[i]->name.c_str();
        ImGui::TextUnformatted(componentName);
        ImGui::Spacing();

        ImGui::PushID(static_cast<int>(componentTypeId));
        ShowReflectType(reinterpret_cast<uint8_t*>(component), *m_ComponentReflected[i]);
        ImGui::Spacing();


        if (ImGui::SmallButton("Delete Component"))
        {
            m_Editor->PushCommand<EditorCommandRemoveComponent>(_id, m_ComponentReflected[i]->typeId);
        }

        ImGui::PopID();
    }


    if (ImGui::ButtonCenteredOnLine("Add Component"))
    {
        ImGui::OpenPopup("Components");
    }


    ImGui::SameLine();
    if (ImGui::BeginPopup("Components"))
    {
        ImGui::SeparatorText("Component");
        for (auto& type : m_ComponentReflected)
        {
            if (ImGui::Selectable(type->name.c_str()))
            {
                if (PC_CORE::World::GetWorld() == nullptr)
                    continue;

                m_Editor->PushCommand<EditorCommandAddComponent>(_id, type->typeId);
            }
        }

        ImGui::EndPopup();
    }
}

void Inspector::ShowResource(PC_CORE::Resource* _resource)
{
    const PC_CORE::ReflectedType& type = _resource->GetType();
    ShowReflectType(reinterpret_cast<uint8_t*>(_resource), type);
}

void Inspector::ShowReflectType(uint8_t* _typePtr, const PC_CORE::ReflectedType& _reflectedType)
{
    // TO DO ADD MEMEBER TO CHIELD FORM BASE
    for (size_t j = 0; j < _reflectedType.metaData.members.size(); j++)
    {
        const PC_CORE::Members& m = _reflectedType.metaData.members[j];
        ImGui::PushID(static_cast<int>(_reflectedType.typeId + j));
        ShowMember(_typePtr + m.offset, m);
        ImGui::PopID();
        ImGui::Spacing();
    }
}

void Inspector::ShowMember(uint8_t* _memberPtr, const PC_CORE::Members& _member)
{
    const PC_CORE::ReflectedType& type = PC_CORE::ReflectorInstance().GetType(_member.typeKey);
    const uintmax_t typeFlag = type.typeFlags;

    if (IsShowable(type.typeId))
    {
        HandleShowAble(_memberPtr, type, _member);
        return;
    }
    std::visit([&](auto&& arg)
               {
                   using T = std::decay_t<decltype(arg)>;
                   if constexpr (std::is_same_v<T, PC_CORE::ReflectedWeakObjPtr>)
                   {
                       const PC_CORE::ReflectedWeakObjPtr& wkPtr = std::get<PC_CORE::ReflectedWeakObjPtr>(
                           type.metaData.data);

                       if (PC_CORE::ReflectorInstance().IsBaseOf<PC_CORE::Resource>(PC_CORE::ReflectorInstance().GetType(wkPtr.typeId)))
                       {
                           // only for little
                           uint64_t ptr = *reinterpret_cast<uint64_t*>(_memberPtr);
                           ImGui::Text("Address %lld", ptr);
                           HandlePtr(_memberPtr, type, _member);
                           return;
                       }
                   }

                   if (typeFlag & PC_CORE::ReflectedType::Composite)
                   {
                       for (auto& member : type.metaData.members)
                       {
                           ShowMember(_memberPtr + member.offset, member);
                       }
                   }
               },
               type.metaData.data);
}

bool Inspector::IsShowable(PC_CORE::TypeId type_id)
{
    auto it = std::ranges::find_if(m_SpecialType
                                   ,
                                   [type_id](const PC_CORE::ReflectedType* _reflectedType)
                                   {
                                       return _reflectedType->typeId == type_id;
                                   });

    return it != m_SpecialType.end();
}

void Inspector::HandleShowAble(uint8_t* _ptr, const PC_CORE::ReflectedType& _type, const PC_CORE::Members& _typeAsMember)
{
    const PC_CORE::TypeId id = _type.typeId;

    if (PC_CORE::ReflectorInstance().IsTypeIdIs<bool>(id))
    {
        auto value = reinterpret_cast<bool*>(_ptr);
        ImGui::Checkbox(_typeAsMember.membersName.c_str(), value);
    }
    if (PC_CORE::ReflectorInstance().IsTypeIdIs<wchar_t>(id))
    {
    }
    if (PC_CORE::ReflectorInstance().IsTypeIdIs<int>(id))
    {
        const auto value = reinterpret_cast<int*>(_ptr);
        ImGui::DragInt(_typeAsMember.membersName.c_str(), value);
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<float>(id))
    {
        const auto value = reinterpret_cast<float*>(_ptr);
        ImGui::DragFloat(_typeAsMember.membersName.c_str(), value);
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<double>(id))
    {
        const auto value = reinterpret_cast<double*>(_ptr);
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_Double, value, sizeof(float), 0.1, &mind,
                           &maxf);
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<uint16_t>(id))
    {
        
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<uint32_t>(id))
    {
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<uint64_t>(id))
    {
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<std::string>(id))
    {
        auto value = reinterpret_cast<std::string*>(_ptr);
        ImGui::InputText("##EntityName", value->data(), value->size());
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<std::wstring>(id))
    {
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<Tbx::Vector2f>(id))
    {
        auto vec = reinterpret_cast<Tbx::Vector2f*>(_ptr);
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_Float, vec->GetPtr(),
                           sizeof(Tbx::Vector2d) / sizeof(float), 0.1f, &minf, &maxf);
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<Tbx::Vector2d>(id))
    {
        auto vec = reinterpret_cast<Tbx::Vector2d*>(_ptr);
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_Double, vec->GetPtr(),
                           sizeof(Tbx::Vector2d) / sizeof(double), 0.1f, &mind, &maxd);
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<Tbx::Vector3f>(id))
    {
        auto vec = reinterpret_cast<Tbx::Vector3f*>(_ptr);
        if (_typeAsMember.memberFlag & PC_CORE::Members::EnumFlag::Color)
        {
            ImGui::ColorPicker3(_typeAsMember.membersName.c_str(), vec->GetPtr());
        }
        else
        {
            ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_Float, vec->GetPtr(),
                               sizeof(Tbx::Vector3f) / sizeof(float), 0.1f, &minf, &maxf);
        }
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<Tbx::Vector3d>(id))
    {
        auto vec = reinterpret_cast<Tbx::Vector3d*>(_ptr);
        // https://github.com/ocornut/imgui/issues/643
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_Double, vec->GetPtr(),
                           sizeof(Tbx::Vector3d) / sizeof(double), 0.1, &mind, &maxd);
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<Tbx::Vector4f>(id))
    {
        const auto vec = reinterpret_cast<Tbx::Vector4f*>(_ptr);
        if (_typeAsMember.memberFlag & PC_CORE::Members::EnumFlag::Color)
        {
            ImGui::ColorPicker4(_typeAsMember.membersName.c_str(), vec->GetPtr());
        }
        else
        {
            ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_Float, vec->GetPtr(),
                               sizeof(Tbx::Vector4f) / sizeof(float), 0.1f, &minf, &maxf);
        }
    }
    else if (PC_CORE::ReflectorInstance().IsTypeIdIs<Tbx::Vector4d>(id))
    {
        auto vec = reinterpret_cast<Tbx::Vector4d*>(_ptr);
        // https://github.com/ocornut/imgui/issues/643
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_Double, vec->GetPtr(),
                           sizeof(Tbx::Vector4d) / sizeof(double), 0.1f, &mind, &maxd);
    }
    else if (_type == PC_CORE::ReflectorInstance().GetType<Tbx::Quaternionf>())
    {
    }
    else if (_type == PC_CORE::ReflectorInstance().GetType<Tbx::Quaterniond>())
    {
    }
    else if (_type == PC_CORE::ReflectorInstance().GetType<PC_CORE::Rotation>())
    {
        PC_CORE::Rotation& rotation = *reinterpret_cast<PC_CORE::Rotation*>(_ptr);

        ImGui::DragScalarN("Rotation", ImGuiDataType_Float, rotation.EulerAngles.GetPtr(),
                           sizeof(Tbx::Vector3f) / sizeof(float), 0.1, &mind, &maxd);
        if (ImGui::IsItemEdited())
        {
            rotation.Quaternion = Tbx::Quaternionf::FromEuler(rotation.EulerAngles);
        }
    }
    else
    {
    }
}

void Inspector::HandlePtr(uint8_t* _ptr, const PC_CORE::ReflectedType& _type, const PC_CORE::Members& _typeAsMember)
{
    static bool showSelectResourceMenue = false;
    static std::string currentSelectedMember;

    if (showSelectResourceMenue == false)
    {
        currentSelectedMember.clear();
    }
    auto doublePtr = reinterpret_cast<PC_CORE::WeakObjectPtr<PC_CORE::Resource>*>(_ptr);

    ImGui::TextUnformatted(_typeAsMember.membersName.c_str());
    ImGui::PushID(_typeAsMember.membersName.c_str());


    if (!doublePtr->IsValid())
    {
        ImGui::Text("%s is null", _typeAsMember.membersName.c_str());
    }
    else
    {
        const auto* underLyingR = doublePtr->Lock().get();
        ImGui::TextUnformatted(underLyingR->Name.c_str());
    }

    if (ImGui::Button("Select"))
    {
        showSelectResourceMenue = true;
        currentSelectedMember = _typeAsMember.membersName;
    }
    ImGui::PopID();

    if (showSelectResourceMenue && currentSelectedMember == _typeAsMember.membersName)
    {
        ImGui::Begin("Select", &showSelectResourceMenue);

        auto l = [&](std::shared_ptr<PC_CORE::Resource> currentResource)
        {
            auto guiidS = static_cast<std::string>(currentResource->GetGuid());

            ImGui::PushID(guiidS.c_str());

            if (ImGui::Button(currentResource->Name.c_str()))
            {
                *doublePtr = currentResource;
                showSelectResourceMenue = false;
            }
            ImGui::PopID();
        };

        if (ImGui::Button("Set to Null"))
        {
            doublePtr->reset();
        }

   
        PC_CORE::ResourceManager::ForEach(std::get<PC_CORE::ReflectedWeakObjPtr>(_type.metaData.data).typeId, l);

        ImGui::End();
    }
}
