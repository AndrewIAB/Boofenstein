#include "File_Utility.hpp"

#include <iostream>

std::string File_Utility::read_file(const std::string path, std::fstream& fstream) {
	std::string str;

	fstream.open(path, std::ios_base::in);
	
	fstream.seekg(0, std::ios_base::end);
	str = std::string(fstream.tellg(), ' ');
	fstream.seekg(0);

	fstream.read(&str[0], str.length());
	fstream.close();

	return str;
}

std::string get_line(std::string str, size_t offset) {
	std::string line;

	for (size_t i = offset; i < str.length(); i++) {
		if (str[i] == '\n') {
			break;
		}
		line += str[i];
	}
	return line;
}

std::string File_Utility::read_file_include(const std::string path, std::fstream& fstream) {
	std::string file = read_file(path, fstream);

	std::string line;
	size_t offset = 0;

	while (offset < file.length()) {
		size_t pre_offset = offset;

		line = get_line(file, offset);
		offset += line.length() + 1;

		// #include
		// 12345678
		if (line.substr(0, 8) == "#include") {
			std::string include_path = line.substr(9);
			file = file.substr(0, pre_offset) + read_file_include(include_path, fstream) + file.substr(offset - 1);
		}
	}

	return file;
}

void File_Utility::read_file_lines(std::vector<std::string>& lines, std::string path, std::fstream& fstream) {
	fstream.open(path, std::ios_base::in);
	fstream.seekg(0);

	for (std::string line; std::getline(fstream, line); ) {
		lines.push_back(line);
	}
	
	fstream.close();
}