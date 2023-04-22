#include "CSV.hpp"

void CSV::exportCSV(std::string path, std::vector<std::string> data, std::vector <std::string> header)
{
    std::ofstream csv(path);
    for(size_t i = 0; i < header.size(); i++)
    {
        csv << header[i];
        if(i != header.size() - 1) //last one
            csv << ";";
    }
    csv << "\n";
        
    for(auto& d : data)
        csv << d << "\n";
    csv.close();
}

std::vector<std::string> CSV::importCSV(std::string path)
{

}