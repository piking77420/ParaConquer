#pragma once
#include "editor_header.hpp"
#include "editor_window.hpp"
#include "core_header.hpp"
#include "resources/resource.hpp"

BEGIN_EDITOR_PCCORE
    class AssetBrowser : public EditorWindow
{
public:
    AssetBrowser(Editor& _editor, const std::string& _name);
    
    ~AssetBrowser() override = default  ;
    
    void Update() override;

private:
    void RenderBrowser();

    fs::path m_BasePath;

    fs::path m_CurrenPath;
};

END_EDITOR_PCCORE