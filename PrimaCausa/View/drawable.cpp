#include "drawable.h"
#include "material.h"
#include "colorprofile.h"

namespace View {
	Drawable::Drawable(Material* mat) : m_pMat(mat), m_pColorProfile(nullptr) {}

	Drawable::~Drawable() {
		delete m_pMat;
		m_pMat = nullptr;

		delete m_pColorProfile;
		m_pColorProfile = nullptr;
	}

	Material* Drawable::GetMaterial() const {
		return m_pMat;
	}

	void Drawable::SetColor(const ColorProfile& color) {
		m_pColorProfile = new ColorProfile(color);
	}

	ColorProfile* Drawable::GetColor() const {
		return m_pColorProfile;
	}
}