/*
 * LuaApi.h
 *
 *  Created on: 30-07-2013
 *      Author: Paweu
 */
#ifndef LUAAPI_H_
#define LUAAPI_H_

#include <LuaJIT/lua.hpp>
#include <LuaBridge/LuaBridge.h>

namespace LuaApi {

class LuaPanic: public std::runtime_error {
public:
	LuaPanic(const std::string &msg): std::runtime_error(msg) { }
};

struct LuaContext {
	LuaContext();
	~LuaContext();

	template<class ... Types>
	int RunFunction(const char *Function, Types ... args) {
		try {
	//		std::cerr << "Call to: '" << Function << "'\n";
			luabridge::LuaRef fun = luabridge::getGlobal(m_Lua, Function);
			luabridge::LuaRef ret = fun(args...);
			return ret.cast<int>();
		} catch (const std::exception & e) {
			std::cerr << "Runtime script error! Function '" << Function << "', message: '" << e.what() << "'\n";
		} catch (...) {
			std::cerr << "Runtime script error! Function '" << Function << "' failed with unknown message!\n";
		}
		return -1;
	}

	void GetFunction(const char *Function) {
		lua_getglobal(m_Lua, Function);
	}

	struct helper {
		template <class T, class ... Params>
		static void unpack(lua_State *l, T t, Params ...p) {
			luabridge::Stack<T>::push(l, t);
			unpack(l, p...);
		}

		static void unpack(lua_State *) {}
	};

	template<class ... Types>
	void Call(Types ... args) {
		try {
//			luabridge::Stack<Types...>::push(m_Lua, args...);//<Types...>
			helper::unpack(m_Lua, args...);
			lua_pcall(m_Lua, sizeof...(Types), 1, 0);
		} catch (const std::exception & e) {
			std::cerr << "Runtime script error! message: '" << e.what() << "'\n";
		} catch (...) {
			std::cerr << "Runtime script error! Failed with unknown message!\n";
		}
	}

	template<class ... Types>
	void ExecuteFunction(const char *Function, Types ... args) {
		try {
	//		std::cerr << "Call to: '" << Function << "'\n";
			luabridge::getGlobal(m_Lua, Function)(args...);
		} catch (const std::exception & e) {
			std::cerr << "Runtime script error! Function '" << Function << "', message: '" << e.what() << "'\n";
		} catch (...) {
			std::cerr << "Runtime script error! Function '" << Function << "' failed with unknown message!\n";
		}
	}

	lua_State* GetLuaState() const { return m_Lua; }

	bool InitContext();
	bool LoadScript(const char* Script);
	bool LoadScriptCode(const char* code);

	float GetMemUsage() const;

	static int LoadAdditionalScript(lua_State *L);
private:
	lua_State *m_Lua;

	static int Lua_panic(lua_State *L);
	static bool DoLoadScript(lua_State *L, const char* File);
	static bool DoLoadScriptCode(lua_State *L, const char* code);
};

typedef std::shared_ptr<LuaContext> SharedLuaContext;

struct TableDispatcher {
	TableDispatcher(lua_State *lua, int index);
	float getFloat(const char* name, float Default = 0.0f);
	int getInt(const char* name, int Default = 0);
	const char* getString(const char* name, const char* Default = "");
	bool getBoolean(const char* name, bool Default = true);

	template <class T, class R = T> R get(const char*, T Default = T(0));
private:
	lua_State *m_lua;
	int m_index;
};

template<> double TableDispatcher::get(const char* name, double Default);
template<> float TableDispatcher::get(const char* name, float Default);
template<> int TableDispatcher::get(const char* name, int Default);
template<> const char* TableDispatcher::get(const char* name, const char* Default);
template<> bool TableDispatcher::get(const char* name, bool Default);

inline float TableDispatcher::getFloat(const char* name, float Default) { return get<>(name, Default); }
inline int TableDispatcher::getInt(const char* name, int Default) { return get<>(name, Default); }
inline const char* TableDispatcher::getString(const char* name, const char* Default) { return get<const char*>(name, Default); }
inline bool TableDispatcher::getBoolean(const char* name, bool Default) { return get<bool>(name, Default); }

template <class T, void(T::*FunPtr)(TableDispatcher&)>
struct TableCallDispatcher {
	typedef TableCallDispatcher<T, FunPtr> ThisClass;
	typedef int(T::*Tfun)(lua_State *);
	static Tfun GetInstance() {
		return (Tfun)&ThisClass::DispatchCall;
	}
private:
	TableCallDispatcher() = delete;
	~TableCallDispatcher();
	TableCallDispatcher& operator=(const TableCallDispatcher&) = delete;
	int DispatchCall(lua_State *l) {
		TableDispatcher t(l, -2);
		T* ptr = (T*)this;
		(ptr->*FunPtr)(t);
		return 0;
	}
};

template <class T, class Ret, Ret(T::*FunPtr)(TableDispatcher&)>
struct TableCallDispatcherEx {
	typedef TableCallDispatcherEx<T, Ret, FunPtr> ThisClass;
	typedef int(T::*Tfun)(lua_State *);

	static Tfun GetInstance() {
		return (Tfun)&ThisClass::DispatchCall;
	}
private:
	TableCallDispatcherEx() = delete;
	~TableCallDispatcherEx();
	TableCallDispatcherEx& operator=(const TableCallDispatcherEx&) = delete;
	int DispatchCall(lua_State *l) {
		TableDispatcher t(l, -2);
		T* ptr = (T*)this;
		Ret r = (ptr->*FunPtr)(t);
		luabridge::Stack<Ret>::push(l, r);
		return 1;
	}
};

};

#endif
