#pragma once

#include <string>

namespace Model {
	class Coordinate;

	typedef unsigned char TileType;
	typedef unsigned short Index;
	typedef const std::string& Path;

	template<typename Renderable, typename CustomData>
	using RenderableLoader = Renderable*(*)(Path, CustomData);

	template<typename Renderable, typename CustomData>
	using Renderer = void(*)(Coordinate, Index, Renderable*, CustomData);

	struct Empty {};
}