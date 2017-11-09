#pragma once

#include "../types.h"

#include <fstream>
#include <functional>
#include <vector>
#include <sstream>

namespace Model {
	template<typename ChunkType, typename Size>
	static bool readContainer(Path path, std::vector<char> magicNumber, std::function<bool(ChunkType, std::ifstream&, Size)> callback) {
		std::ifstream file(path, std::ios::binary);

		// Check for right magic number
		char* first = new char[magicNumber.size()];
		if (!file || !file.read(first, magicNumber.size()) || !std::equal(magicNumber.begin(), magicNumber.end(), first)) {
			delete[] first;
			return false;
		} else {
			delete[] first;
		}

		// Read chunk by chunk
		ChunkType type;
		Size size;
		while (file && file.peek() != EOF) {
			if (!file.read((char*)&type, sizeof(ChunkType)) || !file.read((char*)&size, sizeof(Size))) {
				return false;
			}

			const auto pos = file.tellg();
			if (!callback(type, file, size) || file.tellg() != pos + std::streamsize(size)) {
				return false;
			}
		}

		return true;
	}

	template<typename ChunkType, typename Size>
	static bool writeContainer(Path path, std::vector<char> magicNumber, std::function<bool(ChunkType&, std::stringstream&)> callback) {
		std::ofstream file(path, std::ios::binary | std::ios::trunc);

		if (!file) {
			return false;
		}

		file.write(magicNumber.data(), magicNumber.size());

		std::stringstream data;
		ChunkType type;
		while (callback(type, data)) {
			Size dataSize = Size(data.str().size());
			file.write(static_cast<char*>(static_cast<void*>(&type)), sizeof(ChunkType));
			file.write(static_cast<char*>(static_cast<void*>(&dataSize)), sizeof(Size));
			file.write(data.str().c_str(), data.str().size());
			data.str(std::string());
		}
		return true;
	}
}