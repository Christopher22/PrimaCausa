#pragma once

#include "screen.h"

#include "../../View/Gui/guicontainer.h"
#include "../../View/Gui/button.h"

namespace View {
	class Window;
	class Input;
}

namespace FMOD {
	class Sound;
}

namespace Controller {
	class StartMenu : public Screen {
	public:
		StartMenu();
		~StartMenu();
		void DrawGui(View::Window* context);
		void ProcessInput(Input* input);
		void ProcessReshape(int w, int h);
	private:
		StartMenu(StartMenu& menu);

		View::GuiContainer m_buttons;
		View::Button* m_logo;
		FMOD::Sound* m_pBackgroundMusic;
	};
}