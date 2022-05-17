#pragma once
#include <lua.hpp>
#include <iostream>
#include <string>

// Little error checking utility function
static bool CheckLua(lua_State* L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << "[LUA] Error: " << errormsg << std::endl;
		return false;
	}
	return true;
}

class LuaScript
{
	lua_State* L;
public:
	LuaScript(std::string filePath, bool bOpenlibs = true);
	~LuaScript();

	void DumpStack();
	void ExecuteString(std::string commend);

	void Update();
private:
	void LuaSetFunction();
};
