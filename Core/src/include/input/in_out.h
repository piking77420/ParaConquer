#pragma once

#include <string>

#include "core_header.hpp"

BEGIN_PCCORE

class InOut
{
public:
    PC_CORE_API static void PrintOut(const std::string& _string);

    PC_CORE_API static void PrintOut(const std::wstring& _string);

    PC_CORE_API static void PrintOut(std::string&& _string);
private:
    
};

END_PCCORE