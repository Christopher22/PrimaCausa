#pragma once
#pragma warning( disable : 4996 )

#include "types.h"
#include "coordinate.h"
#include "tilemanager.h"
#include "luafile.h"

#include <queue>
#include <algorithm>
#include <string>
#include <fstream>
#include <limits>
#include <cassert>
#include <unordered_set>

struct lua_State;

namespace Model {

	template<typename Renderable>
	class Layer {
	public:
		Layer(TileManager<Renderable>* tiles, const Index width, const Index height, const TileType defaultTile, const bool isBackground) : m_tiles(tiles), m_width(width), m_height(height), m_pIds(new TileType[width * height]), m_isBackground(isBackground), m_nextTiles() {
			std::fill(m_pIds, m_pIds + width * height - 1, defaultTile);

			for (Index x = 0; x < width; x++) {
				for (Index y = 0; y < height; y++) {
					m_nextTiles.emplace(x, y);
				}
			}
		}

		~Layer() {
			delete[] m_pIds;
			m_pIds = nullptr;
		}

		inline TileType& operator[] (const Coordinate index) {
			return m_pIds[index.GetX() + m_width * index.GetY()];
		}

		inline Index GetHeight() const {
			return m_height;
		}

		inline Index GetWidth() const {
			return m_width;
		}

		inline bool IsBackground() const {
			return m_isBackground;
		}

		inline void Process(const float deltaTime) {
			m_tiles->Process(deltaTime);
		}

		template<typename ErrorHandler>
		void NextTurn(const Index layerId, ErrorHandler luaErrorHandler) {
			if (!m_isBackground) {
				for (auto i = m_nextTiles.size(); i > 0; i--) {
					const auto current = m_nextTiles.front();
					if (m_ignoreTiles.find(current) == m_ignoreTiles.end()) {
						const auto id = (*this)[current];
						const auto tile = (*m_tiles)[id];
						if (tile) {
							tile->Update(current, layerId, luaErrorHandler);
						}
					}
					m_nextTiles.pop();
				}
				m_ignoreTiles.clear();
			}
		}

		inline void AddNextTile(const Coordinate position) {
			m_nextTiles.push(position);
		}

		inline void IgnoreTile(const Coordinate position) {
			m_ignoreTiles.emplace(position);
		}

		template<typename CustomData>
		void Draw(Renderer<Renderable, CustomData> drawer, Index layerId, CustomData customData) const  {
			for (Index x = 0; x < m_width; x++) {
				for (Index y = 0; y < m_height; y++) {
					const auto id = m_pIds[x + m_width * y];
					Tile<Renderable>* target = (*m_tiles)[id];
					if (target) {
						target->Draw(drawer, Coordinate(x, y), layerId, customData);
					}
				}
			}
		}

		bool Save(std::stringstream& output) {
			// Write header
			output.write("P5\n", 3);

			output << std::to_string(m_width) << '\n';
			output << std::to_string(m_height) << '\n';
			output << std::to_string(std::numeric_limits<TileType>::max()) << '\n';

			// Write image data
			for (Index y = 0; y < m_height; y++) {
				for (Index x = 0; x < m_width; x++) {
					output.put(static_cast<char>(m_pIds[x + m_width * y]));
				}
			}

			return true;
		}

		static Layer* Load(TileManager<Renderable>* tiles, std::ifstream& img, bool isBackground) {
			std::string inputLine;
			if (!(img >> inputLine) || inputLine.compare("P5") != 0) {
				return nullptr;
			}

			Index width = 0, height = 0;
			try {
				while (img >> inputLine) {
					if (inputLine.at(0) == '#') {
						continue;
					} else if (width == 0) {
						width = (Index)std::stoi(inputLine);
					} else if (height == 0) {
						height = (Index)std::stoi(inputLine);
					} else {
						if (std::stoi(inputLine) > std::numeric_limits<TileType>::max()) {
							return nullptr;
						}
						break;
					}
				}

				// Check if the while loop was breaked early
				if (height == 0) {
					return nullptr;
				}
			}
			catch (std::exception exeption) {
				return nullptr;
			}

			img.ignore(1); // Ignore whitespace

			char* buffer = new char[width * height];
			if (!img.read(buffer, width * height)) {
				delete[] buffer;
				return nullptr;
			}
			else {
				Layer<Renderable>* result = new Layer<Renderable>(tiles, width, height, 0, isBackground);
				std::transform(buffer, buffer + width * height, result->m_pIds, [](char id) {
					return static_cast<TileType>(id);
				});

				delete[] buffer;
				return result;
			}
		}

	private:
		Layer(const Layer& map);

		TileManager<Renderable>* m_tiles;
		TileType* m_pIds;
		const Index m_width, m_height;
		const bool m_isBackground;
		std::queue<Coordinate> m_nextTiles;
		std::unordered_set<Coordinate> m_ignoreTiles;
	};
}