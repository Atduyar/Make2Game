#include "LuaScript.h"

void LuaScript::DumpStack() {
	int top = lua_gettop(L);

	std::cout << "<Stack " << top << ">" << std::endl;

	for (int i = top; i >= 1; i--) {
		std::cout << "\t[Stack " << i << "] " << "\t" << luaL_typename(L, i);
		switch (lua_type(L, i)) {
		case LUA_TNUMBER:
			std::cout << lua_tonumber(L, i);
			break;
		case LUA_TSTRING:
			std::cout << lua_tostring(L, i);
			break;
		case LUA_TBOOLEAN:
			std::cout << (lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNIL:
			std::cout << "nil";
			break;
		default:
			std::cout << luaL_typename(L, i) << ": " << lua_topointer(L, i);
			break;
		}

		std::cout << std::endl;
	}
}

static int l_my_print(lua_State* L) {
	int nargs = lua_gettop(L);

	std::cout << "[Print] ";
	for (int i = 1; i <= nargs; i++) {

		switch (lua_type(L, i)) {
		case LUA_TNUMBER:
			std::cout << lua_tonumber(L, i);
			break;
		case LUA_TSTRING:
			std::cout << lua_tostring(L, i);
			break;
		case LUA_TBOOLEAN:
			std::cout << (lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNIL:
			std::cout << "nil";
			break;
		default:
			std::cout << luaL_typename(L, i) << ": " << lua_topointer(L, i);
			break;
		}
		std::cout << "\t";
	}

	std::cout << std::endl;

	return 0;
}

static const struct luaL_Reg printlib[] = {
  {"print", l_my_print},
  {NULL, NULL} /* end of array */
};



LuaScript::LuaScript(std::string filePath, bool bOpenlibs)
{
	// Create Lua State
	L = luaL_newstate();

	// Add standard libraries to Lua Virtual Machine
	if(bOpenlibs)
		luaL_openlibs(L);

	LuaSetFunction();
	//luaL_dostring(L, "Rstdout = io.stdout; io.stdout= io.open ('stdout.log', 'w')");

	// Load and parse the Lua File
	if (CheckLua(L, luaL_dofile(L, filePath.c_str())))
	{
		//lua_RedirectIO();
		//stdout

		
		
	}

}

LuaScript::~LuaScript()
{
	lua_close(L);
}

void LuaScript::Update()
{
	//Get Update Function
	lua_getglobal(L, "Update");
	if (lua_isfunction(L, -1))
	{
		if (CheckLua(L, lua_pcall(L, 0, 0, 0)));
	}
	else
	{
		std::cout << "Updata fucton not found" << std::endl;
	}
}

void LuaScript::LuaSetFunction()
{
	lua_getglobal(L, "_G");
	// luaL_register(L, NULL, printlib); // for Lua versions < 5.2
	luaL_setfuncs(L, printlib, 0);  // for Lua versions 5.2 or greater
	lua_pop(L, 1);
}

void LuaScript::ExecuteString(std::string commend)
{
	CheckLua(L, luaL_dostring(L, commend.c_str()));
}
