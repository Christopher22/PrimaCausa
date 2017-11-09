#pragma once

#include <vector>

namespace View {
	namespace Tokenizer {
		static void Tokenize(const std::string& s, char delim, std::vector<std::string>& output) {
			if ( output.size() > 0 ) {
				output.clear();
			}

			std::size_t start = 0, end = 0;
			while ( ( end = s.find(delim, start) ) != std::string::npos ) {
				output.push_back(s.substr(start, end - start));
				start = end + 1;
			}
			output.push_back(s.substr(start));
		}
	}
}