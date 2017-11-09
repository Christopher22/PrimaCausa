#pragma once;

namespace Controller{
	class Input;
}

namespace View{
	class Material;

	class GuiElement {
	public:
		GuiElement();
		GuiElement(float x, float y);
		virtual ~GuiElement();

		virtual void Draw() = 0 ;
		virtual bool ProcessInput(Controller::Input* input) = 0;

		float GetX() const;
		float GetY() const;
		virtual float GetWidth() const = 0;
		virtual float GetHeight() const = 0;

		virtual void SetX(float x);
		virtual void SetY(float y);
		virtual void SetWidth(float width) = 0;
		virtual void SetHeight(float height) = 0;
		void SetPosition(float x, float y);
	protected:
		float m_x, m_y;
	};

}