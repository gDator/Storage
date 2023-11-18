#ifndef ITEM_DATABASE_HPP
#define ITEM_DATABASE_HPP
#include <string>
#include <deque>
#include "Item.hpp"
#include "Assemble.hpp"
#include "logger.hpp"

#ifdef _WIN32
#include <windows.h>
#include <Lmcons.h>
#elif __linux__
#include <unistd.h>
#endif

class ItemDatabase
{
    private:
        std::string m_filename;
        SQLite::Database m_database;
        std::deque<Item> m_list;
        std::deque<Assemble> m_assemble_list;
        char username[256];
        bool m_updated = true;
        int m_id; //tracks intern id 
    public:
        const std::string getDatabasePath() {return m_filename;}
        void initStorage(bool new_database);
        int addItem(Item i);    //returns the used id
        void updateItem(Item i);
        const std::deque<Item>& searchItem();
        const std::deque<Item>& searchItem(Item i);
        const std::deque<Item>& searchItemByID(int id);
        bool removeItem(int id);
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
        void reserveItemToAssemble(int id, Item item, int count, bool stack_reservation = false);
        void removeReservationFromAssemble(int id, Item item);
        void removeReservationFromAssemblePartial(int assemble_id, Item item, int count);
        int getReservationsFromAssembles(Item item);
        int itemIsReservedFromAssemble(int assemble_id, Item item);
        void updateItemInReservation(int assemble_id, Item item, int count);
         //this fucntion delivers all assembles  without it items
        const std::deque<Assemble>& searchAssembles();

        ItemDatabase(std::string filename) : m_filename(filename), m_database(m_filename, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)
        {
            #ifdef _WIN32
                DWORD username_len = 256+1;
                GetUserName(username, &username_len);
                
            #elif __linux__
                char name [256]; 
                int getlogin_r(char *name, size_t 256);
                username = std::to_string(name);
            #endif
            LOG_HISTORY("User " << username <<  "logged in Database " << filename);
        }

        ~ItemDatabase() 
        {
            LOG_HISTORY("User " << username <<  "logged out from Database " << m_filename);
        }
};

#endif /*ITEM_DATABASE_HPP*/