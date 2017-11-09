#include "window.h"

#include "../Controller/engine.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>

static const float PI = 3.14159265358979f;

namespace View {
	const float Window::AMBIENT_LIGHT[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const float Window::DIFFUSE_LIGHT[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float Window::SPECULAR_LIGHT[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float Window::LIGHT_POSITION[] = { 0.0f, 0.5f, 0.5f };

	Window::Window(int windowId, unsigned short width, unsigned short height) : m_winHandle(windowId), m_width(width), m_height(height), m_camDistance(3), m_camAngle(-30), m_oldWindowSize(nullptr) {}

	Window::~Window() {
		if ( m_oldWindowSize ) {
			delete m_oldWindowSize;
			m_oldWindowSize = nullptr;
		}
	}

	Window* Window::Initialise(unsigned short width, unsigned short height) {
		//Create Double Buffered Window
		glutInitDisplayMode(GLUT_DOUBLE);
		glutInitWindowSize(width, height);
		int windowId = glutCreateWindow("Prima Causa");

		width = glutGet(GLUT_WINDOW_WIDTH);
		height = glutGet(GLUT_WINDOW_HEIGHT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, (float) width / (float) height, 1.0f, 10.0f);

		//Initialize Modelview Matrix		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);

		//Check for error
		GLenum error = glGetError();
		if ( error != GL_NO_ERROR ) {
			return nullptr;
		}

		return new Window(windowId, width, height);
	}

	unsigned short Window::GetHeight() const {
		return m_height;
	}

	unsigned short Window::GetWidth() const {
		return m_width;
	}

	void Window::SetCameraPosition(Vec2d<float> newPos) {
		m_panX = newPos.GetX();
		m_panY = newPos.GetY();
	}

	void Window::MoveCamera(Vec3d<float> moveVector) {
		m_panX += moveVector.GetX();
		m_panY += moveVector.GetY();
	}

	void Window::MoveCamera(Vec2d<float> moveVector) {
		m_panX += moveVector.GetX();
		m_panY += moveVector.GetY();
	}

	void Window::SetupCamera() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, m_width / m_height, 1.0f, 10.0f);

		const float eyeX = m_panX;
		const float eyeY = m_panY + m_camDistance*sin(toRadians(m_camAngle));
		const float eyeZ = m_camDistance * cos(toRadians(m_camAngle));

		gluLookAt(
			eyeX, //eye X
			eyeY, //eye Y
			eyeZ, //eye Y
			m_panX, //target X
			m_panY, //target Y
			0,               //target Z
			0,               //not up axis  
			1,               //up axis
			0                //not up axis
		);

		glPushMatrix();
		glTranslatef(LIGHT_POSITION[0], LIGHT_POSITION[1], LIGHT_POSITION[2]);
		glPopMatrix();

		glLightfv(GL_LIGHT0, GL_AMBIENT, AMBIENT_LIGHT);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, DIFFUSE_LIGHT);
		glLightfv(GL_LIGHT0, GL_SPECULAR, SPECULAR_LIGHT);
		glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_POSITION);
		glEnable(GL_LIGHT0);
	}

	Vec2d<float> Window::GetCameraPosition() const {
		return Vec2d<float>(m_panX, m_panY);
	}

	void Window::MoveCamera(char d, float mag) {
		switch ( d ) {
			case 'u':
				m_panY += mag;
				break;
			case 'd':
				m_panY -= mag;
				break;
			case 'l':
				m_panX -= mag;
				break;
			case 'r':
				m_panX += mag;
				break;
			case 'i':{
				float newDis = m_camDistance - mag;
				if ( newDis > 1.5f ) {
					m_camDistance = newDis;
				}
				break;
			}
			case 'o':{
				float newDis = m_camDistance + mag;
				if ( newDis < 9 ) {
					m_camDistance = newDis;
				}
				break;
			}
			default:
				break;
		}
	}

	void Window::RotateCamera(float angle) {
		m_camAngle = angle;
	}

	float Window::GetCameraDistance() const {
		return m_camDistance;
	}

	float Window::GetCameraAngle() const {
		return m_camAngle;
	}

	float Window::toRadians(float angle) {
		return angle * ( PI / 180.0f );
	}

	void Window::Start2DMode() {
		glPushMatrix();
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
	}

	void Window::EndMode() {
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	void Window::SetHeight(unsigned int h) {
		m_height = h;
	}
	void Window::SetWidth(unsigned int w) {
		m_width = w;
	}

	void Window::ToggleFullscreen() {
		if ( m_oldWindowSize ) {
			glutReshapeWindow(m_oldWindowSize->GetX(), m_oldWindowSize->GetY());
			glutPositionWindow(0, 0);

			delete m_oldWindowSize;
			m_oldWindowSize = nullptr;
		} else {
			m_oldWindowSize = new Vec2d<unsigned short>(GetWidth(), GetHeight());
			glutFullScreen();
		}
	}
}
