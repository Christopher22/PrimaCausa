#pragma once

#include "screen.h"

#include "../../View/Gui/guicontainer.h"

namespace View {
	class Window;
	class Input;
}

namespace Controller {
	class TutorialMenu : public Screen {
	public:
		TutorialMenu();
		void DrawGui(View::Window* context);
		void ProcessInput(Input* input);
		void ProcessReshape(int w, int h);
	private:
		TutorialMenu(TutorialMenu& menu);
		View::GuiContainer m_gui;
	};
}