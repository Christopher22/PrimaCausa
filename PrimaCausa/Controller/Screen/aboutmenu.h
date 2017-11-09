#pragma once

#include "screen.h"

#include "../../View/Gui/guicontainer.h"

namespace View {
	class Window;
	class Input;
}

namespace Controller {
	class AboutMenu : public Screen {
	public:
		AboutMenu();
		void DrawGui(View::Window* context);
		void ProcessInput(Input* input);
		void ProcessReshape(int w, int h);
	private:
		AboutMenu(AboutMenu& menu);
		View::GuiContainer m_gui;
	};
}