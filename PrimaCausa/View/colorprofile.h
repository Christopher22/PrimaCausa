#pragma once;

namespace View {
	class ColorProfile {
	public:
		const static ColorProfile RED;
		const static ColorProfile BLUE;
		const static ColorProfile WHITE;
		const static ColorProfile BLACK;
		const static ColorProfile LIME;
		const static ColorProfile YELLOW;
		const static ColorProfile CYAN;
		const static ColorProfile MAGENTA;
		const static ColorProfile SILVER;
		const static ColorProfile GRAY;
		const static ColorProfile MAROON;
		const static ColorProfile OLIVE;
		const static ColorProfile GREEN;
		const static ColorProfile PURPLE;
		const static ColorProfile TEAL;
		const static ColorProfile NAVY;
		const static ColorProfile BROWN;
		ColorProfile(float red, float green, float blue, float alpha=0);
		void Apply() const;
	private:
		float m_red, m_green, m_blue, m_alpha;
	};
}