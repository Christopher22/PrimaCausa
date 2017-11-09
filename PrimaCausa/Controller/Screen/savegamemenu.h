#pragma once

#include "screen.h"

#include "../../View/Gui/guicontainer.h"

namespace View {
	class Window;
	class Input;
}

namespace Controller {
	class SaveGameMenu : public Screen {
	public:
		const static char* SAVEGAME_PATH;

		SaveGameMenu(bool save);
		void DrawGui(View::Window* context);
		void ProcessInput(Input* input);
		void ProcessReshape(int w, int h);
	private:
		SaveGameMenu(SaveGameMenu& menu);
		void processSlot(unsigned char slot);

		static bool fileExist(const char* fileName);
		static bool fileExist(unsigned char slot);

		View::GuiContainer m_gui;
		const bool m_saveGame;
	};
}