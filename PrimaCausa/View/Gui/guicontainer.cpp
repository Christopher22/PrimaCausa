#include "guicontainer.h"
#include "guielement.h"
#include "button.h"
#include "../window.h"

#include <algorithm>

namespace View {
	GuiContainer::GuiContainer(Window* context, float x, float y, float spacing) 
		: GuiElement(( x >= 0 ? x : context->GetWidth() / 2 ), 
		( y >= 0 ? y : context->GetHeight() / 2 )), 
		m_pContext(context), 
		m_spacing(spacing), 
		m_size(100) 
	{}

	void GuiContainer::PushElement(GuiElement* element) {
		if ( element ) {
			m_elements.push_back(element);
		}
	}

	void GuiContainer::Draw() {
		std::for_each(m_elements.begin(), m_elements.end(), [] (GuiElement* elem) {
			elem->Draw();
		});
	}

	GuiContainer::~GuiContainer() {
		std::for_each(m_elements.begin(), m_elements.end(), [] (GuiElement* elem) {
			delete elem;
			elem = nullptr;
		});

		m_elements.clear();
		m_pContext = nullptr;
	}

	bool GuiContainer::ProcessInput(Controller::Input* input) {
		bool consumed = false;
		// Warning: Take care of removal in loop
		for ( auto i = m_elements.begin(); i < m_elements.end(); i++ ) {
			if ( consumed = ( *i )->ProcessInput(input) || consumed ) {
				break;
			}
		}
		return consumed;
	}

	void GuiContainer::PackHorizontal(float newSize) {
		if ( newSize >= 0 ) {
			m_size = newSize;
		}

		const unsigned short num = m_elements.size();
		const float item_size = ( m_size + ( num * m_spacing ) ) / num;

		for ( unsigned short i = 0; i < num; i++ ) {
			m_elements[i]->SetX(m_x + item_size * i);
			m_elements[i]->SetY(m_y);
			m_elements[i]->SetWidth(item_size * 0.9f);
		}
	}

	void GuiContainer::PackVertical(float newSize) {
		if ( newSize >= 0 ) {
			m_size = newSize;
		}

		const unsigned short num = m_elements.size();
		const float item_size = ( m_size + ( num * m_spacing ) ) / num;

		for ( unsigned short i = 0; i < num; i++ ) {
			const auto rev = num - 1 - i;
			m_elements[rev]->SetHeight(item_size * 0.9f - m_spacing);
			m_elements[rev]->SetX(m_x);
			m_elements[rev]->SetY(item_size * i + m_y);
		}
	}

	float GuiContainer::GetWidth() const {
		auto maxWidth = *std::max_element(m_elements.begin(), m_elements.end(), [] (const GuiElement* a, const GuiElement* b) {
			return a->GetWidth() < b->GetWidth();
		});

		return !m_elements.empty() ? ( m_elements.size() * ( maxWidth->GetWidth() + m_spacing ) ) : 0;
	}

	float GuiContainer::GetHeight() const {
		auto maxHeight = *std::max_element(m_elements.begin(), m_elements.end(), [] (const GuiElement* a, const GuiElement* b) {
			return a->GetHeight() < b->GetHeight();
		});

		return !m_elements.empty() ? ( m_elements.size() * ( maxHeight->GetHeight() + m_spacing ) ) : 0;
	}

	void GuiContainer::SetWidth(float width) {
		m_size = width;
	}

	void GuiContainer::SetHeight(float height) {
		m_size = height;
	}

	void GuiContainer::CenterVertical(float newSize) {
		if ( newSize >= 0 ) {
			m_size = newSize;
		}

		float height = 0;
		std::for_each(m_elements.begin(), m_elements.end(), [&] (GuiElement* elem) {
			elem->SetWidth(m_size);
			height += float(elem->GetHeight() + m_spacing);
		});

		m_x = m_pContext->GetWidth() / 2.0f - GetWidth() / 2.0f;
		m_y = m_pContext->GetHeight() / 2.0f + height / 2.0f;

		float current = m_y;
		for (unsigned int i = 0; i < m_elements.size(); i++) {
			m_elements[i]->SetX(m_pContext->GetWidth() / 2 - m_elements[i]->GetWidth() / 2);

			current -= m_elements[i]->GetHeight();
			m_elements[i]->SetY(current);
			current -= m_spacing;
		}
	}

	void GuiContainer::SetHover(bool hover) {
		for each ( GuiElement* elem in m_elements ) {
			Button* b = dynamic_cast<Button*>( elem );
			if ( b ) {
				b->SetHover(hover);
			}
		}
	}

	void GuiContainer::SetSpacing(float spacing) {
		m_spacing = spacing;
	}

	GuiElement* GuiContainer::operator[] (unsigned short index) {
		return m_elements[index];
	}
}