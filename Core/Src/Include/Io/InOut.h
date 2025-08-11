#pragma once

#include <String>
#include <Vector>

#include "CoreHeader.hpp"

BEGIN_PCCORE

class InOut
{
public:
    PC_CORE_API static void PrintOut(const std::string& _string);

    PC_CORE_API static void PrintOut(const std::wstring& _string);

    PC_CORE_API static void PrintOut(std::string&& _string);

    PC_CORE_API static std::vector<char> ReadFile(const std::string& _filename);
private:
    
};

END_PCCORE