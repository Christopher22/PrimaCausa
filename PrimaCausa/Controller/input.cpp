#include "input.h"
#include "engine.h"

#include "../View/Utils/vec2d.h"
#include "../View/Utils/linearscale.h"

namespace Controller {
	Input::Input(const unsigned short x, const unsigned short y, const State state) : m_x(x), m_y(y), m_state(state) {}

	unsigned short Input::GetX() const {
		return m_x;
	}

	unsigned short Input::GetY() const {
		return m_y;
	}

	View::Vec2d<float> Input::GetPos(unsigned short width, unsigned short height) const {
		return View::Vec2d<float>(
			View::LinearScale::Scale(0.0f, float(width), -1.0f, 1.0f, m_x),
			View::LinearScale::Scale(0.0f, float(height), -1.0f, 1.0f, float(height - m_y))
		);
	}

	Input::State Input::GetState() const {
		return m_state;
	}

	Input::MouseEvent::MouseEvent(const Button button, const State state, const unsigned short x, const unsigned short y) : Input(x, y, state), m_button(button) {}

	Input::MouseEvent::Button Input::MouseEvent::GetButton() const {
		return m_button;
	}

	Input::KeyboardEvent::KeyboardEvent(const Key key, const State state, const unsigned short x, const unsigned short y) : Input(x, y, state), m_key(key) {}

	Input::KeyboardEvent::Key Input::KeyboardEvent::GetKey() const {
		return m_key;
	}

	char Input::MouseEvent::GetId()const {
		return 'M';
	}

	char Input::KeyboardEvent::GetId()const {
		return 'K';
	}
}