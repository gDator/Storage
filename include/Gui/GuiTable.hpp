#ifndef GUI_TABLE_HPP
#define GUI_TABLE_HPP

#include "Item.hpp"
#include <deque>
class GuiTable
{
    public:
        void draw(std::deque<Item>& content, ImVector<unsigned int>& selection, Item*& selected);
};

#endif /*GUI_TABLE_HPP*/