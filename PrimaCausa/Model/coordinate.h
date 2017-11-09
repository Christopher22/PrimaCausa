#pragma once

#include "types.h"

#include <functional>

namespace Model {
	class Coordinate {
	public:
		Coordinate(const Index x, const Index y);
		Index GetX() const;
		Index GetY() const;
		bool operator==(const Coordinate &anotherCoord) const;
	private:
		Index m_x, m_y;
	};
}

namespace std {
	template <>
	struct hash<Model::Coordinate> {
		std::size_t operator()(const Model::Coordinate& coord) const {
			size_t res = 17;
			res *= 31 + hash<Model::Index>()(coord.GetX());
			res *= 31 + hash<Model::Index>()(coord.GetY());
			return res;
		}
	};
}