#pragma once;

#include "guielement.h"

#include <vector>

namespace Controller{
	class Input;
}

namespace View{
	class Window;
	class GuiElement;

	class GuiContainer: public GuiElement {
	public:
		GuiContainer(Window* context, float x = -1, float y = -1, float spacing = 0);
		~GuiContainer();
		void Draw();
		bool ProcessInput(Controller::Input* input);

		void PushElement(GuiElement* element);
		void PackHorizontal(float newSize = -1);
		void PackVertical(float newSize = -1);
		void CenterVertical(float newSize = -1);
		GuiElement* operator[] (unsigned short index);
		
		float GetWidth() const;
		float GetHeight() const;
		void SetWidth(float width);
		void SetHeight(float height);
		void SetHover(bool hover);
		void SetSpacing(float spacing);
	private:
		std::vector<GuiElement*> m_elements;
		Window* m_pContext;
		float m_spacing, m_size;
	};

	

}