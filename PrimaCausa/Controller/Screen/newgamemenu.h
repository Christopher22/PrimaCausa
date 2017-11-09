#pragma once

#include "screen.h"

#include "../../View/Gui/guicontainer.h"

#include <map>
#include <string>

namespace View {
	class Window;
	class Input;
}

namespace Controller {
	class NewGameMenu : public Screen {
	public:
		NewGameMenu();
		void DrawGui(View::Window* context);
		void ProcessInput(Input* input);
		void ProcessReshape(int w, int h);
		static std::map<std::string, std::string> GetAvailableMaps();
	private:
		NewGameMenu(NewGameMenu& menu);

		View::GuiContainer m_buttons;
	};
}