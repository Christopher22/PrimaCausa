#include "newgamemenu.h"
#include "ingame.h"

#include "../engine.h"

#include "../../Model/luafile.h"

#include "../../View/window.h"
#include "../../View/texture.h"
#include "../../View/Gui/button.h"
#include "../../View/Gui/textbutton.h"
#include "../../View/Gui/label.h"

#include <algorithm>

namespace Controller {

	NewGameMenu::NewGameMenu() : m_buttons(Engine::GetInstance()->GetContext()) {
		m_buttons.PushElement(new View::Label(0, 0, "Please select a map", View::Label::Size::Large));

		auto& maps = NewGameMenu::GetAvailableMaps();
		std::for_each(maps.begin(), maps.end(), [&] (const std::pair<const std::string, const std::string>& map) {
			// Add a button for each element
			m_buttons.PushElement(new View::TextButton(0.f, 0.f, View::Texture::Load("Assets/Gui/empty.png"), map.first.c_str(), [&] (const char* path) {
				InGame* game = nullptr;
				if ( ( game = InGame::Load("Assets\\tiles.lua", path) ) ) {
					Engine::GetInstance()->PopScreen(false);
					Engine::GetInstance()->PushScreen(game);
				} else {
					Engine::Log("[ERROR] Unable to load map '%s'!", path);
				}
			}, new std::string(map.second)));
		});

		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/back.png", [] (const char*) {
			Engine::GetInstance()->PopScreen();
		}));

		m_buttons.SetSpacing(20);
		m_buttons.CenterVertical(0.6f * Engine::GetInstance()->GetContext()->GetHeight());
		m_buttons.SetHover(false);
	}

	void NewGameMenu::DrawGui(View::Window*) {
		m_buttons.Draw();
	}

	void NewGameMenu::ProcessInput(Input* input) {
		m_buttons.ProcessInput(input);
	}

	void NewGameMenu::ProcessReshape(int w, int h) {
		m_buttons.CenterVertical(0.6f * Engine::GetInstance()->GetContext()->GetHeight());
	}

	std::map<std::string, std::string> NewGameMenu::GetAvailableMaps() {
		return Model::LuaFile::LoadTable<std::string, std::string>("Assets\\maps.lua");
	}
}
