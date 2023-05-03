#ifndef GUI_DATABASE
#define GUI_DATABASE

#include <thread>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_stdlib.h"
#include <iostream>
#include "Console.hpp"
#include "Item.hpp"
#include "SimpleIni.h"
#define DEBUG
#include "logger.hpp"

class GuiDatabase
{
    enum class Action {NONE, NEW_ITEM, UPDATE_ITEM, SEARCH_ITEM, NEW_ASSEMBLE, ADD_ITEM_TO_ASSEMBLE, UPDATE_ASSEMBLE, ADD_ITEM_TO_ASSEMBLE_AND_STORAGE};
    //old state was int: 0, 1, 2, 3, 4 | 1, 2, 3
    enum class StateAlternativePicking {NONE, CHECK_EXISTANCE, ERROR_EXISTS, CHECK_ALTERNATIVES, SELECT_ALTERNATIVE, FINISHED, CHECK_ALTERNATIVES_ASSEMBLE, SELECT_ALTERNATIVE_ASSEMBLE};
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
        bool show_import_check = false;
        bool m_dark_mode = true;
        bool m_gui_database_updated = false;
        
        //state machine varibales for alternative picking dialog
        StateAlternativePicking show_check_item = StateAlternativePicking::NONE;
        StateAlternativePicking show_check_item_in_assemble = StateAlternativePicking::NONE;
        Item item_to_check;
        int count_to_check;

        std::map<std::string, std::tuple<std::string, std::string, std::string>> g_filter_hints;

        Action m_action;
        Item* p_selected_item = nullptr;
        Assemble* p_selected_assemble = nullptr;
        Console* p_console;
        std::unique_ptr<ItemDatabase> p_database;

        std::vector<Item> import_csv{0};

        void showConsole(GLFWwindow* w);
        void showSearch(GLFWwindow *w);
        void showItem(GLFWwindow *w);
        void showChangeItem(GLFWwindow *w);
        void showAssemble(GLFWwindow *w);
        void showInfo(GLFWwindow *w);
        void showRemove(GLFWwindow *w);
        void showBOM(GLFWwindow *w);
        void showAssembleList(GLFWwindow *w);

        void addItemToStorageWithCheck();
        void addItemToAssembleWithCheck();
        void importItemToAssembly();

    public:
        //Callback Varaiables
        // std::function<void(std::filesystem::path p)> callback_file_reader;

        void draw(GLFWwindow *w);
        
        GuiDatabase(Console* c)
        {
            CSimpleIniA ini;
            auto result = "";
            if(std::filesystem::exists("./storage.ini"))
            {
                
                ini.LoadFile("storage.ini");
                result = ini.GetValue("Settings", "Database", "");
                if(result != "" && std::filesystem::exists(result))
                {
                    p_database = std::make_unique<ItemDatabase>(std::string(result));
                    p_database->initStorage(false);
                }
                else
                    LOG_ERROR("No Database on this path");
            }
            
            p_console = c;
            p_console->linkVisibleVaraibale(&show_console);
            ImGui::GetCurrentContext()->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

            //Add all the filter hints
            for(auto& entry : g_categories)
            {
                std::string result(ini.GetValue("Hints", entry.c_str(), ""));
                if(result == "")
                    continue;
                std::string::size_type position = 0;
                std::string::size_type last_position = 0;
                std::vector<std::string> hint(3);
                int index = 0;
                LOG_INFO(result);
                while(position < result.size())
                {
                    last_position = position;
                    position = result.find(";", position);
                    if(index <= 2) // ignore more than 3 entries
                    {
                        hint[index] = result.substr(last_position, position - last_position);
                        index++;
                    }
                    if(position == std::string::npos) //break;
                        break;
                    position++; // jump over semikolon
                }
                LOG_INFO("found hints: " << hint[0] << ", " << hint[1]<< "," <<hint[2]);
                g_filter_hints.emplace(entry, std::make_tuple(hint[0], hint[1], hint[2]));
            }
            
            // g_filter_hints.emplace("C", std::make_tuple("100n", "0603", "1% X7R"));
            // g_filter_hints.emplace("L", std::make_tuple("100n", "0603", "1% X7R"));
            // g_filter_hints.emplace("J", std::make_tuple("100n", "0603", "1% X7R"));
            // g_filter_hints.emplace("U", std::make_tuple("100n", "0603", "1% X7R"));
            // g_filter_hints.emplace("D", std::make_tuple("100n", "0603", "1% X7R"));
            // g_filter_hints.emplace("K", std::make_tuple("100n", "0603", "1% X7R"));
            // g_filter_hints.emplace("Q", std::make_tuple("100n", "0603", "1% X7R"));
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