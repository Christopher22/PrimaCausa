#pragma once;

#include <array>

namespace View{
	class Material {
	public:
		static Material* Load(const char* path);

		Material(const std::array<float, 4>& ambient, const std::array<float, 4>& diffuse, const std::array<float, 4>& specular, const std::array<float, 3>& emissive, float shininess);
		Material();
		void Apply(bool dull);
	private:
		std::array<float, 4> m_ambient, m_diffuse, m_specular;
		std::array<float, 3> m_emissive;
		float m_shininess;
		bool m_isDull;
	};
}