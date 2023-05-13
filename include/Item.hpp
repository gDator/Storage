#ifndef ITEM_HPP
#define ITEM_HPP

#include "SQLiteCpp/SQLiteCpp.h"
#include "program.hpp"
#include <string>
#include <deque>
#include <filesystem>
#include <iostream>
#include "logger.hpp"

inline std::map<std::string, std::map<std::string, std::string>> g_categories;
inline std::map<std::tuple<std::string, std::string>, std::tuple<std::string, std::string, std::string, std::string>> g_filter_hints;

// inline std::map<std::string, std::string> g_categories;
inline std::vector<std::string> g_storage_x = {"None"};
inline std::vector<std::string> g_storage_y = {"None"};
inline std::map<std::string, std::string> g_units = {{"None", "None"}};
// inline std::vector<std::string> g_filter_categories = {"None", "R", "C", "L", "J", "U", "D", "K", "Q"};
// inline std::vector<std::string> g_filter_storage_x = {"None"};
// inline std::vector<std::string> g_filter_storage_y = {"None"};

class Item
{
    private: 
        bool m_filter_item;

    public: 
        Item(bool filter_item = false) : m_filter_item(filter_item)
        {
            clear();
        }
        int id;
        int count;
        double price;
        double price_per_unit; // not stored in database
        int vpe;
        int reserved;
        std::string value;
        std::string value_2;
        std::string package;
        std::string description;
        std::string unit;
        std::string main_category;
        std::string manufactor;
        std::string manufactor_number;
        std::string distributor;
        std::string shop_number;
        std::string storage_place;
        std::string category;
        std::string datasheet;

    void clear();
    std::string serializeCSV() const;
    static bool deserializeCSV(Item& i, std::vector<std::string> data);

};

void sortItems(std::deque<Item>& content);

#endif /*ITEM_HPP*/