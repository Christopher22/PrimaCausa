#include "guielement.h"
#include "../material.h"

namespace View {
	GuiElement::GuiElement() : m_x(0), m_y(0) {}
	GuiElement::GuiElement(float x, float y) : m_x(x), m_y(y) {}
	GuiElement::~GuiElement() {}

	float GuiElement::GetX() const {
		return m_x;
	}

	float GuiElement::GetY() const {
		return m_y;
	}

	void GuiElement::SetX(float x) {
		m_x = x;
	}

	void GuiElement::SetY(float y) {
		m_y = y;
	}

	void GuiElement::SetPosition(float x, float y) {
		this->SetX(x);
		this->SetY(y);
	}
}