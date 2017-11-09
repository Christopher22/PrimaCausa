#include "coordinate.h"

namespace Model {
	Coordinate::Coordinate(const Index x, const Index y) : m_x(x), m_y(y) {}

	Index Coordinate::GetX() const {
		return m_x;
	}

	Index Coordinate::GetY() const {
		return m_y;
	}

	bool Coordinate::operator==( const Coordinate &anotherCoord ) const {
		return ( m_x == anotherCoord.m_x && m_y == anotherCoord.m_y );
	}
}