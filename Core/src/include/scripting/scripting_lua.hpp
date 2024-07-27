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

	static void LuaLoadModule(const std::string& _filePath);

	static void RemoveModule(const std::string& _filePath);

	static void RunFunction(const std::string& _funcName);

private:
	static std::vector<std::string> m_Scripts;
};

END_PCCORE