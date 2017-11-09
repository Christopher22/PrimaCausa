#include "ingame.h"
#include "endmenu.h"
#include "savegamemenu.h"

#include "../input.h"
#include "../engine.h"

#include "../../View/drawable.h"
#include "../../View/mesh.h"
#include "../../View/window.h"
#include "../../View/texture.h"
#include "../../View/material.h"
#include "../../View/colorprofile.h"

#include "../../View/Utils/Vec3d.h"
#include "../../View/Utils/vec2d.h"
#include "../../View/Utils/linearscale.h"

#include "../../View/Gui/guicontainer.h"
#include "../../View/Gui/textbutton.h"
#include "../../View/Gui/button.h"

#include <cassert>
#include <string>
#include <cstdio>

namespace Controller {

	InGame::InGame(Model::TileManager<View::Drawable>* tiles, Model::Map<View::Drawable>* map) :
		m_pMap(map),
		m_pClicked(nullptr),
		m_LastClick(View::Vec2d<float>(0, 0)),
		m_camPos(View::Vec2d<float>(0, 0)),
		m_status(Status::Running),
		m_turns(0),
		m_divinity(500),
		m_resourcePanel(Engine::GetInstance()->GetContext(), 0.03f*Engine::GetInstance()->GetContext()->GetWidth(), .1f*Engine::GetInstance()->GetContext()->GetHeight()),
		m_terrainPanel(Engine::GetInstance()->GetContext(), 0.8f*Engine::GetInstance()->GetContext()->GetWidth(), .1f*Engine::GetInstance()->GetContext()->GetHeight()),
		m_powerPanel(Engine::GetInstance()->GetContext(), 0.9f * Engine::GetInstance()->GetContext()->GetHeight(), .1f*Engine::GetInstance()->GetContext()->GetHeight()),
		m_configPanel(Engine::GetInstance()->GetContext(), 0.9f * Engine::GetInstance()->GetContext()->GetHeight(), .9f*Engine::GetInstance()->GetContext()->GetHeight()),
		m_statusLabel(float(0.05f*Engine::GetInstance()->GetContext()->GetWidth()), float(0.9f*Engine::GetInstance()->GetContext()->GetHeight()), "", View::Label::Size::Medium, { { 90.0f / 256, 200.0f / 256, 250.0f / 256 } }),
		m_godMode(false) {

		// Closure for registister constants and variables
		const auto addRequiredFunctions = [&] (Model::LuaFile* lua) {
			lua->Set<int>({ "PrimaCausa", "STATUS", "RUNNING" }, int(Status::Running));
			lua->Set<int>({ "PrimaCausa", "STATUS", "WIN" }, int(Status::Win));
			lua->Set<int>({ "PrimaCausa", "STATUS", "LOSE" }, int(Status::Lose));

			lua->Set<int>({ "PrimaCausa", "TERRAIN", "PLAIN" }, int(Terrain::Plain));
			lua->Set<int>({ "PrimaCausa", "TERRAIN", "MOUNTAIN" }, int(Terrain::Mountain));
			lua->Set<int>({ "PrimaCausa", "TERRAIN", "WATER" }, int(Terrain::Water));
			lua->Set<int>({ "PrimaCausa", "TERRAIN", "DESERT" }, int(Terrain::Desert));

			lua->Set<int>({ "PrimaCausa", "SETTLEMENT", "VILLAGE" }, int(Settlement::Village));
			lua->Set<int>({ "PrimaCausa", "SETTLEMENT", "TOWN" }, int(Settlement::Town));
			lua->Set<int>({ "PrimaCausa", "SETTLEMENT", "CITY" }, int(Settlement::City));
			lua->Set<int>({ "PrimaCausa", "SETTLEMENT", "METROPOLIS" }, int(Settlement::Metropolis));

			lua->Set<int>({ "PrimaCausa", "RESOURCE", "GRAIN" }, int(Resource::Grain));
			lua->Set<int>({ "PrimaCausa", "RESOURCE", "WATER" }, int(Resource::Water));
			lua->Set<int>({ "PrimaCausa", "RESOURCE", "FISH" }, int(Resource::Fish));
			lua->Set<int>({ "PrimaCausa", "RESOURCE", "STONE" }, int(Resource::Stone));
			lua->Set<int>({ "PrimaCausa", "RESOURCE", "ORE" }, int(Resource::Ore));
			lua->Set<int>({ "PrimaCausa", "RESOURCE", "URANIUM" }, int(Resource::Uranium));

			lua->RegisterFunction<InGame>({ "PrimaCausa", "log" }, &InGame::logLua, this);
			lua->RegisterFunction<InGame>({ "PrimaCausa", "getDivinity" }, &InGame::getDivinity, this);
			lua->RegisterFunction<InGame>({ "PrimaCausa", "updateDivinity" }, &InGame::updateDivinity, this);
			lua->RegisterFunction<InGame>({ "PrimaCausa", "getTurns" }, &InGame::getTurns, this);
		};

		assert(tiles);
		assert(map);

		// Add functions to the tiles and map script
		tiles->ForEach([&] (Model::Tile<View::Drawable>* tile) {
			addRequiredFunctions(tile->GetLua());
		});
		addRequiredFunctions(m_pMap->GetMapScript());

		// Load divinity
		const auto savedDivinity = m_pMap->GetValue<int>("divinity");
		if ( savedDivinity ) {
			m_divinity = *savedDivinity;
		}

		// Load turns
		const auto savedTurns = m_pMap->GetValue<unsigned short>("turns");
		if ( savedTurns ) {
			m_turns = *savedTurns;
		}

		// Set neightbour algorithm
		m_pMap->SetNeightbourAlgorithm(Model::Neighbours::Hex);

		// Precalculate points for the 2D->3D mapping
		for ( Model::Index i = 0, width = m_pMap->GetWidth(); i < width; i++ ) {
			for ( Model::Index j = 0, height = m_pMap->GetHeight(); j < height; j++ ) {
				float d = std::sqrtf(std::powf(View::Hex::Size / 2, 2) + std::powf(View::Hex::Size, 2));
				float x = View::LinearScale::Scale(0.0f, 2.0f / ( View::Hex::Size*1.5f ), -1.0f, +1.0f, float(i));
				float y = View::LinearScale::Scale(0.0f, 2.0f / ( d*2.0f ), -1.0f, +1.0f, float(j));
				if ( ( i % 2 ) != 0 ) {                        //offset
					y += d;
				}
				m_tileCenters.push_back(View::Vec2d<float>(x, y));
			}
		}

		// Create Gui
		m_terrainPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/plains.png", [&] (const char*) { setCurrentTerrain(Terrain::Plain); }));
		m_terrainPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/sea.png", [&] (const char*) { setCurrentTerrain(Terrain::Water); }));
		m_terrainPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/mountain.png", [&] (const char*) { setCurrentTerrain(Terrain::Mountain); }));
		m_terrainPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/desert.png", [&] (const char*) { setCurrentTerrain(Terrain::Desert); }));

		m_resourcePanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/grain.png", [&] (const char*) { setCurrentResource(Resource::Grain); }));
		m_resourcePanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/water.png", [&] (const char*) { setCurrentResource(Resource::Water); }));
		m_resourcePanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/fish.png", [&] (const char*) { setCurrentResource(Resource::Fish); }));
		m_resourcePanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/stone.png", [&] (const char*) { setCurrentResource(Resource::Stone); }));
		m_resourcePanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/ore.png", [&] (const char*) { setCurrentResource(Resource::Ore); }));
		m_resourcePanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/uranium.png", [&] (const char*) { setCurrentResource(Resource::Uranium); }));

		m_powerPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/downgrade.png", [&] (const char*) { changeSettlement(false); }));
		m_powerPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/wrath.png", [&] (const char*) { wrath(); }));
		m_powerPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/upgrade.png", [&] (const char*) { changeSettlement(true); }));

		m_configPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/save.png", [&] (const char*) {
			Engine::GetInstance()->PushScreen(new SaveGameMenu(true));
		}));

		m_configPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/load.png", [&] (const char*) {
			Engine::GetInstance()->PushScreen(new SaveGameMenu(false));
		}));

		m_configPanel.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/exit.png", [&] (const char*) {
			Engine::GetInstance()->PopScreen();
		}));

		// Format GUI
		const auto width = Engine::GetInstance()->GetContext()->GetWidth(), height = Engine::GetInstance()->GetContext()->GetHeight();
		m_terrainPanel.PackVertical(0.2f*height);
		m_resourcePanel.PackVertical(0.3f*height);
		m_configPanel.PackHorizontal(0.2f* width);

		m_powerPanel.PackHorizontal(0.5f* width);
		m_powerPanel.SetX(width / 2 - m_powerPanel.GetWidth() / 2);
		m_powerPanel.PackHorizontal(0.5f* width);

		m_configPanel.PackHorizontal(0.3f * width);
		m_configPanel.SetPosition(0.9f * width - m_configPanel.GetWidth(), .9f*height);
		m_configPanel.PackHorizontal(0.3f * width);

		updateStatusLabel();
	}

	InGame* InGame::Load(Model::Path tiles, Model::Path map) {
		// Load tile manager
		auto tileManager = new Model::TileManager<View::Drawable>();
		bool result = tileManager->Load<Model::Empty*>([] (Model::Path path, Model::Empty*) -> View::Drawable* {
			if ( path.find(".png") != std::string::npos ) {
				// Load hex in case of image
				View::Texture* tmp = nullptr;
				if ( ( tmp = View::Texture::Load(path.c_str()) ) ) {
					View::Material* tmp_mat = new View::Material();
					return new View::Hex(tmp, tmp_mat);
				}
			} else {
				// Load model
				View::Material* tmp_mat = new View::Material();
				auto mesh = View::Mesh::Load(path.c_str(), tmp_mat);
				if ( mesh ) {
					if (path == "Assets\\3D\\village.obj"){
						mesh->SetColor(View::ColorProfile::BROWN);
						mesh->Translate(0,+0.0f,-0.0f);
					} else if (path == "Assets\\3D\\town.obj"){
						mesh->SetColor(View::ColorProfile::TEAL);
						mesh->Translate(0, 0, -1.0f);
						mesh->Translate(0,0,0);
					} else if (path == "Assets\\3D\\city.obj"){
						mesh->SetColor(View::ColorProfile::YELLOW);
						mesh->Translate(0,0,0);
					} else{
						mesh->SetColor(View::ColorProfile::GRAY);
						mesh->Translate(0.025f,0.025f,-0.1f);
					}
					return mesh;
				} else {
					delete tmp_mat;
				}
			}
			return nullptr;
		}, tiles, nullptr);

		// Load map
		Model::Map<View::Drawable>* mapPointer = nullptr;
		if ( result && ( mapPointer = Model::Map<View::Drawable>::Load(map, tileManager) ) != nullptr ) {
			return new InGame(tileManager, mapPointer);
		} else {
			delete tileManager;
			return nullptr;
		}
	}

	InGame::~InGame() {
		delete m_pMap;
		m_pMap = nullptr;

		delete m_pClicked;
		m_pClicked = nullptr;
	}

	bool InGame::SaveGame(Model::Path path) {
		// Ignore divinity and turns in god mode
		if ( !m_godMode ) {
			m_pMap->SetValue("divinity", m_divinity);
			m_pMap->SetValue("turns", m_turns);
			assert(*m_pMap->GetValue<int>("divinity") == m_divinity);
			assert(*m_pMap->GetValue<unsigned short>("turns") == m_turns);
		}

		return m_pMap->Save(path);
	}

	void InGame::Draw(View::Window* context) {
		m_pMap->Draw<InGame*>([] (Model::Coordinate pos, Model::Index layer, View::Drawable* drawable, InGame* ingame) {
			drawable->Draw(pos.GetX(), pos.GetY(), layer, ( ingame->m_pClicked != nullptr && pos == *( ingame->m_pClicked ) ));
		}, this);
	}

	void InGame::DrawGui(View::Window* context) {
		m_terrainPanel.Draw();
		m_resourcePanel.Draw();
		m_powerPanel.Draw();
		m_configPanel.Draw();
		if ( !m_godMode ) {
			m_statusLabel.Draw();
		}
	}

	void InGame::ProcessInput(Input* input) {
		// Check if any GUI element handle the input
		bool consumed = false;
		consumed = m_terrainPanel.ProcessInput(input) || consumed;
		consumed = m_resourcePanel.ProcessInput(input) || consumed;
		consumed = m_configPanel.ProcessInput(input) || consumed;
		consumed = m_powerPanel.ProcessInput(input) || consumed;
		if ( consumed ) {
			return;
		}

		View::Window* win = Engine::GetInstance()->GetContext();
		const float x = float(input->GetX()), y = float(input->GetY());
		const unsigned short width = win->GetWidth(), height = win->GetHeight();

		switch ( input->GetId() ) {
			case 'M':{
				const Input::MouseEvent* mouseInput = dynamic_cast<Input::MouseEvent*>( input );
				if ( mouseInput->GetButton() == Input::MouseEvent::Button::Left && mouseInput->GetState() == Input::MouseEvent::State::Down ) {
					m_LastClick = ( mouseInput->GetPos(width, height) );
					m_camPos = ( win->GetCameraPosition() );

					GLint viewport[4];
					GLdouble modelview[16];
					GLdouble projection[16];
					GLfloat winX, winY, winZ;
					GLdouble posX, posY, posZ;

					glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
					glGetDoublev(GL_PROJECTION_MATRIX, projection);
					glGetIntegerv(GL_VIEWPORT, viewport);

					winX = x;
					winY = (float) viewport[3] - y;
					glReadPixels(int(x), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

					gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

					View::Vec2d<float> mP((float) posX, (float) posY);

					int p, q;
					float distance = 1000;
					auto w = m_pMap->GetWidth();
					for ( unsigned int i = 0; i < m_tileCenters.size(); i++ ) {
						int tempp = i / w;
						int tempq = i %  w;
						View::Vec2d<float> diff = mP - m_tileCenters[i];
						float currdistance = diff.Magnitude();
						if ( currdistance <= distance ) {
							distance = currdistance;
							p = tempp;
							q = tempq;
						}
					}

					m_pClicked = new Model::Coordinate(p, q);
				} else if ( mouseInput->GetButton() == Input::MouseEvent::Button::None && mouseInput->GetState() != Input::MouseEvent::State::Up ) {
					View::Vec2d<float> currMouse = mouseInput->GetPos(width, height);
					View::Vec2d<float> diff = currMouse - m_LastClick;
					View::Vec2d<float> newCam = m_camPos - diff;
					win->SetCameraPosition(newCam);
				}

				if ( mouseInput->GetButton() == Input::MouseEvent::Button::WheelUp ) {
					win->MoveCamera('i', 0.2f);
				} else if ( mouseInput->GetButton() == Input::MouseEvent::Button::WheelDown ) {
					win->MoveCamera('o', 0.2f);
				}

				break;
			}
			case 'K':{
				const Input::KeyboardEvent* kbInput = dynamic_cast<Input::KeyboardEvent*>( input );
				switch ( kbInput->GetKey() ) {
					case Input::KeyboardEvent::Key::Up: win->MoveCamera('u', 0.1f); break;
					case Input::KeyboardEvent::Key::Down: win->MoveCamera('d', 0.1f); break;
					case Input::KeyboardEvent::Key::Left: win->MoveCamera('l', 0.1f); break;
					case Input::KeyboardEvent::Key::Right: win->MoveCamera('r', 0.1f); break;
					case Input::KeyboardEvent::Key::Space: nextTurn(); break;
					case Input::KeyboardEvent::Key::GodMode: m_godMode = !m_godMode; break;
				}
				break;
			}
			default:
				break;
		}
	}

	bool InGame::nextTurn() {
		if ( m_status == Status::Running ) {
			// Calculate changes
			m_pMap->NextTurn([] (const char* message) {
				Engine::Log("[LUA] Error in calculating next turn: %s\n", message);
			});

			if ( !m_godMode ) {
				// Increase turn counter
				++m_turns;

				// Calculate if game endet
				int gameStatus;
				if ( m_pMap->GetMapScript()->CallFunction<std::array<Model::Empty, 0>>({ "getStatus" }, nullptr, &gameStatus, [] (const char* message) {
					Engine::Log("[LUA] Error in recieving current game status: %s\n", message);
				}) ) {
					switch ( gameStatus ) {
						case int(Status::Running) : m_status = Status::Running; break;
						case int(Status::Win) : m_status = Status::Win; break;
						case int(Status::Lose) : m_status = Status::Lose; break;
						default: Engine::Log("[ERROR] Invalid game status"); return false;
					}

					if ( m_status != Status::Running ) {
						Engine::GetInstance()->PushScreen(new EndMenu(m_status == Status::Win));
					}
				}

				updateStatusLabel();
			}
		}

		return m_status == Status::Running;
	}

	bool InGame::wrath() {
		// Destroy a settlement with enough power
		if ( m_pClicked && ( m_godMode || m_divinity - WRATH_COST >= 0 ) ) {
			m_pMap->operator[](Model::Index(1)).operator[](*m_pClicked) = 0;
			if ( !m_godMode ) {
				m_divinity -= WRATH_COST;
				updateStatusLabel();
			}
			return true;
		} else {
			return false;
		}
	}

	bool InGame::changeSettlement(bool upgrade) {
		if ( !m_pClicked ) {
			return false;
		}

		// Up- or downgrade settlement
		auto& settlementTile = m_pMap->operator[](Model::Index(1)).operator[](*m_pClicked);
		if ( settlementTile != Model::TileType(Settlement::Village) && settlementTile != Model::TileType(Settlement::Town) && settlementTile != Model::TileType(Settlement::City) && settlementTile != Model::TileType(Settlement::Metropolis) ) {
			return false;
		} else if ( upgrade && ( m_godMode || m_divinity - UPGRADE_COST >= 0 ) && settlementTile != Model::TileType(Settlement::Metropolis) ) {
			if ( !m_godMode ) {
				m_divinity -= UPGRADE_COST;
			}
			settlementTile += 1;
		} else if ( !upgrade && ( m_godMode || m_divinity - DOWNGRADE_COST >= 0 ) && settlementTile != Model::TileType(Settlement::Village) ) {
			if ( !m_godMode ) {
				m_divinity -= DOWNGRADE_COST;
			}
			settlementTile -= 1;
		} else {
			return false;
		}

		updateStatusLabel();
		return true;
	}

	bool InGame::setCurrentTerrain(Terrain terrain) {
		if ( !m_pClicked ) {
			return false;
		}

		auto& resourceTile = m_pMap->operator[](Model::Index(1)).operator[](*m_pClicked);
		if ( resourceTile == Model::TileType(Settlement::Village) || resourceTile == Model::TileType(Settlement::Town) || resourceTile == Model::TileType(Settlement::City) || resourceTile == Model::TileType(Settlement::Metropolis) ) {
			return false;
		}

		// Call the cost function and do operation, if possible
		int costs = 0;
		auto& tile = m_pMap->operator[](Model::TileType(terrain));
		if ( ( m_godMode || ( tile.GetLua()->CallFunction<std::array<Model::Empty, 0>, int>({ tile.GetName().c_str(), "getCosts" }, nullptr, &costs, [] (const char* message) {
			Engine::Log("[LUA] Error in recieving costs of tile: %s\n", message);
		}) && m_divinity - costs >= 0 ) ) ) {
			m_pMap->operator[](Model::Index(0)).operator[](*m_pClicked) = Model::TileType(terrain);

			// Remove resource, if there
			resourceTile = 0;

			if ( !m_godMode ) {
				m_divinity -= costs;
				updateStatusLabel();
			}

			return true;
		} else {
			return false;
		}
	}

	bool InGame::setCurrentResource(Resource resource) {
		int costs = 0;
		auto& tile = m_pMap->operator[](Model::TileType(resource));

		if ( !m_pClicked ) {
			return false;
		}

		auto& resourceTile = m_pMap->operator[](Model::Index(1)).operator[](*m_pClicked);
		if ( resourceTile == Model::TileType(Settlement::Village) || resourceTile == Model::TileType(Settlement::Town) || resourceTile == Model::TileType(Settlement::City) || resourceTile == Model::TileType(Settlement::Metropolis) ) {
			return false;
		}

		const std::array<int, 2> pos = { m_pClicked->GetX(), m_pClicked->GetY() };
		// Call the cost function and do operation, if possible
		if ( m_pClicked
			&& tile.GetLua()->CallFunction<std::array<int, 2>, int>({ tile.GetName().c_str(), "getCosts" }, &pos, &costs, [] (const char* message) { Engine::Log("[LUA] Error in recieving costs of tile: %s\n", message); })
			&& costs >= 0
			&& ( m_godMode || m_divinity - costs >= 0 ) ) {
			resourceTile = Model::TileType(resource);
			if ( !m_godMode ) {
				m_divinity -= costs;
				updateStatusLabel();
			}

			return true;
		} else {
			return false;
		}
	}

	InGame::Status InGame::GetStatus() const {
		return m_status;
	}

	void InGame::updateStatusLabel() {
		char* buffer = new char[40];
		std::sprintf(buffer, "Divinity: %.5i (Turn %.3i)", m_divinity, m_turns);
		m_statusLabel.SetText(buffer);
		delete[] buffer;
	}

	int InGame::logLua(lua_State* lua) {
		const auto message = luaL_checkstring(lua, 1);
		if ( message ) {
			Engine::Log("[LUA] %s\n", message);
		}
		return 0;
	}

	int InGame::getTurns(lua_State* lua) {
		const auto game = ( (InGame*) lua_touserdata(lua, lua_upvalueindex(1)) );
		assert(game);

		lua_pushnumber(lua, int(game->m_turns));
		return 1;
	}

	int InGame::getDivinity(lua_State* lua) {
		const auto game = ( (InGame*) lua_touserdata(lua, lua_upvalueindex(1)) );
		assert(game);

		lua_pushnumber(lua, int(game->m_divinity));
		return 1;
	}

	int InGame::updateDivinity(lua_State* lua) {
		auto game = ( (InGame*) lua_touserdata(lua, lua_upvalueindex(1)) );
		assert(game);

		if ( !game->m_godMode ) {
			game->m_divinity += (int) luaL_checknumber(lua, 1);
		}
		return 0;
	}

	void InGame::ProcessReshape(int w, int h) {
		m_resourcePanel.SetPosition(0.03f * w, .1f * h);
		m_terrainPanel.SetPosition(0.8f * w, .1f * h);

		m_terrainPanel.PackVertical(0.2f * h);
		m_resourcePanel.PackVertical(0.3f * h);

		m_configPanel.PackHorizontal(0.3f * w);
		m_configPanel.SetPosition(0.9f * w - m_configPanel.GetWidth(), .9f * h);
		m_configPanel.PackHorizontal(0.3f * w);

		m_powerPanel.PackHorizontal(0.5f * w);
		m_powerPanel.SetPosition(w / 2 - m_powerPanel.GetWidth() / 2, .1f * h);
		m_powerPanel.PackHorizontal(0.5f * w);

		m_statusLabel.SetX(float(0.05f * w));
		m_statusLabel.SetY(float(0.95f * h));
	}
}