#pragma once;

#include "guielement.h"
#include "label.h"

#include <string>
#include <functional>

namespace View{
	class Texture;
	class Button;

	class TextButton : public GuiElement {
	public:
		TextButton(float x, float y, Texture* tex, const char* text, std::function<void(const char*)> callback, std::string* payload = nullptr, const std::array<float, 3>& textcolor = { { 0.f, 0.f, 0.f } });
		~TextButton();

		void Draw();
		bool ProcessInput(Controller::Input* input);

		float GetWidth() const;
		float GetHeight() const;
		float GetPadding() const;

		void SetWidth(float width);
		void SetHeight(float height);
		void SetX(float x);
		void SetY(float y);
		void SetPadding(float padding);
	private:
		void adjustLabel();

		const std::string m_text;
		Button* m_pButton;
		Label m_label;
		float m_padding, m_ratio, m_width, m_height, m_verticalOff;
	};
}