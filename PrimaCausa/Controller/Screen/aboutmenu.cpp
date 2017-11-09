#include "aboutmenu.h"

#include "../engine.h"

#include "../../View/window.h"
#include "../../View/texture.h"
#include "../../View/Gui/button.h"
#include "../../View/Gui/label.h"

namespace Controller {

	AboutMenu::AboutMenu() : m_gui(Engine::GetInstance()->GetContext()) {
		m_gui.PushElement(new View::Label(0, 0, "About", View::Label::Size::Large));
		m_gui.PushElement(new View::Label(0, 0, "Programmed by Aritra Das and Christopher Gundler"));
		m_gui.PushElement(new View::Label(0, 0, "Texture tiles by Soartex"));
		m_gui.PushElement(new View::Label(0, 0, "Button images by Teinstud (Freepika)"));
		m_gui.PushElement(new View::Label(0, 0, "Music by Igor Jovanovic & Jaan Patterson"));
		m_gui.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/back.png", [] (const char*) {
			Engine::GetInstance()->PopScreen();
		}));

		m_gui.SetSpacing(20);
		m_gui.CenterVertical(200.0f);
		m_gui.SetHover(false);
	}

	void AboutMenu::DrawGui(View::Window*) {
		m_gui.Draw();
	}

	void AboutMenu::ProcessInput(Input* input) {
		m_gui.ProcessInput(input);
	}

	void AboutMenu::ProcessReshape(int w, int h) {
		m_gui.CenterVertical(200.0f);
	}
}