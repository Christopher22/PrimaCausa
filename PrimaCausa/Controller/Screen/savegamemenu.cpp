#include "savegamemenu.h"
#include "ingame.h"

#include "../engine.h"

#include "../../View/texture.h"
#include "../../View/window.h"
#include "../../View/Gui/button.h"
#include "../../View/Gui/textbutton.h"
#include "../../View/Gui/label.h"

#include <fstream>

namespace Controller {
	const char* SaveGameMenu::SAVEGAME_PATH = "Savegames/slot%.1u.map";

	SaveGameMenu::SaveGameMenu(bool save) : m_gui(Engine::GetInstance()->GetContext()), m_saveGame(save) {
		m_gui.PushElement(new View::Label(0, 0, (save ? "Save game:" : "Load game:"), View::Label::Size::Large));

		m_gui.PushElement(new View::TextButton(0.f, 0.f, View::Texture::Load("Assets/Gui/empty.png"), ( fileExist(unsigned char(0)) ? "Slot 0" : "Slot 0 (Free)" ), [&] (const char*) {
			processSlot(0);
		}));

		m_gui.PushElement(new View::TextButton(0.f, 0.f, View::Texture::Load("Assets/Gui/empty.png"), ( fileExist(unsigned char(1)) ? "Slot 1" : "Slot 1 (Free)" ), [&] (const char*) {
			processSlot(1);
		}));

		m_gui.PushElement(new View::TextButton(0.f, 0.f, View::Texture::Load("Assets/Gui/empty.png"), ( fileExist(unsigned char(2)) ? "Slot 2" : "Slot 2 (Free)" ), [&] (const char*) {
			processSlot(2);
		}));

		m_gui.PushElement(new View::TextButton(0.f, 0.f, View::Texture::Load("Assets/Gui/empty.png"), ( fileExist(unsigned char(3)) ? "Slot 3" : "Slot 3 (Free)" ), [&] (const char*) {
			processSlot(3);
		}));

		m_gui.PushElement(new View::TextButton(0.f, 0.f, View::Texture::Load("Assets/Gui/empty.png"), ( fileExist(unsigned char(4)) ? "Slot 4" : "Slot 4 (Free)" ), [&] (const char*) {
			processSlot(4);
		}));

		m_gui.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/back.png", [] (const char*) {
			Engine::GetInstance()->PopScreen();
		}));

		m_gui.SetSpacing(20);
		m_gui.SetHover(false);
		m_gui.CenterVertical(0.4f * Engine::GetInstance()->GetContext()->GetHeight());
	}

	void SaveGameMenu::DrawGui(View::Window*) {
		m_gui.Draw();
	}

	void SaveGameMenu::ProcessInput(Input* input) {
		m_gui.ProcessInput(input);
	}

	void SaveGameMenu::ProcessReshape(int w, int h) {
		m_gui.CenterVertical(0.6f * h);
	}

	void SaveGameMenu::processSlot(unsigned char slot) {
		char* buffer = new char[22];
		std::sprintf(buffer, SAVEGAME_PATH, slot);

		InGame* game = nullptr;
		if ( m_saveGame ) {
			Engine::GetInstance()->PopScreen();
			game = dynamic_cast<InGame*>( Engine::GetInstance()->GetScreen() );
			if ( !game->SaveGame(buffer) ) {
				Engine::Log("Error: Unable to save game to '%s'!", buffer);
			}
		} else if ( fileExist(buffer) && ( game = InGame::Load("Assets\\tiles.lua", buffer) ) ) {
			// Remove game in background
			if ( Engine::GetInstance()->GetNumScreens() > 2 ) {
				Engine::GetInstance()->PopScreen(false);
			}
			Engine::GetInstance()->PopScreen(false);
			Engine::GetInstance()->PushScreen(game);
		}

		delete[] buffer;
	}

	bool SaveGameMenu::fileExist(const char* fileName) {
		std::ifstream infile(fileName);
		return infile.good();
	}

	bool SaveGameMenu::fileExist(unsigned char slot) {
		char* buffer = new char[22];
		std::sprintf(buffer, SAVEGAME_PATH, slot);
		const bool status = fileExist(buffer);
		delete[] buffer;
		return status;
	}
}