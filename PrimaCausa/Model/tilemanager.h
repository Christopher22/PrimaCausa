#pragma once

#include "types.h"
#include "tile.h"
#include "luafile.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <map>
#include <string>
#include <map>

namespace Model {
	template<typename Renderable>
	class TileManager
	{
	public:
		TileManager() : m_pTiles(new Tile<Renderable>*[std::numeric_limits<TileType>::max() + 1]) {
			std::fill(m_pTiles, m_pTiles + std::numeric_limits<TileType>::max() + 1, nullptr);
		}

		~TileManager() {
			if (m_pTiles) {
				ForEach([](Tile<Renderable>* tile) {
					delete tile;
					tile = nullptr;
				});

				delete[] m_pTiles;
				m_pTiles = nullptr;
			}
		}

		template<typename Functor>
		void ForEach(const Functor functor) {
			std::for_each(m_pTiles, m_pTiles + std::numeric_limits<TileType>::max() + 1, [&](Tile<Renderable>* tile) {
				if (tile) {
					functor(tile);
				}
			});
		}

		void Process(const float deltaTime) {
			ForEach([&](Tile<Renderable>* tile) {
				tile->Process(deltaTime);
			});
		}

		template<typename CustomData>
		bool Load(const RenderableLoader<Renderable, CustomData> loader, const Path path, CustomData customData) {
			std::map<int, std::string> tiles = LuaFile::LoadTable<int, std::string>(path);
			if (tiles.size() == 0) {
				return false;
			}

			for (auto tileName : tiles) {
				Tile<Renderable>* tile = Tile<Renderable>::LoadFromFile(loader, tileName.second, customData);
				if (tile) {
					const auto type = static_cast<TileType>(tileName.first);
					this->m_pTiles[type] = tile;
					this->m_names[tile->GetName()] = type;
					tile->GetLua()->RegisterFunction<TileManager<Renderable>>({ "Tiles", "load" }, &TileManager<Renderable>::loadModule, this);
				} else {
					ForEach([](Tile<Renderable>* tile) {
						delete tile;
						tile = nullptr;
					});

					delete[] m_pTiles;
					m_pTiles = nullptr;
					return false;
				}
			}

			return true;
		}

		inline Tile<Renderable>* operator[] (const TileType id) const {
			return m_pTiles[id];
		}

		inline Tile<Renderable>* operator[] (const std::string& name) const {
			auto tmp = m_names.find(name);
			return (tmp != m_names.end() ? m_pTiles[tmp->second] : nullptr);
		}

	private:
		TileManager(const TileManager& copy);

		static int loadModule(lua_State* l) {
			const char* name = luaL_checkstring(l, 1);
			TileManager<Renderable>* manager = ((TileManager<Renderable>*)lua_touserdata(l, lua_upvalueindex(1)));
			Tile<Renderable>* tile = manager->operator [](name);
		
			return (tile != nullptr && tile->GetLua()->IncludeIn(l)) ? 1 : 0;
		}

		Tile<Renderable>** m_pTiles;
		std::map<std::string, TileType> m_names;
	};
}