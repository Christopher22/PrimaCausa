#include "tutorialmenu.h"

#include "../engine.h"

#include "../../View/window.h"
#include "../../View/texture.h"
#include "../../View/Gui/button.h"
#include "../../View/Gui/label.h"

namespace Controller {

	TutorialMenu::TutorialMenu() : m_gui(Engine::GetInstance()->GetContext()) {
		m_gui.PushElement(new View::Label(0, 0, "Tutorial",View::Label::Size::Large));
		m_gui.PushElement(new View::Label(0, 0, "Selection: Click"));
		m_gui.PushElement(new View::Label(0, 0, "Movement: Click & drag mouse button"));
		m_gui.PushElement(new View::Label(0, 0, "Zooming: Mouse wheel"));
		m_gui.PushElement(new View::Label(0, 0, "'f': Toggle fullscreen"));
		m_gui.PushElement(new View::Label(0, 0, "<spacebar>: End Turn"));
		m_gui.PushElement(new View::Label(0, 0, "'g': Enable debugging feature 'god mode'"));

		m_gui.PushElement(new View::Label(0, 0, " ", View::Label::Size::Medium));

		m_gui.PushElement(new View::Label(0, 0, "HOW TO PLAY", View::Label::Size::Medium));
		m_gui.PushElement(new View::Label(0, 0, "Select a tile and click on a button to change tile properties. This costs divinity.                                                                      ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "A tile has terrain and resources. A settlement on a tile removes the resources it has.                                                            ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "The neighbours of a tile are all tiles within a 2 hex radius.                                                                                                      ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "If a settlement has the resources it needs then it may upgrade to higher tier settlement or expand to the neighboring tiles.", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "Higher tier settlements provide more divinity if they have the resources they need. Lack of resources has divinity penalty.   ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "VILLAGE needs WATER (resource or terrain) and GRAIN to upgrade to TOWN.                                                                 ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "TOWN needs STONE, FISH and above to upgrade to CITY.                                                                                              ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "CITY needs URANIUM, OIL and above to upgrade to METROPOLIS.                                                                                ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "URANIUM can only be in DESERT or MOUNTAIN                                                                                                             ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "GRAIN can only be in PLAIN                                                                                                                                              ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "FISH can only be in SEA                                                                                                                                                     ", View::Label::Size::Small));
		m_gui.PushElement(new View::Label(0, 0, "OIL can be anywhere but SEA                                                                                                                                             ", View::Label::Size::Small));

		m_gui.PushElement(View::Button::Load(0, 0, 0, "Assets/Gui/Menu/back.png", [] (const char*) {
			Engine::GetInstance()->PopScreen();
		}));

		m_gui.SetSpacing(5);
		m_gui.CenterVertical(200.0f);
		m_gui.SetHover(false);
	}

	void TutorialMenu::DrawGui(View::Window*) {
		m_gui.Draw();
	}

	void TutorialMenu::ProcessInput(Input* input) {
		m_gui.ProcessInput(input);
	}

	void TutorialMenu::ProcessReshape(int w, int h) {
		m_gui.CenterVertical( 200.0f);
	}
}