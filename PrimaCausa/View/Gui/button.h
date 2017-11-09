#pragma once;

#include "guielement.h"
#include "../Utils/vec2d.h"

#include <functional>
#include <string>

namespace View {
	class Label;
	class Texture;

	class Button : public GuiElement {
	public:
		static Button* Load(float x, float y, float width, const std::string& path, std::function<void(const char*)> callback, std::string* payload = nullptr);
		static Button* Load(float x, float y, const std::string& path, std::function<void(const char*)> callback, std::string* payload = nullptr);

		Button(float x, float y, float width, Texture* tex, std::function<void(const char*)> callback, std::string* payload = nullptr);
		~Button();

		void Draw();
		bool ProcessInput(Controller::Input* input);
		void SetSize(float width, float height);
		void SetHover(bool hover);

		float GetWidth() const;
		float GetHeight() const;

		void SetHeight(float height);
		void SetWidth(float width);
	private:
		bool isInside(float x, float y);

		Label* m_pLabel;
		std::string* m_pUserdata;
		Texture* m_pTexture;
		float m_width, m_height, m_ratio;
		std::function<void(const char*)> m_callback;
		bool m_selected, m_hasHover;
	};
}