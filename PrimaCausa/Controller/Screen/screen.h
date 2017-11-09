#pragma once;

namespace View {
	class Window;
}

namespace Controller {
	class Input;

	class Screen {
	public:
		virtual ~Screen() {}
		virtual void Draw(View::Window* context) {};
		virtual void DrawGui(View::Window* context) = 0;
		virtual void ProcessInput(Input* input) = 0;
		virtual void ProcessReshape(int w, int h) = 0;
	};
}

