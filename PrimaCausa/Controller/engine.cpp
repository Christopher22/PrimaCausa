#include "engine.h"
#include "input.h"

#include "Screen/screen.h"
#include "Screen/startmenu.h"

#include "../View/window.h"
#include "../View/mesh.h"
#include "../View/texture.h"

#include "../View/Gui/button.h"
#include "../View/Gui/textbutton.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "fmod.hpp"

#include <cassert>
#include <array>

namespace Controller {
	Engine* Engine::instance = nullptr;

	Engine::Engine(View::Window* w, FMOD::System* audio, unsigned short fps) : m_pContext(w), m_fps(fps), m_pAudio(audio), m_shouldRender(true), m_gentleRemove(0) {
		assert(w);
		assert(audio);
	}

	Engine::~Engine() {
		while ( m_screens.size() > 0 ) {
			delete m_screens.top();
			m_screens.pop();
		}

		delete m_pContext;
		m_pContext = nullptr;

		m_pAudio->release();
		m_pAudio = nullptr;
	}

	bool Engine::Initialise(int argc, char* args[], unsigned short width, unsigned short height, unsigned short fps) {
		assert(Engine::instance == nullptr);

		// Load audio
		FMOD::System* fmodHandle = nullptr;
		if ( FMOD::System_Create(&fmodHandle) != FMOD_OK || fmodHandle->init(512, FMOD_INIT_NORMAL, 0) != FMOD_OK ) {
			Engine::Log("[ERROR] Audio initializing failed!");
			return false;
		}

		// Initialize freeGLUT and create window
		glutInit(&argc, args);
		glutInitContextVersion(2, 1);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

		View::Window* window = View::Window::Initialise(width, height);
		if ( !window ) {
			return false;
		}

		glutDisplayFunc(&Engine::render);
		glutReshapeFunc(&Engine::reshape);
		glutMouseFunc(&Engine::mouseHandler);
		glutSpecialUpFunc(&Engine::kbSplUpHandler);
		glutKeyboardUpFunc(&Engine::kbUpHandler);
		glutMotionFunc(&Engine::motionHandler);
		glutMouseWheelFunc(&Engine::scrollHandler);
		glutPassiveMotionFunc(&Engine::passiveMotionhandler);

		// Handles FreeGLUT bug: Timer gets called after main loop exit!
		glutCloseFunc([] () {
			Engine::instance->m_shouldRender = false;
		});

		Engine::instance = new Engine(window, fmodHandle, fps);
		return true;
	}

	Engine* Engine::GetInstance() {
		assert(Engine::instance);
		return Engine::instance;
	}

	void Engine::Run() {
		this->PushScreen(new StartMenu());

		// Start the main loop
		glutTimerFunc(1000 / m_fps, &Engine::runMainLoop, 0);
		glutMainLoop();
	}

	void Engine::Exit() {
		glutLeaveMainLoop();
	}

	void Engine::PushScreen(Screen* screen) {
		assert(screen);
		m_screens.push(screen);
	}

	bool Engine::PopScreen(bool gently) {
		if ( m_screens.size() > 1 && gently ) {
			++m_gentleRemove;
			return true;
		} else if ( m_screens.size() > 1 ) {
			pop();
			return true;
		} else {
			return false;
		}
	}

	unsigned short Engine::GetNumScreens() const {
		return m_screens.size();
	}

	void Engine::runMainLoop(int val) {
		for ( ; Engine::instance->m_gentleRemove > 0; Engine::instance->m_gentleRemove-- ) {
			Engine::instance->pop();
		}

		Engine::render();
		Engine::instance->m_pAudio->update();
		glutTimerFunc(1000 / Engine::instance->m_fps, &Engine::runMainLoop, val);
	}

	void Engine::render() {
		if ( Engine::instance->m_shouldRender ) {
			glMatrixMode(GL_MODELVIEW);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			Engine::instance->m_pContext->SetupCamera();
			Engine::instance->m_screens.top()->Draw(Engine::instance->m_pContext);

			Engine::instance->m_pContext->Start2DMode();
			Engine::instance->m_screens.top()->DrawGui(Engine::instance->m_pContext);
			Engine::instance->m_pContext->EndMode();
			glutSwapBuffers();
		}
	}

	void Engine::reshape(int w, int h) {
		// Prevent a divide by zero, when window is too short
		// (you cant make a window of zero width).
		if ( h == 0 )
			h = 1;
		float ratio = float(1.0 * w / h);

		// Use the Projection Matrix
		glMatrixMode(GL_PROJECTION);

		// Reset Matrix
		glLoadIdentity();

		// Set the viewport to be the entire window
		glViewport(0, 0, w, h);

		// Set the correct perspective.
		gluPerspective(45.0f, ratio, 1.0f, 10.0f);

		// Get Back to the Modelview
		glMatrixMode(GL_MODELVIEW);
		Engine::instance->GetContext()->SetHeight(h);
		Engine::instance->GetContext()->SetWidth(w);
		Engine::instance->m_pContext->SetupCamera();
		Engine::instance->m_screens.top()->ProcessReshape(w, h);
	}

	void Engine::mouseHandler(int button, int state, int x, int y) {
		auto eventButton = ( button == GLUT_LEFT_BUTTON ) ? Input::MouseEvent::Button::Left : Input::MouseEvent::Button::Right;
		auto eventState = ( state == GLUT_UP ) ? Input::State::Up : Input::State::Down;
		Input::MouseEvent tmp(eventButton, eventState, x, y);
		Engine::instance->m_screens.top()->ProcessInput(&tmp);
	}

	void Engine::kbSplUpHandler(int key, int x, int y) {
		Input::KeyboardEvent::Key relaykey;
		switch ( key ) {
			case GLUT_KEY_LEFT: relaykey = Input::KeyboardEvent::Key::Left; break;
			case GLUT_KEY_RIGHT: relaykey = Input::KeyboardEvent::Key::Right; break;
			case GLUT_KEY_UP: relaykey = Input::KeyboardEvent::Key::Up; break;
			case GLUT_KEY_DOWN: relaykey = Input::KeyboardEvent::Key::Down; break;
			default:
				return;
		}
		Input::KeyboardEvent tmp(relaykey, Input::State::Up, x, y);
		Engine::instance->m_screens.top()->ProcessInput(&tmp);
	}

	void Engine::kbUpHandler(unsigned char key, int x, int y) {
		if ( key == ' ' ) {
			Input::KeyboardEvent tmp(Input::KeyboardEvent::Key::Space, Input::State::Up, x, y);
			Engine::instance->m_screens.top()->ProcessInput(&tmp);
		} else if ( key == 'g' ) {
			Input::KeyboardEvent tmp(Input::KeyboardEvent::Key::GodMode, Input::State::Up, x, y);
			Engine::instance->m_screens.top()->ProcessInput(&tmp);
		} else if ( key == 'f' ) {
			Engine::instance->m_pContext->ToggleFullscreen();
		}
	}

	void Engine::motionHandler(int x, int y) {
		Input::MouseEvent tmp(Input::MouseEvent::Button::None, Input::MouseEvent::State::Down, x, y);
		Engine::instance->m_screens.top()->ProcessInput(&tmp);
	}

	void Engine::passiveMotionhandler(int x, int y) {
		Input::MouseEvent tmp(Input::MouseEvent::Button::None, Input::MouseEvent::State::Up, x, y);
		Engine::instance->m_screens.top()->ProcessInput(&tmp);
	}

	void Engine::scrollHandler(int wheel, int direction, int x, int y) {
		Input::MouseEvent tmp(
			( direction == 1 ) ? Input::MouseEvent::Button::WheelUp : Input::MouseEvent::Button::WheelDown,
			Input::MouseEvent::State::Down,
			x, y);
		Engine::instance->m_screens.top()->ProcessInput(&tmp);
	}

	void Engine::DestroyInstance() {
		delete Engine::instance;
		Engine::instance = nullptr;
	}

	View::Window* Engine::GetContext() {
		return m_pContext;
	}

	Screen* Engine::GetScreen() const {
		return m_screens.top();
	}

	FMOD::System* Engine::GetAudio() const {
		return m_pAudio;
	}

	void Engine::pop() {
		delete m_screens.top();
		m_screens.pop();

		// Send resize signal for the case fullscreen was toggled while not on top
		m_screens.top()->ProcessReshape(Engine::instance->m_pContext->GetWidth(), Engine::instance->m_pContext->GetHeight());
	}
}