#include "inspector.hpp"

#include "editor.hpp"
#include "imgui_helper.h"
#include <ImguiNodeEditor/imgui_node_editor.h>

#include "resources/resource_manager.hpp"

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

    if (m_Editor->m_SelectedEntityId == PC_CORE::INVALID_ENTITY_ID)
        return;

    Show();
    OnInput();
}

Inspector::Inspector(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    m_ReflectedTypes = PC_CORE::Reflector::GetAllTypesFrom<PC_CORE::Component>();

    
    m_SpecialType =
        {
        &PC_CORE::Reflector::GetType<bool>(),
        &PC_CORE::Reflector::GetType<char>(),
        &PC_CORE::Reflector::GetType<wchar_t>(),
        
        &PC_CORE::Reflector::GetType<int>(),
        &PC_CORE::Reflector::GetType<float>(),
        &PC_CORE::Reflector::GetType<double>(),
        
        &PC_CORE::Reflector::GetType<uint16_t>(),
        &PC_CORE::Reflector::GetType<uint32_t>(),
        &PC_CORE::Reflector::GetType<uint64_t>(),
        
        &PC_CORE::Reflector::GetType<std::string>(),
        &PC_CORE::Reflector::GetType<std::wstring>(),
        
        &PC_CORE::Reflector::GetType<Tbx::Vector2f>(),
        &PC_CORE::Reflector::GetType<Tbx::Vector3f>(),
        &PC_CORE::Reflector::GetType<Tbx::Vector4f>(),
        
        &PC_CORE::Reflector::GetType<Tbx::Vector2<double>>(),
        &PC_CORE::Reflector::GetType<Tbx::Vector3<double>>(),
        &PC_CORE::Reflector::GetType<Tbx::Vector4<double>>(),
        
        &PC_CORE::Reflector::GetType<Tbx::Quaternionf>(),
        &PC_CORE::Reflector::GetType<Tbx::Quaterniond>(),
        &PC_CORE::Reflector::GetType<PC_CORE::Rotation>(),
        };

    PC_CORE::World* w = &PC_CORE::App::instance->world;
    
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::World, PC_CORE::EntityManager>(w, "m_EntityManager", &entityManagerPtr);
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::World, PC_CORE::ComponentManager>(w, "m_ComponentManager", &componentManagerPtr);
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::ComponentManager, PC_CORE::ComponentArrayMap>(componentManagerPtr, "m_ComponentMapArray", &componentArrayMapPtr);
    PC_CORE::Reflector::GetPtrToTypeField<PC_CORE::ComponentManager, std::unordered_map<PC_CORE::ComponentTypeBit, PC_CORE::TypeId >>(componentManagerPtr, "m_ComponentBitFlagToComponentType", &componentTypeBitToTypeId);
    

    if (entityManagerPtr == nullptr)
    {
        PC_LOGERROR("EntityManagerPtr == nullptr");
        return;
    }

    if (componentManagerPtr == nullptr)
    {
        PC_LOGERROR("ComponentManagerPtr == nullptr");
        return;
    }

    if (componentArrayMapPtr == nullptr)
    {
        PC_LOGERROR("ComponentArrayMapPtr == nullptr");
        return;
    }
}

void Inspector::Show()
{
    PC_CORE::World* w = PC_CORE::World::GetWorld();

    
    const uint32_t ComponentCount = componentManagerPtr->GetComponentCount();
    const PC_CORE::EntityId selectedId = m_Editor->m_SelectedEntityId;


    std::string_view string = entityManagerPtr->GetEntityName(selectedId);
    ImGui::PushID("EntityNameInput");
    ImGui::InputText("##EntityName", const_cast<char*>(string.data()), PC_CORE::MAX_ENTITY_NAME_LENGHT);
    ImGui::PopID();


    const auto& signature = entityManagerPtr->GetSignature(selectedId);

    for (int i = 0; i < ComponentCount; ++i)
    {
        if (!signature.test(i))
            continue;

        PC_CORE::TypeId componentTypeId = componentTypeBitToTypeId->at(i); 
        PC_CORE::ComponentArray* arr = &componentArrayMapPtr->at(componentTypeId);
        PC_CORE::Component* component = reinterpret_cast<PC_CORE::Component*>(&arr->GetData(selectedId));
        const auto& reflectedMember = PC_CORE::Reflector::GetType(componentTypeId);

        const char* componentName = m_ReflectedTypes[i]->name.c_str();
        ImGui::Text(componentName);
        ImGui::Spacing();

        ImGui::PushID(static_cast<int>(componentTypeId));
        ShowReflectType(reinterpret_cast<uint8_t*>(component), *m_ReflectedTypes[i]);
        ImGui::Spacing();

        

        if (ImGui::SmallButton("Delete Component"))
        {
            //componentManagerPtr->RemoveComponent(m_ReflectedTypes[i]->typeId, m_Editor->m_SelectedEntityId);
            //m_Editor->world.scene.RemoveComponent(_entity, _componentId);
        }

        ImGui::PopID();
    }
    /*

    for (size_t i = 0; i < m_ReflectedTypes.size(); i++)
    {
        const uint32_t currentKeyComponent = m_ReflectedTypes[i]->typeId;

        if (!entityManager.HasComponent(currentKeyComponent, selectedId))
            continue;

        PC_CORE::Component* component = static_cast<PC_CORE::Component*>(entityManager.GetComponent(
            currentKeyComponent, selectedId));
        auto reflectedMember = PC_CORE::Reflector::GetType(currentKeyComponent);

        const char* componentName = m_ReflectedTypes[i]->name.c_str();
        ImGui::Text(componentName);
        ImGui::Spacing();

        ImGui::PushID(static_cast<int>(currentKeyComponent));
        ShowReflectType(reinterpret_cast<uint8_t*>(component), *m_ReflectedTypes[i]);
        ImGui::Spacing();

        

        if (ImGui::SmallButton("Delete Component"))
        {
            //entityManager.RemoveComponent(m_ReflectedTypes[i]->typeId, m_Editor->m_SelectedEntityId);
            //m_Editor->world.scene.RemoveComponent(_entity, _componentId);
        }

        ImGui::PopID();
    }*/
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

                //PC_CORE::World::GetWorld()->entityManager.AddComponent(type->typeId, m_Editor->m_SelectedEntityId);
            }
        }

        ImGui::EndPopup();
    }
}



void Inspector::ShowReflectType(uint8_t* _typePtr ,const PC_CORE::ReflectedType& _reflectedType)
{
    // TO DO ADD MEMEBER TO CHIELD FORM BASE
    for (size_t j = 0; j < _reflectedType.metaData.members.size(); j++)
    {
        const PC_CORE::Members& m = _reflectedType.metaData.members[j];
        ImGui::PushID(static_cast<int>(_reflectedType.typeId + j));
        ShowMember(static_cast<uint8_t*>(_typePtr) + m.offset, m);
        ImGui::PopID();
        ImGui::Spacing();
    }
}

void Inspector::ShowMember(uint8_t* _memberPtr, const PC_CORE::Members& _member)
{
    const PC_CORE::ReflectedType& type = PC_CORE::Reflector::GetType(_member.typeKey);
    const uintmax_t typeFlag = type.typeFlags;
    const uintmax_t& memberFlag = _member.memberFlag;

    
    if (IsShowable(type.typeId))
    {
        HandleShowAble(_memberPtr, type, _member);
        return;
    }
    
    if (type.metaData.typeNatureMetaData.metaDataTypeEnum == PC_CORE::TypeNatureMetaDataEnum::WeakPtr 
        && PC_CORE::Reflector::IsBaseOf<PC_CORE::Resource>(PC_CORE::Reflector::GetType(type.metaData.typeNatureMetaData.metaDataType.weakPtr.type)))
    {
        // only for little
        uint64_t ptr = (uint64_t)(*reinterpret_cast<uint64_t*>(_memberPtr));
        ImGui::Text("Address %lld", ptr);
        HandlePtr(_memberPtr, type, _member);
    }
    if (typeFlag & PC_CORE::TypeFlagBits::COMPOSITE)
    {
        for (auto& member : type.metaData.members)
        {
            ShowMember(_memberPtr + member.offset, member);
        }
    }
    
}

bool Inspector::IsShowable(PC_CORE::TypeId type_id)
{
    auto it = std::find_if(
        m_SpecialType.begin(), m_SpecialType.end(), 
        [type_id](const PC_CORE::ReflectedType* reflectedType)
        {
            return reflectedType->typeId == type_id; // Assuming GetTypeId() returns TypeId
        });

    return it != m_SpecialType.end();
}

void Inspector::HandleShowAble(uint8_t* ptr, const PC_CORE::ReflectedType& type, const PC_CORE::Members& _typeAsMember)
{
    const PC_CORE::TypeId id = type.typeId;

    if (PC_CORE::Reflector::IsTypeIdIs<bool>(id))
    {
        bool* value = reinterpret_cast<bool*>(ptr);
        ImGui::Checkbox(_typeAsMember.membersName.c_str(), value);
    }
    if (PC_CORE::Reflector::IsTypeIdIs<wchar_t>(id))
    {
        
    }
    if (PC_CORE::Reflector::IsTypeIdIs<int>(id))
    {
        int* value = reinterpret_cast<int*>(ptr);
        ImGui::DragInt(_typeAsMember.membersName.c_str(), value);
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<float>(id))
    {
        float* value = reinterpret_cast<float*>(ptr);
        ImGui::DragFloat(_typeAsMember.membersName.c_str(), value);
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<double>(id))
    {
        double* value = reinterpret_cast<double*>(ptr);
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_::ImGuiDataType_Double, value, sizeof(float), 0.1, &mind, &maxf);
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<uint16_t>(id))
    {
        
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<uint32_t>(id))
    {
        
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<uint64_t>(id))
    {
        
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<std::string>(id))
    {
        std::string* value = reinterpret_cast<std::string*>(ptr);
        ImGui::InputText("##EntityName", value->data(), value->size());
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<std::wstring>(id))
    {
        
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<Tbx::Vector2f>(id))
    {
        Tbx::Vector2f* vec = reinterpret_cast<Tbx::Vector2f*>(ptr);
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_::ImGuiDataType_Double, vec->GetPtr(), sizeof(Tbx::Vector2d) / sizeof(float), 0.1, &minf, &maxf);
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<Tbx::Vector2d>(id))
    {
        Tbx::Vector2d* vec = reinterpret_cast<Tbx::Vector2d*>(ptr);
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_::ImGuiDataType_Double, vec->GetPtr(), sizeof(Tbx::Vector2d) / sizeof(double), 0.1, &mind, &maxd);
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<Tbx::Vector3f>(id))
    {
        Tbx::Vector3f* vec = reinterpret_cast<Tbx::Vector3f*>(ptr);
        if (_typeAsMember.memberFlag & PC_CORE::MemberEnumFlag::COLOR)
        {
            ImGui::ColorPicker3(_typeAsMember.membersName.c_str(), vec->GetPtr());
        }
        else
        {
            ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_::ImGuiDataType_Double, vec->GetPtr(), sizeof(Tbx::Vector3f) / sizeof(float), 0.1, &minf, &maxf);
        }
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<Tbx::Vector3d>(id))
    {
        Tbx::Vector3d* vec = reinterpret_cast<Tbx::Vector3d*>(ptr);
        // https://github.com/ocornut/imgui/issues/643
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_::ImGuiDataType_Double, vec->GetPtr(), sizeof(Tbx::Vector3d) / sizeof(double), 0.1, &mind, &maxd);
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<Tbx::Vector4f>(id))
    {
        Tbx::Vector4f* vec = reinterpret_cast<Tbx::Vector4f*>(ptr);
        if (_typeAsMember.memberFlag & PC_CORE::MemberEnumFlag::COLOR)
        {
            ImGui::ColorPicker4(_typeAsMember.membersName.c_str(), vec->GetPtr());
        }
        else
        {
            ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_::ImGuiDataType_Double, vec->GetPtr(), sizeof(Tbx::Vector4f) / sizeof(float), 0.1, &minf, &maxf);
        }
    }
    else if (PC_CORE::Reflector::IsTypeIdIs<Tbx::Vector4d>(id))
    {
        Tbx::Vector4d* vec = reinterpret_cast<Tbx::Vector4d*>(ptr);
        // https://github.com/ocornut/imgui/issues/643
        ImGui::DragScalarN(_typeAsMember.membersName.c_str(), ImGuiDataType_::ImGuiDataType_Double, vec->GetPtr(), sizeof(Tbx::Vector4d) / sizeof(double), 0.1, &mind, &maxd);
    }
    else if (type == PC_CORE::Reflector::GetType<Tbx::Quaternionf>())
    {
        
    }else if (type == PC_CORE::Reflector::GetType<Tbx::Quaterniond>())
    {
        
    }
    else if (type == PC_CORE::Reflector::GetType<PC_CORE::Rotation>())
    {
        PC_CORE::Rotation& rotation = *reinterpret_cast<PC_CORE::Rotation*>(ptr);

        ImGui::DragScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Double, rotation.eulerAngles.GetPtr(), sizeof(Tbx::Vector3d) / sizeof(double), 0.1 * M_PI_4, &mind, &maxd);
        if (ImGui::IsItemEdited())
        {
            rotation.quaternion = Tbx::Quaterniond::FromEuler(rotation.eulerAngles);
        }
    }
    else
    {
        
    }

    
}

void Inspector::HandlePtr(uint8_t* ptr, const PC_CORE::ReflectedType& type, const PC_CORE::Members& _typeAsMember)
{
    static bool showSelectResourceMenue = false;
    static std::string currentSelectedMember;

    if (showSelectResourceMenue == false)
    {
        currentSelectedMember.clear();
    }
    PC_CORE::ResourceRef<PC_CORE::Resource>* doublePtr = reinterpret_cast<PC_CORE::ResourceRef<PC_CORE::Resource>*>(ptr);

    ImGui::PushID(_typeAsMember.membersName.c_str());


    if (doublePtr->expired())
    {
        ImGui::Text("%s is null", _typeAsMember.membersName.c_str());
    }
    else
    {
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
                std::string guiidS = (std::string)currentResource->guid;

                ImGui::PushID(guiidS.c_str());

                if (ImGui::Button(currentResource->name.c_str()))
                {
                    *doublePtr = currentResource;
                    showSelectResourceMenue = false;
                }
                ImGui::PopID();
            };

        if (ImGui::Button("Set to Null"))
        {
            *doublePtr = {};
        }

        // remove the type of the pointer
        PC_CORE::ResourceManager::ForEach(type.metaData.typeNatureMetaData.metaDataType.weakPtr.type, l);

        ImGui::End();

    }
}