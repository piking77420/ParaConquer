#include "input/in_out.h"

#include <iostream>

void PC_CORE::InOut::PrintOut(const std::string& _string)
{
    std::cout << _string;
}

void PC_CORE::InOut::PrintOut(const std::wstring& _string)
{
    std::wcout << _string;
}

void PC_CORE::InOut::PrintOut(std::string&& _string)
{
    std::cout << _string;
}
