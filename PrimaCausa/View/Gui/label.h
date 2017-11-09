#pragma once;

#include "guielement.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <string>
#include <array>

namespace View{

	class Label : public GuiElement {
	public:
		enum class Size: int {
			Small = (int)GLUT_BITMAP_HELVETICA_10,
			Medium = (int)GLUT_BITMAP_HELVETICA_12,
			Large = (int)GLUT_BITMAP_HELVETICA_18
		};

		Label(float x, float y, const std::string& text, Size size = Size::Medium, const std::array<float, 3>& color = { { 1.0f, 1.0f, 1.0f } });

		void Draw();
		bool ProcessInput(Controller::Input* input);

		float GetHeight() const;
		float GetWidth() const;

		void SetWidth(float width);
		void SetHeight(float height);
		void SetText(const char* value);
		void SetColor(const std::array<float, 3> color);
		void SetSize(Size size);
	private:
		std::string m_text;
		Size m_size;
		std::array<float, 3> m_color;
	};
}