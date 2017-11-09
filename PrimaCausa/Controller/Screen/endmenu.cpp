#include "endmenu.h"

#include "../engine.h"

#include "../../View/window.h"
#include "../../View/texture.h"
#include "../../View/Gui/button.h"
#include "../../View/Gui/label.h"

namespace Controller {

	EndMenu::EndMenu(bool win) : m_gui(Engine::GetInstance()->GetContext()) {
		m_gui.PushElement(new View::Label(0, 0, (win ? "You win!" : "You lose!"), View::Label::Size::Large));
		m_gui.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/back.png", [] (const char*) {
			Engine::GetInstance()->PopScreen();
			Engine::GetInstance()->PopScreen();
		}));

		m_gui.SetSpacing(60);
		m_gui.CenterVertical(200.0f);
		m_gui.SetHover(false);
	}

	void EndMenu::DrawGui(View::Window*) {
		m_gui.Draw();
	}

	void EndMenu::ProcessInput(Input* input) {
		m_gui.ProcessInput(input);
	}

	void EndMenu::ProcessReshape(int w, int h) {
		m_gui.CenterVertical(200.0f);
	}
}