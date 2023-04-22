#ifndef CSV_HPP
#define CSV_HPP

#include <string>
#include <vector>
#include <fstream>


class CSV
{
    public:
        //template needs function serializeCSV wich returns a string
        static void exportCSV(std::string path, std::vector<std::string> data, std::vector <std::string> header);
        static std::vector<std::string> importCSV(std::string path);
};
#endif /*CSV_HPP*/