#include "scripting/scripting_lua.hpp"
#include "assert.hpp"
#include "log.hpp"

#include "lua/lua.hpp"

using namespace PC_CORE;

lua_State* luaState = nullptr;
std::vector<std::string> ScriptingLua::m_Scripts;

#define CHECK_LUA(condition, message)\
	if(condition != LUA_OK)\
	{	PC_LOGERROR(message)\
	}\

ScriptingLua::ScriptingLua()
{
	assert(luaState == nullptr, "Lua state is alrealy created");
	
	luaState = luaL_newstate();
}

ScriptingLua::~ScriptingLua()
{
	assert(luaState != nullptr, "Lua Virtual Machnine is alrealy destroy");

	lua_close(luaState);
	luaState = nullptr;
}

void ScriptingLua::LuaLoadModule(const std::string& _filePath)
{
	const int returnValue = luaL_loadfile(luaState, _filePath.c_str());
	CHECK_LUA(returnValue, "Failed to load File")
	m_Scripts.push_back(_filePath);
}

void ScriptingLua::RemoveModule(const std::string& _filePath)
{
}

void ScriptingLua::RunFunction(const std::string& _funcName)
{
	for(std::string& s : m_Scripts)
	{
		lua_getglobal(luaState, _funcName.c_str());
		bool isFunc = lua_isfunction(luaState, -1);
		CHECK_LUA(isFunc, "In the file " + s + ", there is no function named " + _funcName);

		if (!isFunc) {
			lua_pop(luaState, 1); // Remove the non-function value from the stack
			continue;
		}

		int returnvalue = lua_pcall(luaState, 0, 0, 0);
		CHECK_LUA(returnvalue == LUA_OK, "Error running function " + _funcName);


		returnvalue = lua_pcall(luaState, 0, 0, 0);
	}
}
