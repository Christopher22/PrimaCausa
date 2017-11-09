#include "startmenu.h"

#include "savegamemenu.h"
#include "newgamemenu.h"
#include "ingame.h"
#include "aboutmenu.h"
#include "tutorialmenu.h"

#include "../engine.h"

#include "../../View/texture.h"
#include "../../View/window.h"
#include "../../View/Gui/button.h"
#include "../../View/Gui/label.h"

#include <fmod.hpp>

#include <algorithm>
#include <cstdlib>

namespace Controller {

	StartMenu::StartMenu() : m_buttons(Engine::GetInstance()->GetContext()), m_logo(View::Button::Load(0, 0, Engine::GetInstance()->GetContext()->GetWidth() * 0.15f, "Assets/Graphic/logo.jpg", [] (const char*) {})) {
		//m_buttons.PushElement(new View::Label(0, 0, "PrimaCausa"));


		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/title.png", [](const char*) {}));
		m_buttons.PushElement(new View::Label(0, 0, "  ", View::Label::Size::Large));


		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/play.png", [] (const char*) {
			auto& availableMaps = NewGameMenu::GetAvailableMaps();
			if ( availableMaps.size() > 0 ) {
				auto randomMap = availableMaps.begin();
				std::advance(randomMap, rand() % availableMaps.size());

				auto game = InGame::Load("Assets\\tiles.lua", randomMap->second.c_str());
				if ( game ) {
					Engine::GetInstance()->PushScreen(game);
				} else {
					Engine::Log("[ERROR] Unable to load map");
				}
			}
		}));

		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/new.png", [] (const char*) {
			Engine::GetInstance()->PushScreen(new NewGameMenu());
		}));

		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/load.png", [] (const char*) {
			Engine::GetInstance()->PushScreen(new SaveGameMenu(false));
		}));

		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/tutorial.png", [] (const char*) {
			Engine::GetInstance()->PushScreen(new TutorialMenu());
		}));

		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/about.png", [] (const char*) {
			Engine::GetInstance()->PushScreen(new AboutMenu());
		}));

		m_buttons.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/exit.png", [] (const char*) {
			Engine::Exit();
		}));

		if ( Engine::GetInstance()->GetAudio()->createSound("Assets\\Sounds\\background.mp3", FMOD_LOOP_NORMAL ^ FMOD_2D ^ FMOD_CREATESTREAM, nullptr, &m_pBackgroundMusic) != FMOD_OK ) {
			Engine::Log("[WARNING] Loading of background audio failed!");
			m_pBackgroundMusic = nullptr;
		} else {
			Engine::GetInstance()->GetAudio()->playSound(m_pBackgroundMusic, nullptr, false, nullptr);
		}

		m_logo->SetPosition(Engine::GetInstance()->GetContext()->GetWidth() - m_logo->GetWidth() - m_logo->GetWidth() * 0.3f, Engine::GetInstance()->GetContext()->GetHeight() - m_logo->GetHeight());
		m_buttons.SetHover(false);
		m_buttons.CenterVertical(0.5f * Engine::GetInstance()->GetContext()->GetHeight());
	}

	StartMenu::~StartMenu() {
		if ( m_pBackgroundMusic ) {
			m_pBackgroundMusic->release();
			m_pBackgroundMusic = nullptr;
		}
	}

	void StartMenu::DrawGui(View::Window*) {
		m_buttons.Draw();
		if ( m_logo ) {
			m_logo->Draw();
		}
	}

	void StartMenu::ProcessInput(Input* input) {
		m_buttons.ProcessInput(input);
	}

	void StartMenu::ProcessReshape(int w, int h) {
		m_buttons.CenterVertical(0.5f * h);
		if ( m_logo ) {
			m_logo->SetWidth(Engine::GetInstance()->GetContext()->GetWidth() * 0.15f);
			m_logo->SetPosition(Engine::GetInstance()->GetContext()->GetWidth() - m_logo->GetWidth() - m_logo->GetWidth() * 0.3f, Engine::GetInstance()->GetContext()->GetHeight() - m_logo->GetHeight());
		}
	}
}