#include "CSV.hpp"
#include <iostream>
void CSV::exportCSV(std::string path, std::vector<std::string> data, std::vector <std::string> header)
{
    std::ofstream csv(path);
    for(size_t i = 0; i < header.size(); i++)
    {
        csv << header[i];
        if(i != header.size() - 1) //last one
            csv << ";";
    }
        
    for(auto& d : data)
        csv << "\n" << d; 
    csv.close();
}

std::vector<std::vector<std::string>> CSV::importCSV(std::string path)
{
    std::ifstream csv(path);
    std::vector<std::vector<std::string>> data;
    data.clear();
    std::vector<std::string> line;
    std::string buffer;
    std::getline(csv, buffer); //dont read header
    while(!csv.eof())
    {
        std::getline(csv, buffer);
        line.clear();
        std::string::size_type position = 0;
        std::string::size_type last_position = 0;
        while(position < buffer.size())
        {
            last_position = position;
            position = buffer.find(";", position);
            line.push_back(std::string(buffer.substr(last_position, position - last_position)));
            if(position == std::string::npos)
                break;
            position++; //jump over semikolon 
        }
        data.push_back(line);        
    }
    csv.close();
    return data;
}