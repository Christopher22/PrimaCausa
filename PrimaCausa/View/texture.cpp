#include "texture.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SOIL.h>

namespace View {
	Texture::Texture(GLuint tex) : m_tex(tex) {}

	Texture::~Texture() {
		glDeleteTextures(1, &m_tex);
	}

	Texture* Texture::Load(const char* path) {
		int width, height;
		unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);

		if ( image ) {
			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
			SOIL_free_image_data(image);
			return new Texture(tex);
		} else {
			return nullptr;
		}
	}

	const GLuint* Texture::GetTexture() const {
		return &m_tex;
	}

	void Texture::Enable() {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Filtering
	}

	void Texture::Disable() {
		glDisable(GL_TEXTURE_2D);
	}
}