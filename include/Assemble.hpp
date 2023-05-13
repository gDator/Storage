#ifndef ASSEMBLE_HPP
#define ASSEMBLE_HPP

struct Assemble
{
    unsigned int id;
    std::string name;
    std::deque<std::tuple<Item, int>> bom;
};


#endif //ASSSEMBLE_HPP