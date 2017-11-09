#pragma once

#include "layer.h"
#include "tilemanager.h"
#include "luafile.h"
#include "Helper\binarycontainer.h"
#include "neighbours.h"

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cassert>

namespace Model {
	
	template<typename Renderable>
	class Map {
	public:
		typedef void(*NeightbourAlgorithm)(Coordinate pos, Index distance, std::vector<Coordinate>& neighbours);

		~Map() {
			m_pTileManager->ForEach([&](Tile<Renderable>* tile) {
				tile->GetLua()->UnregisterFunction({ "Map", "setTile" });
				tile->GetLua()->UnregisterFunction({ "Map", "getTile" });
				tile->GetLua()->UnregisterFunction({ "Map", "addNextTile" });
				tile->GetLua()->UnregisterFunction({ "Map", "loadNeightbours" });
				tile->GetLua()->UnregisterFunction({ "Map", "getNeightbour" });
			});

			std::for_each(m_layers.begin(), m_layers.end(), [](Layer<Renderable>* layer) {
				delete layer;
				layer = nullptr;
			});

			delete m_pTileManager;
			m_pTileManager = nullptr;

			delete m_pLua;
			m_pLua = nullptr;
		}

		static Map* Load(Path path, TileManager<Renderable>* tiles) {
			// Fill the data structures from the binary file.
			LuaFile* lua = nullptr;
			std::vector<Layer<Renderable>*> layers;
			std::map<std::string, std::vector<char>> data;
			bool isBackground = true;
			const bool success = Model::readContainer<char, unsigned short>(path, { 'P', 'C' }, [&](char id, std::ifstream& bytes, unsigned short size) {
				switch (id) {
				case ChunkType::MAP: {
					// Load map
					auto layer = Layer<Renderable>::Load(tiles, bytes, isBackground);
					if (!layer) {
						return false;
					} else if (isBackground) {
						isBackground = false;
					}
					layers.push_back(layer);
					break;
				}	
				case ChunkType::LUA: {
					// If lua is already read
					if (lua) {
						return false;
					}

					// Load lua
					char* buffer = new char[size];
					if (bytes.read(buffer, size)) {
						lua = LuaFile::LoadString(buffer, 0);
					}
					delete[] buffer;

					// Return false on failure
					if (!lua) {
						return false;
					}
					break;
				}
				case ChunkType::DATA: {
					// Load free data chunks
					char* buffer = new char[size];
					bytes.read(buffer, size);
					if (bytes) {
						std::string key(buffer); 

						const auto valueStart = buffer + key.size() + 1;
						std::vector<char> value(valueStart, valueStart + (size - key.size() - 1));

						data.emplace(key, value);
						delete[] buffer;
					} else {
						delete[] buffer;
						return false;
					}
					break;
				}
				default: return false;
				};
				return true;
			});
			
			if(!success || !lua || layers.size() < 1)  {
				// Delete LUA, if existing
				if (lua) {
					delete lua;
					lua = nullptr;
				}

				// Delete already constructed layers
				std::for_each(layers.begin(), layers.end(), [](Layer<Renderable>* layer) {
					delete layer;
					layer = nullptr;
				});
				return nullptr;
			}

			return new Map(layers, tiles, lua, data);
		}

		bool Save(Path path) const {
			Index layerIndex = 0;
			auto dataIterator = m_data.begin();

			return Model::writeContainer<char, unsigned short>(path, { 'P', 'C' }, [&](char& chunkType, std::stringstream& output) {
				if (layerIndex < m_layers.size()) {
					chunkType = char(ChunkType::MAP);
					m_layers[layerIndex]->Save(output);
				} else if (layerIndex == m_layers.size()) {
					chunkType = char(ChunkType::LUA);
					output.write(m_pLua->GetCode().data(), m_pLua->GetCode().size());
					output.put(0);
				} else if (dataIterator != m_data.end()) {
					chunkType = char(ChunkType::DATA);
					output.write(dataIterator->first.c_str(), dataIterator->first.size() + 1);
					output.write(dataIterator->second.data(), dataIterator->second.size());
					++dataIterator;
				} else{
					return false;
				}

				++layerIndex;
				return true;
			});
		}

		template<typename Value>
		const Value* GetValue(const std::string& id, Value* defaultValue = nullptr) {
			auto result = m_data.find(id);
			if (result != m_data.end()) {
				assert(sizeof(Value) == result->second.size());
				return static_cast<const Value*>(static_cast<const void*>(result->second.data()));
			} else if (defaultValue) {
				this->SetValue<Value>(id, *defaultValue);
				return defaultValue;
			} else {
				return nullptr;
			}
		}
		
		template<typename Value>
		void SetValue(const std::string& id, Value value) {
			const char* bytes = static_cast<const char*>(static_cast<void*>(&value));
			m_data[id] = std::vector<char>(bytes, bytes + sizeof(Value));
		}

		Index GetWidth() const {
			Index maxWidth = 0;
			std::for_each(m_layers.begin(), m_layers.end(), [&](Layer<Renderable>* layer) {
				if (maxWidth < layer->GetWidth()) {
					maxWidth = layer->GetWidth();
				}
			});
			return maxWidth;
		}

		Index GetHeight() const {
			Index maxHeight = 0;
			std::for_each(m_layers.begin(), m_layers.end(), [&](Layer<Renderable>* layer) {
				if (maxHeight < layer->GetHeight()) {
					maxHeight = layer->GetHeight();
				}
			});
			return maxHeight;
		}

		template<typename ErrorHandler>
		void NextTurn(ErrorHandler luaErrorHandler) {
			Index layerId = 0;
			std::for_each(m_layers.begin(), m_layers.end(), [&](Layer<Renderable>* layer) {
				if (!layer->IsBackground()) {
					layer->NextTurn(layerId, luaErrorHandler);
				}
				++layerId;
			});
		}

		template<typename CustomData>
		void Draw(Renderer<Renderable, CustomData> renderer, CustomData customData) const {
			Index layerId = 0;
			std::for_each(m_layers.begin(), m_layers.end(), [&](const Layer<Renderable>* layer) {
				layer->Draw(renderer, layerId++, customData);
			});
		}

		inline void SetNeightbourAlgorithm(NeightbourAlgorithm algorithm) {
			m_neightbour = algorithm;
		}

		inline LuaFile* GetMapScript() {
			return m_pLua;
		}

		inline Layer<Renderable>& operator[] (const Index layer) {
			return *m_layers[layer];
		}

		inline Tile<Renderable>& operator[] (const TileType id) {
			return *m_pTileManager->operator[](id);
		}

	private:
		enum class ChunkType: char {
			MAP = 'M',
			LUA = 'L',
			DATA = 'D'
		};

		Map(std::vector<Layer<Renderable>*>& layers, TileManager<Renderable>* tiles, LuaFile* lua, std::map<std::string, std::vector<char>>& data) : 
			m_layers(layers), 
			m_pTileManager(tiles), 
			m_pLua(lua), 
			m_data(data),
			m_neightbour(&Neighbours::Rect)
		{
			assert(m_layers.size() > 0);
			assert(m_pLua);
			assert(m_pTileManager);

			const auto addRequiredFunctions = [&](LuaFile* lua) {
				lua->RegisterFunction<Map<Renderable>>({ "Map", "setTile" }, &Map<Renderable>::setTile, this);
				lua->RegisterFunction<Map<Renderable>>({ "Map", "getTile" }, &Map<Renderable>::getTile, this);
				lua->RegisterFunction<Map<Renderable>>({ "Map", "addNextTile" }, &Map<Renderable>::addNextTile, this);
				lua->RegisterFunction<Map<Renderable>>({ "Map", "loadNeightbours" }, &Map<Renderable>::loadNeightbour2d, this);
				lua->RegisterFunction<Map<Renderable>>({ "Map", "getNeightbour" }, &Map<Renderable>::getNeightbour2d, this);
			};

			tiles->ForEach([&](Tile<Renderable>* tile) {
				addRequiredFunctions(tile->GetLua());
			});

			addRequiredFunctions(m_pLua);
		}

		Map(Map& map);

		static int setTile(lua_State* lua) {
			auto map = ((Model::Map<Renderable>*)lua_touserdata(lua, lua_upvalueindex(1)));
			const int x = (int)luaL_checknumber(lua, 1), y = (int)luaL_checknumber(lua, 2), z = (int)luaL_checknumber(lua, 3);
			const TileType id = TileType(luaL_checknumber(lua, 4));

			// Get layer
			assert(z >= 0 && z < int(map->m_layers.size()));
			auto layer = map->m_layers[z];

			// Get coordinate
			assert(x >= 0 && x < (int)layer->GetWidth() && y >= 0 && y < (int)layer->GetHeight());
			const auto target = Model::Coordinate(Model::Index(x), Model::Index(y));

			layer->operator[](target) = id;
			layer->IgnoreTile(target);
			return 0;
		}

		static int getTile(lua_State* lua) {
			auto map = ((Model::Map<Renderable>*)lua_touserdata(lua, lua_upvalueindex(1)));
			const int x = (int)luaL_checknumber(lua, 1), y = (int)luaL_checknumber(lua, 2), z = (int)luaL_checknumber(lua, 3);

			auto layer = (z >= 0 && z < int(map->m_layers.size())) ? map->m_layers[z] : nullptr;
			if (layer && x >= 0 && x < (int)layer->GetWidth() && y >= 0 && y < (int)layer->GetHeight()) {
				lua_pushnumber(lua, map->m_layers[z]->operator[](Model::Coordinate((Model::Index)x, (Model::Index)y)));
			} else {
				lua_pushnumber(lua, -1);
			}
			return 1;
		}

		static int addNextTile(lua_State* lua) {
			auto map = ((Model::Map<Renderable>*)lua_touserdata(lua, lua_upvalueindex(1)));
			const int x = (int)luaL_checknumber(lua, 1), y = (int)luaL_checknumber(lua, 2), z = (int)luaL_checknumber(lua, 3);
			assert(z >= 0 && z < int(map->m_layers.size()) && x >= 0 && x < (int)map->m_layers[z]->GetWidth() && y >= 0 && y < (int)map->m_layers[z]->GetHeight());

			map->m_layers[z]->AddNextTile(Model::Coordinate((Model::Index)x, (Model::Index)y));
			return 0;
		}

		static int loadNeightbour2d(lua_State* lua) {
			auto map = ((Model::Map<Renderable>*)lua_touserdata(lua, lua_upvalueindex(1)));
			const int x = (int)luaL_checknumber(lua, 1), y = (int)luaL_checknumber(lua, 2), z = (int)luaL_checknumber(lua, 3), distance = (int)luaL_checknumber(lua, 4);
			assert(z >= 0 && z < int(map->m_layers.size()) && x >= 0 && x < (int)map->m_layers[z]->GetWidth() && y >= 0 && y < (int)map->m_layers[z]->GetHeight() && distance > 0);

			auto& loaded_neightbours = neightbours();
			map->m_neightbour(Coordinate(x, y), distance, loaded_neightbours);
			std::remove_if(loaded_neightbours.begin(), loaded_neightbours.end(), [&](const Coordinate& coord) {
				return ((coord.GetX() == x && coord.GetY() == y) || x >= map->m_layers[z]->GetWidth() || y >= map->m_layers[z]->GetHeight());
			});

			return 0;
		}

		static int getNeightbour2d(lua_State* lua) {
			auto& loaded_neightbours = neightbours();
			if (!loaded_neightbours.empty()) {
				const auto& coord = loaded_neightbours.back();
				lua_pushnumber(lua, coord.GetX());
				lua_pushnumber(lua, coord.GetY());
				
				loaded_neightbours.pop_back();
				return 2;
			} else {
				return 0;
			}
		}

		inline static std::vector<Coordinate>& neightbours() {
			static std::vector<Coordinate> NEIGHTBOURS;
			return NEIGHTBOURS;
		}

		std::vector<Layer<Renderable>*> m_layers;
		std::map<std::string, std::vector<char>> m_data;
		TileManager<Renderable>* m_pTileManager;
		LuaFile* m_pLua;
		NeightbourAlgorithm m_neightbour;
	};
}