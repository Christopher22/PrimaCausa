#pragma once

#include "drawable.h"
#include "Utils/vec2d.h"
#include "Utils/Vec3d.h"

#include <vector>

namespace View {
	class Window;

	class Mesh : public Drawable {
	public:
		static const float SIZE;
		static const float INNER_RATIO;

		static Mesh* Load(const char* filepath, Material* mat);
		void Draw(unsigned short xIn, unsigned short yIn, unsigned short layer, bool selected);
		void Translate(float x,float y,float z);
	private:
		Mesh(unsigned int size, Material* mat);
		Mesh(Mesh& copy);

		void add(const Vec3d<float>& vertex, const Vec3d<float>& normal, const Vec2d<float>& uv);
		void add(const Vec3d<float>& vertex, const Vec3d<float>& normal);
		void add(const Vec3d<float>& vertex);
		
		std::vector<Vec3d<float>> m_vertices;
		std::vector<Vec3d<float>> m_vertexNormals;
		std::vector<Vec2d<float>> m_vertexTextureCoords;
		float m_Tx, m_Ty, m_Tz;
	};
}


