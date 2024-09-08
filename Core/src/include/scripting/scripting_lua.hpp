#pragma once
#include <vector>
#include <string>

#include "core_header.hpp"

BEGIN_PCCORE

class ScriptingLua
{
public:
	ScriptingLua();

	~ScriptingLua();

	PC_CORE_API static void LuaLoadModule(const std::string& _filePath);

	PC_CORE_API static void RemoveModule(const std::string& _filePath);

	PC_CORE_API static void RunFunction(const std::string& _funcName);

private:
	PC_CORE_API static std::vector<std::string> m_Scripts;
};

END_PCCORE