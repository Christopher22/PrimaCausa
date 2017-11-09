#pragma once

#include <GL/freeglut.h>

namespace View {
	class Texture {
	public:
		static Texture* Load(const char* path);
		~Texture();

		void Enable();
		void Disable();
		const GLuint* GetTexture() const;
	private:
		Texture(GLuint tex);
		Texture(Texture& copy);

		GLuint m_tex;
	};
}

