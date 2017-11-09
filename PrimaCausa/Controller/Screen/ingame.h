#pragma once

#include "screen.h"
#include "../../View/hex.h"

#include "../../Model/map.h"
#include "../../Model/types.h"

#include "../../View/Utils/vec2d.h"
#include "../../View/Utils/Vec3d.h"

#include "../../View/Gui/guicontainer.h"
#include "../../View/Gui/label.h"

#include <vector>

namespace View {
	class Window;
	class Input;
}

namespace Controller {
	class InGame : public Screen {
	public:
		const static unsigned short WRATH_COST = 800;
		const static unsigned short UPGRADE_COST = 800;
		const static unsigned short DOWNGRADE_COST = 800;

		enum class Status {
			Running = 0,
			Win = 1,
			Lose = 2
		};

		enum class Settlement : Model::TileType {
			Village = 1,
			Town = 2,
			City = 3,
			Metropolis = 4
		};

		enum class Resource : Model::TileType {
			Grain = 10,
			Water = 11,
			Fish = 12,
			Stone = 13,
			Ore = 14,
			Uranium = 15
		};

		enum class Terrain : Model::TileType {
			Plain = 255,
			Mountain = 254,
			Water = 253,
			Desert = 252
		};

		static InGame* Load(Model::Path tiles, Model::Path map);

		virtual ~InGame();
		void Draw(View::Window* context);
		void DrawGui(View::Window* context);
		void ProcessInput(Input* input);
		bool SaveGame(Model::Path path);

		Status GetStatus() const;
		void ProcessReshape(int w, int h);
	private:
		InGame(Model::TileManager<View::Drawable>* tiles, Model::Map<View::Drawable>* map);
		InGame(InGame& menu);

		bool nextTurn();
		bool setCurrentTerrain(Terrain terrain);
		bool setCurrentResource(Resource terrain);
		bool wrath();
		bool changeSettlement(bool upgrade);
		void updateStatusLabel();

		static int logLua(lua_State* lua);
		static int getTurns(lua_State* lua);
		static int getDivinity(lua_State* lua);
		static int updateDivinity(lua_State* lua);

		Status m_status;
		unsigned short m_turns;
		int m_divinity;
		bool m_godMode;

		View::Vec2d<float> m_LastClick, m_camPos;
		std::vector<View::Vec2d<float>> m_tileCenters;

		Model::Coordinate* m_pClicked;
		Model::Map<View::Drawable>* m_pMap;

		View::GuiContainer m_resourcePanel, m_terrainPanel, m_powerPanel, m_configPanel;
		View::Label m_statusLabel;
	};
}