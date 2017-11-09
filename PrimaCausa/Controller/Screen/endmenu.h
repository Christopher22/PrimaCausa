#pragma once

#include "screen.h"

#include "../../View/Gui/guicontainer.h"

namespace View {
	class Window;
	class Input;
}

namespace Controller {
	class EndMenu : public Screen {
	public:
		EndMenu(bool win);
		void DrawGui(View::Window* context);
		void ProcessInput(Input* input);
		void ProcessReshape(int w, int h);
	private:
		EndMenu(EndMenu& menu);
		View::GuiContainer m_gui;
	};
}