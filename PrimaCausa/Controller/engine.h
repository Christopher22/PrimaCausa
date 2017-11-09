#pragma once

#include <stack>
#include <iostream>
#include <cstdio>

namespace View {
	class Window;
	class Mesh;
}

namespace FMOD {
	class System;
	class Sound;
}

namespace Controller {
	class Screen;

	class Engine {
	public:
		static bool Initialise(int argc, char* args[], unsigned short width, unsigned short height, unsigned short fps);
		static void DestroyInstance();
		static void Exit();

		static Engine* GetInstance();
		View::Window* GetContext();
		~Engine();
		void Run();
		void PushScreen(Screen*);
		bool PopScreen(bool gently = true);
		Screen* GetScreen() const;
		FMOD::System* GetAudio() const;
		unsigned short GetNumScreens() const;
		
		template<typename ... Args>
		static inline void Log(const char* format, Args ... args) {
			std::printf(format, args ...);
		}

	private:
		Engine(View::Window* w, FMOD::System* audio, unsigned short fps);
		Engine(Engine& copy);
		void pop();

		static void render();
		static void reshape(int w,int h);
		static void runMainLoop(int val);
		static void mouseHandler(int button, int state, int x, int y);
		static void kbSplUpHandler(int key, int x, int y);
		static void kbUpHandler(unsigned char key, int x, int y);
		static void motionHandler(int x, int y);
		static void passiveMotionhandler(int x, int y);
		static void scrollHandler(int wheel, int direction, int x, int y);

		static Engine* instance;

		std::stack<Screen*> m_screens;
		View::Window* m_pContext;
		unsigned short m_fps, m_gentleRemove;
		bool m_shouldRender;
		FMOD::System* m_pAudio;
	};
}