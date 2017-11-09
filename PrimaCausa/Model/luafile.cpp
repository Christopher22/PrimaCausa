#include "luafile.h"

#include <map>
#include <algorithm>
#include <cassert>
#include <fstream>

namespace Model {

	LuaFile::LuaFile(lua_State* state, std::vector<char>* code) : m_pLua(state), m_pBuffer(code) {
		assert(state);
		assert(code);
	}

	LuaFile::~LuaFile() {
		lua_close(m_pLua);

		delete m_pBuffer;
		m_pBuffer = nullptr;
	}

	LuaFile* LuaFile::LoadFile(const Path path, const int results) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		const std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char>* buffer = new std::vector<char>(unsigned int(size));
		if ( !file.read(buffer->data(), size) ) {
			delete buffer;
			return nullptr;
		}

		lua_State* tmp = luaL_newstate();
		luaL_openlibs(tmp);
		if ( luaL_loadbuffer(tmp, buffer->data(), buffer->size(), nullptr) || lua_pcall(tmp, 0, results, 0) ) {
			lua_close(tmp);
			return nullptr;
		}

		return new LuaFile(tmp, buffer);
	}

	LuaFile* LuaFile::LoadString(const char* string, const int results) {
		lua_State* tmp = luaL_newstate();
		luaL_openlibs(tmp);
		if ( luaL_loadstring(tmp, string) || lua_pcall(tmp, 0, results, 0) ) {
			lua_close(tmp);
			return nullptr;
		}

		std::vector<char>* vec = new std::vector<char>(string, string + strlen(string));
		return new LuaFile(tmp, vec);
	}

	int LuaFile::moveToLevel(const IdentifierPath path, const bool ignoreLast, const bool createIfNotExisting) {
		int currentLevel = 0;
		std::for_each(path.begin(), ( !ignoreLast ? path.end() : path.end() - 1 ), [&] (Identifier part) {
			if ( currentLevel == -1 ) {
				return;
			} else if ( currentLevel == 0 ) {
				lua_getglobal(m_pLua, part);
			} else {
				lua_getfield(m_pLua, -1, part);
			}

			if ( lua_isnil(m_pLua, -1) ) {
				if ( !createIfNotExisting ) {
					lua_pop(m_pLua, currentLevel);
					currentLevel = -1;
				} else {
					lua_pop(m_pLua, 1); // Remove nil
					if ( currentLevel == 0 ) {
						lua_newtable(m_pLua);
						lua_setglobal(m_pLua, part);
						lua_getglobal(m_pLua, part);
					} else {
						lua_pushstring(m_pLua, part);
						lua_newtable(m_pLua);
						lua_settable(m_pLua, -3);
						lua_getfield(m_pLua, -1, part);
					}

					currentLevel++;
				}
			} else {
				currentLevel++;
			}
		});
		return currentLevel;
	}

	void LuaFile::UnregisterFunction(const Identifier path) {
		Set(path, Nil());
	}

	void LuaFile::UnregisterFunction(const IdentifierPath path) {
		Set(path, Nil());
	}

	bool LuaFile::IncludeIn(lua_State* target) {
		return ( luaL_loadbuffer(target, m_pBuffer->data(), m_pBuffer->size(), nullptr) == 0 );
	}
}