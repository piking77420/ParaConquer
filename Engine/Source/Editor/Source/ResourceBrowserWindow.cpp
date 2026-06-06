#include <backends/imgui_impl_vulkan.h>

#include "ResourceBrowserWindow.hpp"

#include "Editor.hpp"
#include "EditorFiles.hpp"
#include "LowRenderer/Rhi.hpp"
#include "Serialize/JsonSerializer.hpp"
#include "SystemDialogue.hpp"
#include "VulkanSampler.hpp"
#include "World/World.hpp"

#include "Resources/StaticMesh.hpp"

#include <Io/FileLoader.hpp>
#include <Fstream>
#include <ImguiHelper.h>
#include <Serialize/JsonSerializer.hpp>

using namespace PC_EDITOR_CORE;


std::string timeToString(std::time_t t)
{
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::wstring normalizePath(const std::wstring& path)
{
    std::wstring result = path;
    for (auto& ch : result)
    {
        if (ch == L'/') ch = L'\\';
    }
    return result;
}

void CreateTextureFromImage(PC_CORE::Rhi& rhi, const std::string& name, PC_CORE::Texture2D& texture, const PC_CORE::Image& image)
{
    using namespace PC_CORE;

    const auto& ImageLevel = image.GetMipDescriptor();

    texture = PC_CORE::Texture2D(rhi, name);
    texture
        ->SetWidth(ImageLevel[0].width)
        .SetHeight(ImageLevel[0].height)
        .SetMemoryUsage(RhiResource::MemoryUsage::StaticGPU)
        .SetTextureUsage(RhiTexture::TextureUsageFlagBits::Sampled | RhiTexture::TextureUsageFlagBits::TransferDst)
        .SetRhiFormat(RhiFormat::R8G8B8A8Unorm)
        .Build();

    const PC_CORE::RhiTexture::LevelUploadOperation op = {
        .Width = ImageLevel[0].width,
        .Height = ImageLevel[0].height,
        .Offset = 0u,
        .Size = ImageLevel[0].size,
    };

    rhi.GetRhiContext().
        ResourceUpdateBranch()
        ->TextureUpload2D(*texture.Get(), image.GetData(), { op }, RhiResourceState::PixelShaderResource);
}

ResourceBrowserWindow::ResourceBrowserWindow(Editor& _editor, const std::string& _name) : EditorWindow(_editor, _name)
{
    using namespace PC_CORE;

    m_BasePathRelative = std::filesystem::relative(std::filesystem::current_path(),
                                                   std::wstring(m_Editor->editorData.projectPath));
    m_CurrenPath = normalizePath(std::wstring(m_Editor->editorData.projectPath));
    windowFlags |= ImGuiWindowFlags_MenuBar;

    m_NearestSampler.reset(m_Editor->RenderHarwareInteface.CreateSampler());
    m_NearestSampler
        ->SetMagFilter(Filter::Linear)
        .SetMinFilter(Filter::Linear)
        .SetName("ImguiImageSampler")
        .Build();

    const PC_CORE::RhiSampler& s = *m_NearestSampler;

    Image imageFolder(EDITOR_RESOURCE_PATH "/Icons/Folder.png", RhiChannel::Rgba);
    Image imageNull(EDITOR_RESOURCE_PATH "/Icons/Null.png", RhiChannel::Rgba);

    CreateTextureFromImage(m_Editor->RenderHarwareInteface, "Folder.png", m_FolderIcon.texure, imageFolder);

    m_Editor->IMGUIContext.CreateImguiVulkanTexture(m_FolderIcon.texure.Get(),
        &s, &m_FolderIcon.descritproSet, 1);

    CreateTextureFromImage(m_Editor->RenderHarwareInteface, "Null.png", m_NullIcon.texure, imageNull);
    m_Editor->IMGUIContext.CreateImguiVulkanTexture(m_NullIcon.texure.Get(), &s,
                                                    &m_NullIcon.descritproSet, 1);

    CreateAssetsBrowserIcon(PC_CORE::Reflector::GetTypeKey<PC_CORE::Texture2D>(),
                            EDITOR_RESOURCE_PATH "/Icons/PngIcon.png");
    CreateAssetsBrowserIcon(PC_CORE::Reflector::GetTypeKey<PC_CORE::StaticMesh>(),
                            EDITOR_RESOURCE_PATH "/Icons/3DModel.png");

    const auto asserR = GetAssetRegisterPath();
    if (!asserR.empty())
    {
        PC_CORE::JsonSerializer s;
        if (s.OpenFile(asserR, PC_CORE::Serializer::SerializeOperation::DeSerialize))
        {
            s.DeSerialize<AssetRegistery>(&m_AssetRegistery);
            s.CloseFile();
        }
    }

    for (auto& it : m_AssetRegistery.pathToType)
    {
        if (std::filesystem::exists(it.first))
        {
            const auto lastTime = GetLastTimeModifyFile(it.first);
            if (it.second.lastTimeModified != lastTime)
                it.second.lastTimeModified = lastTime;

            if (!AssetFromFile(it.first))
            {
                PC_LOGERROR("Failed to import assets {}", it.first.generic_string());
                m_AssetRegistery.pathToType.erase(it.first);
            }
        }
    }
}

ResourceBrowserWindow::~ResourceBrowserWindow()
{
    
    m_Editor->IMGUIContext.DestroyVulkanTexture(&m_FolderIcon.descritproSet, 1);
    m_Editor->IMGUIContext.DestroyVulkanTexture(&m_NullIcon.descritproSet, 1);

    for (auto& it : m_TypeIconMap)
        m_Editor->IMGUIContext.DestroyVulkanTexture(&it.second.descritproSet, 1);

    const auto asserR = GetAssetRegisterPath();
    if (!asserR.empty())
    {
        PC_CORE::JsonSerializer s;
        s.OpenFile(asserR, PC_CORE::Serializer::SerializeOperation::Serialize);
        s.Serialize<AssetRegistery>(m_AssetRegistery);
        s.CloseFile();
    }
}


void ResourceBrowserWindow::Render()
{
    EditorWindow::Render();
}

void ResourceBrowserWindow::Update()
{
    PERF_REGION_SCOPED;
    EditorWindow::Update();


    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Option"))
        {
            ImGui::Text("Asset Browser Settings");
            ImGui::Separator();

            ImGui::DragFloat("Spacing", &m_AssetBrowserOption.spacing, 1.0f, MIN_MAX_FILE_SPACING.x,
                             MIN_MAX_FILE_SPACING.y, "%.1f px");
            ImGui::DragFloat("Padding", &m_AssetBrowserOption.padding, 1.0f, 10.0f, 40.0f, "%.1f px");
            ImGui::DragFloat("ThumbnailSize", &m_AssetBrowserOption.thumbnailSize, 1.0f, 10.0f, 100.0f, "%.1f px");

            ImGui::EndMenu();
        }

        if (ImGui::Button("Import"))
        {
            OnImportButton();
        }

        ImGui::EndMenuBar();
    }


    m_HasSelectedObject = false;

    ImGui::PushFont(m_Editor->editorData.editorFont.veryBig);
    ImGui::Text(m_BasePathRelative.generic_string().c_str());
    ImGui::PopFont();
    RenderDirectories();

    if (ImGui::IsWindowFocused() && IsCursorInsideWindow())
    {
        if (!m_HasSelectedObject && ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            CreateAsset();
        }

        if (!m_HasSelectedObject && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            m_Editor->selectedObject = std::monostate();
        }
    }
}


void ResourceBrowserWindow::CreateAsset() const
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup("createAssets");


    if (ImGui::BeginPopup("CreateAssets"))
    {
        ImGui::SeparatorText("Assets");

        //if (ImGui::Selectable("Textures"))

        ImGui::BeginPopup("Textures");
        if (ImGui::Selectable("Texture2D"))
        {
            /*
            const PC_CORE::SamplerCreateInfo info =
            {
                .SamplerName = "LinearRepeat",
                .magFilter = PC_CORE::Filter::Linear,
                .minFilter = PC_CORE::Filter::Linear,
                .u = PC_CORE::SamplerAddressMode::Repeat,
                .v = PC_CORE::SamplerAddressMode::Repeat,
                .w = PC_CORE::SamplerAddressMode::Repeat
            };*/

            //PC_CORE::Sampler newTexture(GetUniqueFileName(m_CurrenPath, "Texture2D", ".presource"));
        }


        ImGui::EndPopup();

        if (ImGui::Selectable("Level"))
        {
            //auto s = GetUniqueFileName(m_CurrenPath, "Level", std::string(PC_Level));
            //CreateFile((m_CurrenPath / s).string());
        }

        ImGui::EndPopup();
    }
}

bool ResourceBrowserWindow::AssetFromFile(const std::filesystem::path& _path)
{
    PERF_REGION_SCOPED;

    PC_CORE::JsonSerializer jSerializer;
    jSerializer.OpenFile(_path.generic_string(), PC_CORE::Serializer::SerializeOperation::DeSerialize);

    if (!jSerializer.IsOpen())
        return false;

    AssetHeader assetHeader;
    PC_CORE::ObjectPtr<PC_CORE::Resource> robjPtr;

    PC_CORE::Serializer::DeserializeEntry deserializeEntryHeader
    {
        PC_CORE::Reflector::GetTypeKey<AssetHeader>(),
        &assetHeader
    };
    PC_CORE::Serializer::DeserializeEntry deserializeObjPtr
    {
        PC_CORE::Reflector::GetTypeKey<PC_CORE::ObjectPtr<PC_CORE::Resource>>(),
        &robjPtr
    };

    jSerializer.DeSerializeEntries(deserializeEntryHeader, deserializeObjPtr);

    const bool typeExist = PC_CORE::Reflector::Exist(assetHeader.typeId);
    assert(typeExist);
    if (!typeExist)
    {
        PC_LOGERROR("Failed to extrute assets from file {}, invalid typeid {}", _path.generic_string(),
                    assetHeader.typeId);
        return false;
    }

    const auto& af = AssetFile(assetHeader.typeId, assetHeader.assetGuid, GetLastTimeModifyFile(_path));
    const auto& t = PC_CORE::Reflector::GetType(assetHeader.typeId);

    PC_LOG_VERBOSE("Cached AssetFile {}, type = {}, last time modified {}", _path.generic_string(), t.name,
                   timeToString(af.lastTimeModified));

    if (!PC_CORE::ResourceManager::Add(robjPtr))
    {
        jSerializer.CloseFile();
        return false;
    }

    jSerializer.CloseFile();
    m_AssetRegistery.pathToType.emplace(_path, af);
    return true;
}

void ResourceBrowserWindow::RenderDirectories()
{
    PERF_REGION_SCOPED;
    ImGui::PushFont(m_Editor->editorData.editorFont.normal);


    const float columnSpacing = m_AssetBrowserOption.spacing;
    const float padding = m_AssetBrowserOption.padding;
    const float thumbnailSize = m_AssetBrowserOption.thumbnailSize;

    float cellSize = thumbnailSize + padding * 2.0f + columnSpacing;

    float panelwidht = ImGui::GetContentRegionAvail().x;
    int colomnCount = static_cast<int>(panelwidht / cellSize);
    if (colomnCount < 1)
        colomnCount = 1;

    if (m_CurrenPath == m_Editor->editorData.projectPath)
    {
    }
    else if (ImGui::ArrowButton("Reverse", ImGuiDir_Left) && m_Editor->editorData.projectPath.string() != m_CurrenPath.
        string())
    {
        m_CurrenPath = m_CurrenPath.parent_path();
    }

    ImGui::Columns(colomnCount, nullptr, false);

    for (auto& entry : std::filesystem::directory_iterator(m_CurrenPath))
    {
        const auto& path = entry.path();
        std::filesystem::path pathname = path.filename();

        const std::string& name = pathname.generic_string();

        // Compute Text Size
        ImVec2 textSize = ImGui::CalcTextSize(name.c_str());
        float scale = thumbnailSize / textSize.x;

        if (entry.is_directory())
        {
            const ImguiImage& icon = m_FolderIcon;

            ImGui::Image(icon.descritproSet, {thumbnailSize, thumbnailSize}, {0, 0}, {1, 1});
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_CurrenPath = entry;
            }

            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
            ImGui::TextWrapped(name.c_str());
            ImGui::PopTextWrapPos();
            ImGui::NextColumn();
        }
        else if (pathname.has_extension() && pathname.extension() == AssetsFormat)
        {
            // if this asset is cached
            auto iterator = m_AssetRegistery.pathToType.find(path);
            PC_CORE::TypeId type = PC_CORE::NullTypeId;
            // Not cached 
            if (iterator == m_AssetRegistery.pathToType.end())
            {
                if (AssetFromFile(path))
                {
                    const auto& iterator2 = m_AssetRegistery.pathToType.at(path);
                    type = iterator2.typeId;
                }
            }
            else
            {
                type = iterator->second.typeId;
            }

            if (type != PC_CORE::NullTypeId && PC_CORE::Reflector::Exist(type))
            {
                
                auto it = m_TypeIconMap.find(type);
                auto& icon = it == m_TypeIconMap.end() ? m_NullIcon : it->second;

                if (ImGui::ImageButton(name.c_str(), icon.descritproSet, {thumbnailSize, thumbnailSize}))
                {
                    if (m_SelectedItem == entry)
                    {
                        // File clicked
                    }

                    if (m_SelectedItem.empty())
                    {
                        m_SelectedItem = entry;
                        PC_LOG("File selected: {}", name);
                    }
                }
                ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
                ImGui::TextWrapped(name.c_str());
                ImGui::PopTextWrapPos();
                ImGui::NextColumn();
            }
        }
    }

    ImGui::Columns(1);
    ImGui::PopFont();
}


std::string ResourceBrowserWindow::GetUniqueFileName(const std::filesystem::path& directory,
                                                     const std::string& baseName, const std::string& extension) const
{
    PERF_REGION_SCOPED;
    std::string fileName = baseName + extension;
    int counter = 1;

    while (std::filesystem::exists(directory / fileName))
    {
        fileName = baseName + "_" + std::to_string(counter) + extension;
        ++counter;
    }

    return fileName;
}


void ResourceBrowserWindow::CreateAssetsBrowserIcon(PC_CORE::TypeId _id, const std::filesystem::path& _path)
{
    PERF_REGION_SCOPED;

    PC_CORE::Image image(_path.generic_string(), PC_CORE::RhiChannel::Rgba);

    ImguiImage newIcon;

    CreateTextureFromImage(m_Editor->RenderHarwareInteface, _path.filename().generic_string(), newIcon.texure, image);


    m_Editor->IMGUIContext.CreateImguiVulkanTexture(
        newIcon.texure.Get(),
        m_NearestSampler.get(),
        &newIcon.descritproSet, 1);

    m_TypeIconMap[_id] = std::move(newIcon);
}

void ResourceBrowserWindow::OnImportButton()
{
    /*
    const std::wstring& assetPath = m_Editor->editorData.projectPath;
    const std::wstring s = SystemDialogue::Instance().SeletecFile(L"Select your project imported source",
                                                                  assetPath.c_str());

    if (s.empty())
        return;

    const std::filesystem::path p(s);
    if (!std::filesystem::exists(p))
        return;

    const auto pathToSerialize = p.parent_path() / (p.filename().stem().string() + AssetsFormat);


    PC_CORE::TypeId id = PC_CORE::NullTypeId;
    PC_CORE::ObjectPtr<PC_CORE::Resource> r;
    PC_CORE::JsonSerializer jSerializer;

    // Create The end asset file
    jSerializer.OpenFile(pathToSerialize.generic_string(), PC_CORE::Serializer::SerializeOperation::Serialize);
    if (!jSerializer.IsOpen())
        return;

    // Try to decode imported file
    // Pass json in order to give render data
    if (!m_Importer.Import(m_Editor->RenderHarwareInteface, p, &jSerializer, &id, &r) || id == PC_CORE::NullTypeId)
    {
        jSerializer.CloseFile();
        return;
    }

    // Serialize in the file
    AssetHeader assetHeader;
    assetHeader.typeId = r->GetTypeKey();
    assetHeader.assetGuid = r->GetGuid();

    jSerializer.Serialize(assetHeader, r);

    // Cache file 
    const auto it = m_AssetRegistery.pathToType.find(pathToSerialize);
    if (it == m_AssetRegistery.pathToType.end())
    {
        const auto& af = AssetFile(*r.get());
        const auto& t = PC_CORE::Reflector::GetType(af.typeId);
        PC_LOG_VERBOSE("Cached AssetFile {}, type = {}, last time modified {}", p.generic_string(), t.name,
                       timeToString(af.lastTimeModified));
        m_AssetRegistery.pathToType.emplace(pathToSerialize, af);
    }

    // Close file
    jSerializer.CloseFile();*/
}

void ResourceBrowserWindow::CreateFile(const std::string& _filename) const
{
    std::ofstream file(_filename);

    if (file.is_open())
    {
        file.close();
        PC_LOG("File created: {}", _filename);
    }
    else
    {
        PC_LOG("Failed to created: {}", _filename);
    }
}


std::string ResourceBrowserWindow::GetAssetRegisterPath() const
{
    const auto p = m_Editor->editorData.projectPath.empty()
                       ? std::string()
                       : m_Editor->editorData.projectPath.generic_string() + "/" + AssetsRegisteryFileName;

    return p;
}

std::time_t ResourceBrowserWindow::GetLastTimeModifyFile(const std::filesystem::path& _p) const
{
    auto ftime = std::filesystem::last_write_time(_p);

    auto sctp = std::chrono::clock_cast<std::chrono::system_clock>(ftime);

    return std::chrono::system_clock::to_time_t(sctp);
}
