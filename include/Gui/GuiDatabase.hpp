#ifndef GUI_DATABASE
#define GUI_DATABASE

#include <thread>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_stdlib.h"
#include <iostream>
#include "Console.hpp"
#include "Item.hpp"
#include "Assemble.hpp"
#include "ItemDatabase.hpp"
#include "SimpleIni.h"
#include "logger.hpp"
#include <queue>

class GuiDatabase
{
    enum class ItemAction {NONE, NEW_ITEM, UPDATE_ITEM,  ADD_ITEM_TO_ASSEMBLE_AND_STORAGE};
    enum class CountAction {NONE, NEW_ITEM, INCREASE_ITEM, DECREASE_ITEM, ADD_ITEM_TO_ASSEMBLE, UPDATE_ASSEMBLE,
                            RESET_ITEM_RESERVATION, RELEASE_ITEM_RESERVATION, RESERVE_ITEM, RESERVE_ITEM_TO_ASSEMBLE};
    //old state was int: 0, 1, 2, 3, 4 | 1, 2, 3
    enum class StateAlternativePicking {NONE, CHECK_EXISTANCE, ERROR_EXISTS, CHECK_ALTERNATIVES, SELECT_ALTERNATIVE, FINISHED, CHECK_ALTERNATIVES_ASSEMBLE, SELECT_ALTERNATIVE_ASSEMBLE};
    private:
        bool show_console = true;
        bool show_new_item = false;
        bool show_assemble = false;
        bool show_search = false;
        bool fullscreen = false;
        bool show_info = true;
        bool show_remove = false;
        bool show_bom = false;
        bool show_assemble_list = false;
        bool show_change_item = false;
        bool show_import_check = false;
        bool show_import_storage_check = false;
        bool m_dark_mode = true;
        bool m_gui_database_updated = false;
        
        
        //state machine varibales for alternative picking dialog
        StateAlternativePicking show_check_item = StateAlternativePicking::NONE;
        StateAlternativePicking show_check_item_in_assemble = StateAlternativePicking::NONE;
        Item item_to_check;
        int count_to_check;

        //Item reservation
        bool allow_assemble_reservation = false;
        std::deque<std::tuple<Item, int>> bom_content;
        std::deque<bool> bom_export_selection;

        ItemAction m_item_action;
        CountAction m_count_action;
        Item* p_selected_item = nullptr;
        Assemble* p_selected_assemble = nullptr;
        Console* p_console;
        std::unique_ptr<ItemDatabase> p_database;

        std::queue<Item> import_csv;

        void showConsole();
        void showSearch();
        void showNewItem();
        void showChangeItem();
        void showAssemble();
        void showInfo();
        void showRemove();
        void showBOM();
        void showAssembleList();

        void addItemToStorageWithCheck();
        void addItemToAssembleWithCheck();
        void importItemToAssembly();
        void importItemToStorage();

    public:
        //Callback Varaiables
        // std::function<void(std::filesystem::path p)> callback_file_reader;

        void draw();
        
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
                    show_search = true;
                    show_assemble_list = true;
                }
                else
                    LOG_ERROR("No Database on this path");
            }
            
            p_console = c;
            p_console->linkVisibleVaraibale(&show_console);
            ImGui::GetCurrentContext()->IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
        }
};

#endif /*GC_FRONTEND*/