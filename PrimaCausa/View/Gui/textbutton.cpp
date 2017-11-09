#include "textbutton.h"
#include "guielement.h"
#include "button.h"
#include "label.h"

#include "../texture.h"
#include "../../Controller/engine.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>

namespace View {
	TextButton::TextButton(float x, float y, Texture* tex, const char* text, std::function<void(const char*)> callback, std::string* payload, const std::array<float, 3>& textcolor)
		: GuiElement(x, y),
		m_text(text),
		m_label(x, y, text, View::Label::Size::Medium, textcolor),
		m_padding(30.0f)
	{
		int tw, th;
		tex->Enable();
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
		m_ratio = tw / float(th);
		tex->Disable();

		const float labelH = m_label.GetHeight(), labelW = m_label.GetWidth();
		m_width = labelW + 2 * m_padding;
		m_height = m_width / m_ratio;

		const float margin = std::abs(m_height - labelH);
		m_label.SetX(x + m_padding);
		m_label.SetY(y + margin / 2 + labelH*0.35f);
		m_verticalOff = margin / 2 + labelH*0.35f;

		m_pButton = new Button(x, y, m_width, tex, callback, payload);
		m_pButton->SetSize(m_width, m_height);
	}

	bool TextButton::ProcessInput(Controller::Input* input) {
		return m_pButton->ProcessInput(input);
	}

	float TextButton::GetWidth() const {
		return m_pButton->GetWidth();
	}

	float TextButton::GetHeight() const {
		return m_pButton->GetHeight();
	}

	void TextButton::SetWidth(float width) {
		m_pButton->SetWidth(width);
		m_width = width;
		adjustLabel();
	}

	void TextButton::SetHeight(float height) {
		m_pButton->SetHeight(height);
		m_height = height;
		adjustLabel();
	}

	void TextButton::Draw() {
		m_pButton->Draw();
		m_label.Draw();
	}

	TextButton::~TextButton() {
		delete m_pButton;
		m_pButton = nullptr;
	}

	void TextButton::SetPadding(float padding) {
		m_padding = padding;
	}

	float TextButton::GetPadding() const {
		return m_padding;
	}

	void TextButton::SetX(float x) {
		m_x = x;
		m_pButton->SetX(x);
		adjustLabel();
	}

	void TextButton::SetY(float y) {
		m_y = y;
		m_pButton->SetY(y);
		adjustLabel();
	}

	void TextButton::adjustLabel() {
		float margin = std::abs(m_height - m_label.GetHeight());
		float padding = std::abs(m_width - m_label.GetWidth());
		m_label.SetX(m_x + padding / 2);
		m_label.SetY(m_y + margin / 2 + m_label.GetHeight()*0.35f);
	}
}