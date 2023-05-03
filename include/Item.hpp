#ifndef ITEM_HPP
#define ITEM_HPP

#include "SQLiteCpp/SQLiteCpp.h"
#include "program.hpp"
#include <string>
#include <deque>
#include <filesystem>
#include <iostream>
#include "logger.hpp"

static std::vector<std::string> g_categories = {"R", "C", "L", "J", "U", "D", "K", "Q"};
static std::vector<std::string> g_storage_x = {"A", "B", "C", "D", "E", "F", "G", "H"};
static std::vector<std::string> g_storage_y = {"1", "2", "3", "4", "5", "6", "7", "8"};

static std::vector<std::string> g_filter_categories = {"None", "R", "C", "L", "J", "U", "D", "K", "Q"};
static std::vector<std::string> g_filter_storage_x = {"None", "A", "B", "C", "D", "E", "F", "G", "H"};
static std::vector<std::string> g_filter_storage_y = {"None", "1", "2", "3", "4", "5", "6", "7", "8"};
struct Item
{
    int id;
    int count;
    double price;

    std::string value;
    std::string value_2;
    std::string package;
    std::string description;
    std::string vpe;
    std::string manufactor;
    std::string manufactor_number;
    std::string distributor;
    std::string shop_number;
    std::string storage_place;
    std::string category;
    std::string datasheet;

    void clear()
    {
        id = 0; 
        count = 0;
        price = 0.0;
        value = "";
        value_2 = "";
        package = "";
        description = "";
        vpe = "";
        manufactor = "";
        manufactor_number = "";
        distributor = "";
        shop_number = "";
        storage_place = "None|None";
        category = "None";
        datasheet = "";
    }

    std::string serializeCSV() const 
    {
        std::string csv;
        csv += category + ";";
        csv += value + ";";
        csv += value_2 + ";";
        csv += package + ";";
        csv += description + ";";
        csv += manufactor + ";";
        csv += manufactor_number + ";";
        csv += distributor + ";";
        csv += shop_number + ";";
        csv += vpe + ";";
        csv += std::to_string(price) + ";";
        csv += std::to_string(count);
        return csv;
    }

    static bool deserializeCSV(Item& i, std::vector<std::string> data)
    {
        i.clear();
        if(data.size() != 12)
        {
            LOG_ERROR("CSV data has not right amount of columns");
            return false;
        }
            
        i.category = data[0];
        i.value = data[1];
        i.value_2 = data[2];
        i.package = data[3];
        i.description = data[4];
        i.manufactor = data[5];
        i.manufactor_number = data[6];
        i.distributor = data[7];
        i.shop_number = data[8];
        i.vpe = data[9];
        i.price = std::stof(data[10]);
        i.count = std::stoi(data[11]);
        return true;
    }

};

struct Assemble
{
    unsigned int id;
    std::string name;
    std::deque<std::tuple<Item, int>> bom;
};

class ItemDatabase
{
    private:
        std::string m_filename;
        SQLite::Database m_database;
        std::deque<Item> m_list;
        std::deque<Assemble> m_assemble_list;

        bool m_updated = true;
        int id; //tracks intern id 
    public:
        const std::string getDatabasePath() {return m_filename;}
        void initStorage(bool new_database);
        int addItem(Item i);    //returns the used id
        void updateItem(Item i);
        const std::deque<Item>& searchItem();
        const std::deque<Item>& searchItem(Item i);
        const std::deque<Item>& searchItemByID(int id);
        void addAssemble(Assemble a);
        void updateItemInAssemble(Assemble a, Item i, int count);
        bool itemExistsInAssemble(Assemble a, Item i);
        const std::deque<Item>& searchItemInAssemble(Assemble a, Item i);
        const std::deque<Item>& searchItemInAssembleByID(Assemble a, Item i);
        void addItemToAssemble(Assemble a, Item i, int count);
        void deleteItemFromAssemble(Assemble a, Item i);
        void updateRecognized() {m_updated = false;}
        bool isUpdated() {return m_updated;}
       
        const Assemble searchAssemble(Assemble a);
         //this fucntion delivers all assembles  without it items
        const std::deque<Assemble>& searchAssembles();

        ItemDatabase(std::string filename) : m_filename(filename), m_database(m_filename, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)
        {
           
        }
        ~ItemDatabase() {}
};
#endif /*ITEM_HPP*/