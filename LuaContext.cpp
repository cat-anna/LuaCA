/*
 * LuaApi.cpp
 *
 *  Created on: 03-08-2013
 *      Author: Paweu
 */

#include "Simulation.h"

typedef luabridge::Namespace ApiInitializer;
#include "SimApi.h"

namespace LuaApi {

int LuaContext::LoadAdditionalScript(lua_State *L) {
	int n = lua_gettop(L);
	if(n == 0) return 0;

	for(int i = 1; i <= n; ++i) {
		if(!lua_isstring(L, i)){
			cerr << "'include' instruction accepts only string parameters!\n";
			continue;
		}

		const char *c = lua_tostring(L, i);
		if(!DoLoadScript(L, c))
			cerr << "Unable to load script: " << c << "\n";
			//throw LuaPanic(std::string("Unable to load script: ") + c);
	}

	return 0;
}

int LuaContext::Lua_panic(lua_State *L) {
	const char *m = lua_tostring(L, 1);
	if (!m)
		throw LuaPanic("Unknown LUA error!");
	throw LuaPanic(m);
}

bool LuaContext::DoLoadScript(lua_State *L, const char* File) {
	cout << "Loading Script: '" << File << "' into Lua context\n";
	int ret = luaL_loadfile(L, File);
	switch (ret) {
		case LUA_ERRSYNTAX:
			std::cerr << "Script '" << File << "' has syntax error!\n";
			break;
		case LUA_ERRMEM:
			std::cerr << "Memory error while initializing Script\n";
			break;
	}

	if (ret != 0) {
		std::cerr << "Loading script failed. Error code: 0x" << std::hex << ret << "\n";
		return false;
	}

	lua_pcall(L, 0, 0, 0);
	return true;
}

bool LuaContext::DoLoadScriptCode(lua_State *L, const char* code) {
	cout << "Loading Script code: '" << code << "' into Lua context\n";
	int ret = luaL_loadstring(L, code);
	switch (ret) {
		case LUA_ERRSYNTAX:
			std::cerr << "code has syntax error!\n";
			break;
		case LUA_ERRMEM:
			std::cerr << "Memory error while initializing Script\n";
			break;
	}

	if (ret != 0) {
		std::cerr << "Loading script failed. Error code: 0x" << std::hex << ret << "\n";
		return false;
	}

	lua_pcall(L, 0, 0, 0);
	return true;
}

LuaContext::LuaContext(): m_Lua(0) {
}

LuaContext::~LuaContext() {
	if(m_Lua) {
		std::cout << "Destroying Lua context\n";
		lua_close(m_Lua);
		m_Lua = 0;
	}
}

bool LuaContext::InitContext() {
	std::cout << "Initializing Lua context\n";
	m_Lua = luaL_newstate();
	luaopen_base(m_Lua);
	luaopen_math(m_Lua);
#ifdef _LUAJIT_H
	luaopen_jit(m_Lua);
#endif
	lua_atpanic(m_Lua, LuaContext::Lua_panic);
	//luabridge::setHideMetatables(true);
	cout << "Initializing Lua api\n";
	SimApi::InitSimApi(luabridge::getGlobalNamespace(m_Lua));
	return true;
}

bool LuaContext::LoadScript(const char* Script) {
	return DoLoadScript(m_Lua, Script);
}

bool LuaContext::LoadScriptCode(const char* code) {
	return DoLoadScriptCode(m_Lua, code);
}

float randomf(float rmin, float rmax){
	float d = rmax - rmin;
	float r = (rand() % static_cast<int>(d)) + rmin;
	return r;
}

float LuaContext::GetMemUsage() const {
	return lua_gc(m_Lua, LUA_GCCOUNT, 0) * 1024 + lua_gc(m_Lua, LUA_GCCOUNTB, 0);
}

//-------------------------------------------------

TableDispatcher::TableDispatcher(lua_State *lua, int index): m_lua(lua), m_index(index) {
	if(!lua_istable(lua, m_index + 1)) {
		cerr << "TableDispatcher: argument is not a table!!\n";
	}
}

template<> float TableDispatcher::get(const char* name, float Default) {
	lua_pushstring(m_lua, name);
	lua_gettable(m_lua, m_index);
	float v;
	if(lua_isnil(m_lua, -1)) v = Default;
	else v = lua_tonumber(m_lua, -1);
	lua_pop(m_lua, 1);
	return v;
}

template<> double TableDispatcher::get(const char* name, double Default) {
	lua_pushstring(m_lua, name);
	lua_gettable(m_lua, m_index);
	double v;
	if(lua_isnil(m_lua, -1)) v = Default;
	else v = lua_tonumber(m_lua, -1);
	lua_pop(m_lua, 1);
	return v;
}

template<> int TableDispatcher::get(const char* name, int Default) {
	lua_pushstring(m_lua, name);
	lua_gettable(m_lua, m_index);
	int v;
	if(lua_isnil(m_lua, -1)) v = Default;
	else v = lua_tointeger(m_lua, -1);
	lua_pop(m_lua, 1);
	return v;
}

template<> const char* TableDispatcher::get(const char* name, const char* Default) {
	lua_pushstring(m_lua, name);
	lua_gettable(m_lua, m_index);
	const char* v;
	if(lua_isnil(m_lua, -1)) v = Default;
	else v = lua_tostring(m_lua, -1);
	lua_pop(m_lua, 1);
	return v;
}

template<> char TableDispatcher::get(const char* name, char Default) {
	lua_pushstring(m_lua, name);
	lua_gettable(m_lua, m_index);
	char v;
	if(lua_isnil(m_lua, -1)) v = Default;
	else v = lua_tointeger(m_lua, -1);
	lua_pop(m_lua, 1);
	return v;
}

template<> bool TableDispatcher::get(const char* name, bool Default) {
	lua_pushstring(m_lua, name);
	lua_gettable(m_lua, m_index);
	char v;
	if(lua_isnil(m_lua, -1)) v = Default;
	else v = lua_toboolean(m_lua, -1);
	lua_pop(m_lua, 1);
	return v;
}
} //namespace LuaApi

