#pragma once;

namespace View {
	class Material;
	class ColorProfile;

	class Drawable {
	public:
		Drawable(Material* mat);
		~Drawable();
		virtual void Draw(unsigned short xIn, unsigned short yIn, unsigned short layer, bool selected) = 0;

		Material* GetMaterial() const;
		void SetColor(const ColorProfile& color);
		ColorProfile* GetColor() const;
	private:
		Material* m_pMat;
		ColorProfile* m_pColorProfile;
	};
}