#include "utils.hpp"
#include <sstream>

void split(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
}

void split(const std::string& s, std::string delim, std::vector<std::string>& elems)
{
	std::string::size_type position = 0;
	std::string::size_type last_position = 0;
	int index = 0;
	while(position < s.size())
	{
		last_position = position;
		position = s.find(";", position);
		elems.push_back(s.substr(last_position, position - last_position));
		index++;
		if(position == std::string::npos) //break;
			break;
		position += delim.length(); // jump over semikolon
	}
}