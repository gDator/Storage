#ifndef GUI_DATABASE
#define GUI_DATABASE

#include <thread>
#include <SFML/Graphics.hpp>
#include "imgui_sfml.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <iostream>
#include "Console.hpp"
#include "Item.hpp"
#include "ViewHandler.hpp"
#include "SimpleIni.h"


class GuiDatabase
{
    enum class Action {NONE, NEW_ITEM, UPDATE_ITEM, SEARCH_ITEM, NEW_ASSEMBLE, ADD_ITEM_TO_ASSEMBLE, UPDATE_ASSEMBLE, ADD_ITEM_TO_ASSEMBLE_AND_STORAGE};
    private:
        bool show_console = false;
        bool show_item = false;
        bool show_assemble = false;
        bool show_search = false;
        bool fullscreen = false;
        bool show_info = false;
        bool show_remove = false;
        bool show_bom = false;
        bool show_assemble_list = false;
        bool show_change_item = false;
        bool m_dark_mode = true;
        bool m_gui_database_updated = false;
        
        int show_check_item = 0;
        int show_check_item_in_assemble = 0;
        Item item_to_check;
        int count_to_check;

        std::map<std::string, std::tuple<std::string, std::string, std::string>> g_filter_hints;

        Action m_action;
        Item* p_selected_item = nullptr;
        Assemble* p_selected_assemble = nullptr;
        Console* p_console;
        sf::ViewHandler* p_view_handler;
        std::unique_ptr<ItemDatabase> p_database;

        void showConsole(sf::RenderWindow &w);
        void showSearch(sf::RenderWindow &w);
        void showItem(sf::RenderWindow &w);
        void showChangeItem(sf::RenderWindow &w);
        void showAssemble(sf::RenderWindow &w);
        void showInfo(sf::RenderWindow &w);
        void showRemove(sf::RenderWindow &w);
        void showBOM(sf::RenderWindow &w);
        void showAssembleList(sf::RenderWindow &w);

        void addItemToStorageWithCheck();
        void addItemToAssembleWithCheck();

    public:
        //Callback Varaiables
        // std::function<void(std::filesystem::path p)> callback_file_reader;

        void draw(sf::RenderWindow &w);
        
        GuiDatabase(Console* c, sf::ViewHandler* view_handler)
        {
            CSimpleIniA ini;
            ini.LoadFile("storage.ini");
            auto result = ini.GetValue("Settings", "Database", "");
            if(result != "")
                p_database = std::make_unique<ItemDatabase>(std::string(result));
            p_console = c;
            p_console->linkVisibleVaraibale(&show_console);
            p_view_handler = view_handler;
            ImGui::GetCurrentContext()->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

            //Add all the filter hints
            g_filter_hints.emplace("R", std::make_tuple("1k", "0603", "1%"));
            g_filter_hints.emplace("C", std::make_tuple("100n", "0603", "1% X7R"));
            g_filter_hints.emplace("L", std::make_tuple("100n", "0603", "1% X7R"));
            g_filter_hints.emplace("J", std::make_tuple("100n", "0603", "1% X7R"));
            g_filter_hints.emplace("U", std::make_tuple("100n", "0603", "1% X7R"));
            g_filter_hints.emplace("D", std::make_tuple("100n", "0603", "1% X7R"));
            g_filter_hints.emplace("K", std::make_tuple("100n", "0603", "1% X7R"));
            g_filter_hints.emplace("Q", std::make_tuple("100n", "0603", "1% X7R"));
        }
};

class SQLList
{
    private:
        std::deque<Item> m_items;
    public:
        SQLList() {}
};

#endif /*GC_FRONTEND*/