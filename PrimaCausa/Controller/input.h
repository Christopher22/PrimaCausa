#pragma once

#include "../View/Utils/vec2d.h"

namespace Controller {
	class Input {
	public:
		enum class State {
			Down,
			Up
		};

		class MouseEvent;
		class KeyboardEvent;

		Input(const unsigned short x, const unsigned short y, const State state);
		virtual char GetId() const = 0;
		unsigned short GetX() const;
		unsigned short GetY() const;
		View::Vec2d<float> GetPos(unsigned short width, unsigned short height) const;
		State GetState() const;
	private:
		const unsigned short m_x, m_y;
		const State m_state;
	};

	class Input::MouseEvent : public Input {
	public:
		enum class Button {
			Left,
			Right,
			None,
			WheelUp,
			WheelDown
		};

		virtual char GetId() const;
		MouseEvent(const Button button, const State state, const unsigned short x, const unsigned short y);
		Button GetButton() const;
	private:
		Button m_button;
	};

	class Input::KeyboardEvent : public Input {
	public:
		enum class Key {
			Up,
			Down,
			Left,
			Right,
			Space,
			GodMode
		};

		virtual char GetId() const;
		KeyboardEvent(const Key button, const State state, const unsigned short x, const unsigned short y);
		Key GetKey() const;
	private:
		Key m_key;
	};
}