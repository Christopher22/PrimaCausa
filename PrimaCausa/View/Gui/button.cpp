#include "button.h"
#include "label.h"
#include "guielement.h"

#include "../texture.h"
#include "../material.h"
#include "../Utils/vec2d.h"

#include "../../Controller/input.h"
#include "../../Controller/engine.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cassert>
#include <string>

namespace View {

	Button* Button::Load(float x, float y, const std::string& path, std::function<void(const char*)> callback, std::string* payload) {
		return Button::Load(x, y, 0.2f, path, callback, payload);
	}

	Button* Button::Load(float x, float y, float width, const std::string& path, std::function<void(const char*)> callback, std::string* payload) {
		Texture* tex = Texture::Load(path.c_str());
		if ( tex == nullptr ) {
			return nullptr;
		}
		return new Button(x, y, width, tex, callback, payload);
	}

	Button::Button(float x, float y, float width, Texture* tex, std::function<void(const char*)> callback, std::string* payload) 
		: GuiElement(x, y), 
		m_pTexture(tex), 
		m_callback(callback), 
		m_width(width), 
		m_pUserdata(payload),
		m_selected(false),
		m_hasHover(true)
	{
		assert(tex);

		m_pTexture->Enable();
		int tw, th;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
		m_ratio = tw / float(th);
		m_height = m_width / m_ratio;
		m_pTexture->Disable();
	}

	Button::~Button() {
		if ( m_pUserdata ) {
			delete m_pUserdata;
			m_pUserdata = nullptr;
		}

		delete m_pTexture;
		m_pTexture = nullptr;
	}

	void Button::Draw() {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		const float x = GuiElement::GetX(), y = GuiElement::GetY(), w = m_width, h = m_height;
		m_pTexture->Enable();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		
		glBegin(GL_QUADS);
		glTexCoord2i(0, 1);
		glVertex2f(x, y);
		glTexCoord2f(0, 0);
		glVertex2f(x, y + h);
		glTexCoord2i(1, 0);
		glVertex2f(x + w, y + h);
		glTexCoord2i(1, 1);
		glVertex2f(x + w, y);
		glEnd();

		m_pTexture->Disable();

		if ( m_selected && m_hasHover ) {
			glPushMatrix();

			glBegin(GL_QUADS);
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glVertex2f(x, y);
			glVertex2f(x, y + h);
			glVertex2f(x + w, y + h);
			glVertex2f(x + w, y);
			glEnd();

			glPopMatrix();
			glColor4f(1, 1, 1, 1);
		}

		glDisable(GL_BLEND);
	}

	float Button::GetHeight() const {
		return m_height;
	}

	float Button::GetWidth() const {
		return m_width;
	}

	void Button::SetSize(float width, float height) {
		m_width = width;
		m_height = height;
	}

	void Button::SetWidth(float width) {
		m_width = width; m_height = m_width / m_ratio;
	}

	void Button::SetHeight(float height) {
		m_height = height; m_width = m_height* m_ratio;
	}

	bool Button::ProcessInput(Controller::Input* input) {
		float x = float(input->GetX()), y = glutGet(GLUT_WINDOW_HEIGHT) - float(input->GetY());
		if ( input->GetId() == 'M' ) {
			Controller::Input::MouseEvent* mouseInput = dynamic_cast<Controller::Input::MouseEvent*>( input );
			bool is_inside = isInside(x, y);
			if ( mouseInput->GetButton() == Controller::Input::MouseEvent::Button::Left &&
				mouseInput->GetState() == Controller::Input::MouseEvent::State::Down &&
				is_inside ) {
				m_callback(( m_pUserdata ? m_pUserdata->c_str() : nullptr ));
				return true;
			} else if ( isInside(x, y) ) {
				m_selected = true;
			} else {
				m_selected = false;
			}
		}
		return false;
	}

	bool Button::isInside(float x1, float y1) {
		const float x = GuiElement::GetX(), y = GuiElement::GetY();
		return ( x1 > x && x1 < x + m_width && y1 > y && y1 < y + m_height );
	}

	void Button::SetHover(bool hover) {
		m_hasHover = hover;
	}
}