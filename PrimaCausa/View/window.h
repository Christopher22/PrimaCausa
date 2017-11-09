#pragma once

#include "Utils/Vec3d.h"
#include "Utils/vec2d.h"

namespace View {
	class Window
	{
	public:
		const static float AMBIENT_LIGHT[];
		const static float DIFFUSE_LIGHT[];
		const static float SPECULAR_LIGHT[];
		const static float LIGHT_POSITION[];

		static Window* Initialise(unsigned short width, unsigned short height);

		~Window();
		void SetupCamera();
		void SetCameraPosition(Vec2d<float> newPos);
		Vec2d<float> GetCameraPosition() const ;
		void MoveCamera(Vec2d<float> newPos);
		void MoveCamera(Vec3d<float> newPos);
		void RotateCamera(float angle);
		void MoveCamera(char d, float mag);
		float GetCameraDistance() const;
		float GetCameraAngle() const;
		
		void SetHeight(unsigned int h);
		unsigned short GetHeight() const;

		void SetWidth(unsigned int w);
		unsigned short GetWidth() const;

		void Start2DMode();
		void EndMode();
		void ToggleFullscreen();
	private:
		Window(int windowId, unsigned short width, unsigned short height);
		Window(Window& copy);
		float toRadians(float angle);

		Vec2d<unsigned short>* m_oldWindowSize;
		unsigned short m_width, m_height;
		int m_winHandle;
		float m_camDistance, m_camAngle, m_panX, m_panY;
	};

}

