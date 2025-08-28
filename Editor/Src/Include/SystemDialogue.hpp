#pragma once

#include <string>

#include "EditorHeader.hpp"
#include "Singleton.hpp"

class SystemDialogue : public PC_CORE::Singleton<SystemDialogue>
{
public:

    [[nodiscard]] static std::wstring SeletecFolder(const wchar_t* _caption);


    [[nodiscard]] static std::wstring SeletecFile(const wchar_t* _caption);

    SystemDialogue();

    ~SystemDialogue();
    
private:

};
