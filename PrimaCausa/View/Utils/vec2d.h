#pragma once
#include "Vec3d.h"

#include <cmath>

namespace View {

	template <class T>
	class Vec2d {
	public:
		Vec2d(const T x, const T y) : m_x(x), m_y(y) {}

		inline T GetX() const {
			return m_x;
		}

		inline T GetY() const {
			return m_y;
		}

		T Magnitude() const {
			return ( std::sqrtf(std::powf(m_x, 2) + std::powf(m_y, 2)) );
		}

		Vec3d<T> Get3DVersion() {
			return Vec3d<T>(m_x, m_y, 0);
		}

		friend bool operator==(Vec2d<T> &vec, Vec2d<T> &anotherVec){
			return (vec.GetX() == anotherVec.GetX() && vec.GetY() == anotherVec.GetY());
		}

		friend Vec2d<T> operator+(Vec2d<T> &vec, Vec2d<T> &anotherVec){
			return Vec2d<T>(vec.GetX() + anotherVec.GetX(), vec.GetY() + anotherVec.GetY());
		}

		friend Vec2d<T> operator-(Vec2d<T> &vec, Vec2d<T> &anotherVec){
			return Vec2d<T>(vec.GetX() - anotherVec.GetX(), vec.GetY() - anotherVec.GetY());
		}

		friend Vec2d<T> operator/(Vec2d<T> &vec, T &num){
			return Vec2d<T>(vec.GetX() / num, vec.GetY() / num);
		}

	private:
		T m_x, m_y;
	};
}



