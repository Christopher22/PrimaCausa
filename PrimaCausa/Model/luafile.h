#pragma once

#include "types.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <cassert>
#include <map>
#include <vector>

namespace Model {

	class LuaFile {
	public:
		using Identifier = const char*;
		using IdentifierPath = std::vector<Identifier>;

		class Nil {};

		~LuaFile();
		static LuaFile* LoadFile(const Path path, const int results = 0);
		static LuaFile* LoadString(const char* string, const int results = 0);

		void UnregisterFunction(const Identifier path);
		void UnregisterFunction(const IdentifierPath path);

		bool IncludeIn(lua_State* target);

		template<class Value>
		bool Get(const IdentifierPath path, Value& value) {
			const int stackSize = lua_gettop(m_pLua);
			if (moveToLevel(path, false) == -1) {
				return false;
			}

			const bool result = readCurrentValue(value, -1);
			lua_pop(m_pLua, lua_gettop(m_pLua) - stackSize);

			assert(stackSize == lua_gettop(m_pLua));
			return result;
		}

		template<class Key, class Value>
		bool Get(const IdentifierPath path, std::map<Key, Value>& value) {
			const int stackSize = lua_gettop(m_pLua);
			if (moveToLevel(path, false) == -1) {
				return false;
			}

			const bool result = readTable<Key, Value>(value);
			lua_pop(m_pLua, lua_gettop(m_pLua) - stackSize);

			assert(stackSize == lua_gettop(m_pLua));
			return result;
		}

		template<class Value>
		bool Set(const Identifier path, Value& value) {
			const bool result = push<Value>(value);
			if (result) {
				lua_setglobal(m_pLua, path);
			}
			return result;
		}

		template<class Value>
		bool Set(const IdentifierPath path, const Value value) {
			assert(path.size() >= 2);

			const int stackSize = lua_gettop(m_pLua);
			moveToLevel(path, true, true);

			const bool result = push<Value>(value);
			if (result) {
				lua_setfield(m_pLua, -2, path.back());
			}

			lua_pop(m_pLua, lua_gettop(m_pLua) - stackSize);
			assert(stackSize == lua_gettop(m_pLua));
			return result;
		}

		template<class Userdata>
		void RegisterFunction(const Identifier path, lua_CFunction function, Userdata* data) {
			lua_pushlightuserdata(m_pLua, data);
			lua_pushcclosure(m_pLua, function, 1);
			lua_setglobal(m_pLua, path);
		}

		template<class Userdata>
		void RegisterFunction(const IdentifierPath path, lua_CFunction function, Userdata* data) {
			assert(path.size() >= 2);

			const int stackSize = lua_gettop(m_pLua);
			moveToLevel(path, true, true);

			lua_pushlightuserdata(m_pLua, data);
			lua_pushcclosure(m_pLua, function, 1);
			lua_setfield(m_pLua, -2, path.back());
			
			lua_pop(m_pLua, lua_gettop(m_pLua) - stackSize);
			assert(stackSize == lua_gettop(m_pLua));
		}

		template<class ParameterType, class ReturnType, class ErrorHandler>
		bool CallFunction(const IdentifierPath path, const ParameterType* arguments, ReturnType* returnValue, const ErrorHandler luaErrorHandler) {
			const int stackSize = lua_gettop(m_pLua);
			if (moveToLevel(path, false) == -1) {
				return false;
			}

			int arraySize = 0;
			if (arguments != nullptr) {
				arraySize = arguments->size();
				for (int i = 0; i < arraySize; i++) {
					push<ParameterType::value_type>(arguments->at(i));
				}
			}

			bool result = lua_pcall(m_pLua, arraySize, (returnValue ? 1 : 0), 0) == 0;
			if (!result) {
				luaErrorHandler(lua_tostring(m_pLua, -1));
			} else if (returnValue) {
				result = readCurrentValue<ReturnType>(*returnValue, -1);
			}

			lua_pop(m_pLua, lua_gettop(m_pLua) - stackSize);

			assert(stackSize == lua_gettop(m_pLua));
			return result;
		}

		template<class MapKey, class MapValue>
		static std::map<MapKey, MapValue> LoadTable(const Path path) {
			std::map<MapKey, MapValue> result;
			LuaFile* file = LuaFile::LoadFile(path, 1);
			if (file) {
				file->readTable(result);
				delete file;
				file = nullptr;
			}
			return result;
		}

		const std::vector<char>& GetCode() const {
			return *m_pBuffer;
		}

	private:
		LuaFile(lua_State* state, std::vector<char>* code);
		LuaFile(LuaFile& copy);

		int moveToLevel(const IdentifierPath path, const bool ignoreLast, const bool createIfNotExisting = false);

		template<typename Value> bool push(const Value value) {
			return false;
		}

		template<> bool push(const Empty value) {
			return true;
		}

		template<> bool push(const Nil value) {
			lua_pushnil(m_pLua);
			return true;
		}

		template<> bool push(const bool value) {
			lua_pushboolean(m_pLua, (value ? 1 : 0));
			return true;
		}

		template<> bool push(const int value) {
			lua_pushnumber(m_pLua, value);
			return true;
		}

		template<> bool push(const std::string& value) {
			lua_pushstring(m_pLua, value.c_str());
			return true;
		}

		template<class Value>
		bool readCurrentValue(Value& value, const int index) {
			return false;
		}

		template<> bool readCurrentValue(Empty& value, const int index) {
			return true;
		}

		template<> bool readCurrentValue(bool& value, const int index) {
			return lua_toboolean(m_pLua, index) != 0;
		}

		template<> bool readCurrentValue(int& value, const int index) {
			if (!lua_isnumber(m_pLua, index)) {
				return false;
			}
			value = (int)lua_tonumber(m_pLua, index);
			return true;
		}

		template<> bool readCurrentValue(std::string& value, const int index) {
			if (!lua_isstring(m_pLua, index)) {
				return false;
			}
			value = std::string(lua_tostring(m_pLua, index));
			return true;
		}

		template<class MapKey, class MapValue> bool readTable(std::map<MapKey, MapValue>& value) {
			lua_pushnil(m_pLua);

			MapKey currentKey;
			MapValue currentValue;
			while (lua_next(m_pLua, -2) != 0) {
				if (readCurrentValue(currentKey, -2) && readCurrentValue(currentValue, -1)) {
					value.insert(std::pair<MapKey, MapValue>(currentKey, currentValue));
				}
				lua_pop(m_pLua, 1);
			}
			return true;
		}

		lua_State* m_pLua;
		std::vector<char>* m_pBuffer;
	};
}