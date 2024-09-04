#pragma once

#include <string>

#include "core_header.hpp"

BEGIN_PCCORE

class InOut
{
public:
    static void PrintOut(const std::string& _string);

    static void PrintOut(const std::wstring& _string);

    static void PrintOut(std::string&& _string);
private:
    
};

END_PCCORE