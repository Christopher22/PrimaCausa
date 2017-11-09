#pragma once

#include "drawable.h"

namespace View {
	class Texture;
	class Material;

	class Hex : public Drawable {
	public:
		static const float Size;
		static const float INNER_RATIO;

		Hex(Texture* tex, Material* mat);
		~Hex();
		void Draw(unsigned short x, unsigned short y, unsigned short layer, bool selected);
	private:
		Hex(Hex& copy);

		Texture* m_pTex;
		const float m_texD, m_texS;
	};
}