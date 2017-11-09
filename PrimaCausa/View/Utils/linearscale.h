#pragma once

namespace View{
	namespace LinearScale {
		static inline float Scale(float sourceMin, float sourceMax, float targetMin, float targetMax, float number) {
			return targetMin + ( (number-sourceMin) * (targetMax - targetMin)) / (sourceMax - sourceMin);
		}
	}
}

