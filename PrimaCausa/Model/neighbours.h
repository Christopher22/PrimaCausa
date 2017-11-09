#pragma once

#include "types.h"
#include "coordinate.h"

#include <vector>

namespace Model {
	namespace Neighbours {
		static void Rect(Coordinate pos, Index distance, std::vector<Coordinate>& neighbours) {
			neighbours.clear();
			const Index x = pos.GetX(), y = pos.GetY(), minX = (int(x) - distance > 0 ? x - distance : 0), min_y = (int(y) - distance > 0 ? y - distance : 0);

			for (auto tmpX = x - distance; tmpX <= x + distance; tmpX++) {
				for (auto tmpY = y - distance; tmpY <= y + distance; tmpY++) {
					Coordinate tmp(tmpX, tmpY);
					if (!(tmpX == x && tmpY == y)) {
						neighbours.push_back(tmp);
					}
				}
			}
		}

		static void Hex(Coordinate pos, Index distance, std::vector<Coordinate>& neighbours) {
			neighbours.clear();

			const int	cubeX = pos.GetY(), 
						cubeZ = pos.GetX() - (pos.GetY() - (pos.GetY() & 1)) / 2, 
						cubeY = -cubeX - cubeZ,
						n = int(distance);

			for (int dx = -n; dx <= n; dx++) {
				for (int dy = std::max(-n, -dx - n); dy <= std::min(n, -dx + n); dy++) {
					const int dz = -dx - dy, tmpX = dx + cubeX, tmpY = dy + cubeY, tmpZ = dz + cubeZ;

					const int offsetX = tmpZ + (tmpX - (tmpX & 1)) / 2, offsetY = tmpX;
					if (offsetX >= 0 && offsetY >= 0 && !(offsetX == pos.GetX() && offsetY == pos.GetY())) {
						neighbours.push_back(Coordinate(offsetX, offsetY));
					}
				}
			}
		}
	}
}
