#include "material.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <algorithm>

namespace View {
	Material::Material(const std::array<float, 4>& ambient, const std::array<float, 4>& diffuse, const std::array<float, 4>& specular, const std::array<float, 3>& emissive, float shininess)
		: m_isDull(false), 
		m_ambient(ambient), 
		m_shininess(shininess), 
		m_diffuse(diffuse), 
		m_emissive(emissive), 
		m_specular(specular) 
	{}

	Material::Material() 
		: m_isDull(false), 
		m_ambient({ { 1.0f, 1.0f, 1.0f, 1.0f } }), 
		m_shininess(5.0f), 
		m_diffuse({ { 1.0f, 1.0f, 1.0f, 1.0f } }), 
		m_emissive({ { 0.1f, 0.1f, 0.1f } }), 
		m_specular({ { 1.0f, 1.0f, 1.0f, 1.0f } }) 
	{}

	void Material::Apply(bool dull) {
		// Efficient inplace modification on change
		if ( dull && !m_isDull ) {
			m_isDull = true;
			std::transform(m_ambient.begin(), m_ambient.end(), m_ambient.begin(), [] (float elem) { return elem / 2; });
			std::transform(m_diffuse.begin(), m_diffuse.end(), m_diffuse.begin(), [] (float elem) { return elem / 2; });
			std::transform(m_specular.begin(), m_specular.end(), m_specular.begin(), [] (float elem) { return elem / 2; });
			std::transform(m_emissive.begin(), m_emissive.end(), m_emissive.begin(), [] (float elem) { return elem / 2; });
		} else if ( !dull && m_isDull ) {
			m_isDull = false;
			std::transform(m_ambient.begin(), m_ambient.end(), m_ambient.begin(), [] (float elem) { return elem * 2; });
			std::transform(m_diffuse.begin(), m_diffuse.end(), m_diffuse.begin(), [] (float elem) { return elem * 2; });
			std::transform(m_specular.begin(), m_specular.end(), m_specular.begin(), [] (float elem) { return elem * 2; });
			std::transform(m_emissive.begin(), m_emissive.end(), m_emissive.begin(), [] (float elem) { return elem * 2; });
		}

		glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient.data());
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse.data());
		glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular.data());
		glMaterialfv(GL_FRONT, GL_SHININESS, &m_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, m_emissive.data());
	}
}