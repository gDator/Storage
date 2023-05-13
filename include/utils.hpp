#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

void split(const std::string& s, char delim, std::vector<std::string>& elems);
void split(const std::string& s, std::string delim, std::vector<std::string>& elems);


#endif /*UTILS_HPP*/