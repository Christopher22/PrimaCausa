#include "label.h"
#include "guielement.h"
#include "../Utils/linearscale.h"
#include "../../Controller/engine.h"
#include <string>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <array>

namespace View {
	Label::Label(float x, float y, const std::string& text, Size size, const std::array<float, 3>& color) : GuiElement(x, y), m_text(text), m_size(size), m_color(color) {}

	void Label::Draw() {
		glColor3fv(m_color.data());
		glRasterPos2f(GuiElement::GetX(), GuiElement::GetY());
		glutBitmapString((void*) m_size, (unsigned char*) m_text.c_str());
		glColor3f(1.0f, 1.0f, 1.0f);
	}

	float Label::GetHeight() const {
		return float(glutBitmapHeight((void*)m_size));
	}

	float Label::GetWidth() const {
		return float(glutBitmapLength((void*)m_size, (unsigned char *)m_text.c_str()));
	}

	void Label::SetText(const char* value) {
		m_text.clear();
		m_text.append(value);
	}

	void Label::SetWidth(float width) {}
	void Label::SetHeight(float height) {}

	bool Label::ProcessInput(Controller::Input* input) {
		return false;
	}

	void Label::SetColor(const std::array<float, 3> color) {
		m_color = color;
	}

	void Label::SetSize(Size size){
		m_size = size;
	}
}