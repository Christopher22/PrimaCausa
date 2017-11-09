#pragma once

#include "types.h"
#include "luafile.h"
#include "coordinate.h"

#include <string>
#include <cassert>
#include <vector>
#include <array>

namespace Model {
	template<typename Renderable>
	class Tile {
	public:
		~Tile() {
			delete m_pLua;
			m_pLua = nullptr;

			std::for_each(m_pRenderable.begin(), m_pRenderable.end(), [](Renderable* texture) {
				delete texture;
				texture = nullptr;
			});
		}

		void Process(const float deltaTime) {
			if (m_pRenderable.size() > 1) {
				m_timeElapsed += deltaTime;
				if (m_timeElapsed > m_frameSpeed) {
					m_timeElapsed = 0;

					++m_currentFrame;
					if (m_currentFrame >= m_pRenderable.size()) {
						m_currentFrame = 0;
					}
				}
			}
		}

		inline LuaFile* GetLua() {
			return m_pLua;
		}

		template<typename ErrorHandler>
		inline bool Update(const Coordinate position, const Index layer, const ErrorHandler luaErrorHandler) {
			std::array<int, 3> positionArray = { int(position.GetX()), int(position.GetY()), int(layer) };
			return m_pLua->CallFunction<std::array<int, 3>, Model::Empty>({ m_name.c_str(), "process" }, &positionArray, nullptr, luaErrorHandler);
		}

		template<typename CustomData>
		inline void Draw(const Renderer<Renderable, CustomData> drawer, const Coordinate coordinate, Index layerId, CustomData customData) {
			(drawer)(coordinate, layerId, m_pRenderable[m_currentFrame], customData);
		}

		template<typename Renderable, typename CustomData>
		static Tile<Renderable>* LoadFromFile(const RenderableLoader<Renderable, CustomData> loader, const Path path, CustomData customData) {
			LuaFile* file = LuaFile::LoadFile(path);
			if (file == nullptr) {
				return nullptr;
			}

			const auto nameBegin = path.find_last_of("\\") + 1;
			const std::string moduleName(path.substr(nameBegin, path.find_last_of(".") - nameBegin));

			std::map<int, std::string> texture_paths;
			if (!file->Get<int, std::string>({ moduleName.c_str(), "images" }, texture_paths)) {
				delete file;
				file = nullptr;
				return nullptr;
			}

			std::vector<Renderable*> textures;
			for (const auto path : texture_paths) {
				Renderable* texture = (loader)(path.second, customData);
				
				if (texture) {
					textures.push_back(texture);
				} else {
					delete file;
					file = nullptr;
					std::for_each(textures.begin(), textures.end(), [](Renderable* texture) {
						delete texture;
						texture = nullptr;
					});
					return nullptr;
				}
			}

			return new Tile(file, moduleName, textures);
		}

		const std::string& GetName() const {
			return m_name;
		}

	private:
		Tile(LuaFile* state, const std::string& name, const std::vector<Renderable*>& textures) : m_pLua(state), m_pRenderable(textures), m_currentFrame(0), m_timeElapsed(0), m_name(name), m_frameSpeed(1 / float(textures.size())) {
			assert(state);
			assert(textures.size() > 0);
		}
		
		Tile(Tile& copy);

		LuaFile* m_pLua;
		std::vector<Renderable*> m_pRenderable;
		unsigned short m_currentFrame;
		float m_frameSpeed, m_timeElapsed;
		std::string m_name;
	};
}