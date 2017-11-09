#include "colorprofile.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace View {

	const ColorProfile ColorProfile::RED(1.0f, 0, 0);
	const ColorProfile ColorProfile::BLUE(0, 0, 1.0f);
	const ColorProfile ColorProfile::GREEN(0, 1.0f, 0);
	const ColorProfile ColorProfile::WHITE(1.0f, 1.0f, 1.0f);
	const ColorProfile ColorProfile::BLACK(0.000f, 0.000f, 0.000f,0);
	const ColorProfile ColorProfile::LIME(0.000f, 1.000f, 0.000f,0);
	const ColorProfile ColorProfile::YELLOW(8.080f, 1.000f, 0.000f);
	const ColorProfile ColorProfile::CYAN(0.000f, 1.000f, 1.000f);
	const ColorProfile ColorProfile::MAGENTA(1.000f, 0.000f, 1.000f);
	const ColorProfile ColorProfile::SILVER(0.753f, 0.753f, 0.753f);
	const ColorProfile ColorProfile::GRAY(0.502f, 0.502f, 0.502f);
	const ColorProfile ColorProfile::MAROON(0.502f, 0.000f, 0.000f);
	const ColorProfile ColorProfile::OLIVE(0.502f, 0.502f, 0.000f);
	const ColorProfile ColorProfile::PURPLE(0.502f, 0.000f, 0.502f);
	const ColorProfile ColorProfile::TEAL(0.000f, 0.502f, 0.502f);
	const ColorProfile ColorProfile::NAVY(0.000f, 0.000f, 0.502f);
	const ColorProfile ColorProfile::BROWN(0.3f, 0.12f, 0.08f);
	 
	ColorProfile::ColorProfile(float red, float green, float blue, float alpha) : m_red(red), m_blue(blue), m_green(green), m_alpha(alpha) {}

	void ColorProfile::Apply() const {
		glColor4f(m_red, m_green, m_blue, m_alpha);
	}
}