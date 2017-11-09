#pragma once

namespace View {

	template <class T>
	class Vec3d {
	public:
		Vec3d(const T x, const T y, const T z) : m_x(x), m_y(y), m_z(z) {}

		Vec3d(const Vec3d<T>& copy) : m_x(copy.GetX()), m_y(copy.GetY()), m_z(copy.GetZ()) {}

		inline T GetX() const {
			return m_x;
		}

		inline T GetY() const {
			return m_y;
		}

		inline T GetZ() const {
			return m_z;
		}

		friend bool operator==(Vec3d<T> &vec, Vec3d<T> &anotherVec){
			return (vec.GetX() == anotherVec.GetX() && vec.GetY() == anotherVec.GetY() && vec.GetZ() == anotherVec.GetZ());
		}

		friend Vec3d<T> operator+(Vec3d<T> &vec, Vec3d<T> &anotherVec){
			return Vec3d<T>(vec.GetX() + anotherVec.GetX(), vec.GetY() + anotherVec.GetY(), vec.GetZ() + anotherVec.GetZ());
		}

		friend Vec3d<T> operator-(Vec3d<T> &vec, Vec3d<T> &anotherVec){
			return Vec3d<T>(vec.GetX() - anotherVec.GetX(), vec.GetY() - anotherVec.GetY(), vec.GetZ() - anotherVec.GetZ());
		}

		friend Vec3d<T> operator/(Vec3d<T> &vec, T &num){
			return Vec3d<T>(vec.GetX() / num, vec.GetY() / num, vec.GetZ() / num);
		}

	private:
		T m_x, m_y, m_z;
	};
}
