#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace File_Utility {
	std::string read_file(const std::string path, std::fstream& fstream);
	std::string read_file_include(const std::string path, std::fstream& fstream);
	void read_file_lines(std::vector<std::string>& lines, std::string path, std::fstream& fstream);
};