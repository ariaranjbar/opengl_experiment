#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <array>

namespace uiutils
{
	const std::array <const std::string, 4> image { "png", "jpg", "jpeg", "hdr" };

	const std::array <const std::string, 1> text { "txt" };

	const std::array <const std::string, 12> shader { "glsl", "shader", "fragment", "frag",
		"vertex", "vert", "geometery", "geom", "tesselation", "tess", "compute", "comp" };

	const std::array <const std::string, 1> model { "obj" };

	enum class file_format {IMAGE, TEXT, SHADER, MODEL, ALL};

	bool is_of_format(std::string str, file_format ff);

	class UI
	{
	private:
		std::filesystem::path m_LastUsedPath;
		
	public:
		inline UI() : m_LastUsedPath("./") {};
		std::string queryFile(std::string file_to_locate, file_format format = file_format::ALL);
	};
}