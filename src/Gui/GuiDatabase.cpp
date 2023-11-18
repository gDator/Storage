#include "Gui/GuiDatabase.hpp"
#include "FileDialog.hpp"
#include "CSV.hpp"
#include "Gui/GuiTable.hpp"
#include "imgui_spectrum.h"
#include "Global.hpp"
#include "Language/Language.hpp"

void GuiDatabase::draw()
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu(L_FILE))
        {
            if(ImGui::MenuItem(L_NEW_DATABASE))
            {
                std::string path = FileDialog::SaveFile("Database (*.db)\0\*.db\0");
                if(path != "")
                {
                    p_database = std::make_unique<ItemDatabase>(path);
                    p_database->initStorage(true);
                }
                // show_file_explorer = true;
                // std::filesystem::path selected_path = std::filesystem::absolute(std::filesystem::current_path());
                // file_explorer.PickFile(std::filesystem::absolute(std::filesystem::current_path()), [&](const std::filesystem::path& selected_path) {/*Store data*/});
            }

            if(ImGui::MenuItem(L_LOAD_DATABASE))
            {
                std::string path = FileDialog::OpenFile("Database (*.db)\0\*.db\0");
                if(path != "")
                {
                    p_database = std::make_unique<ItemDatabase>(path);
                    CSimpleIniA ini;
                    SI_Error rc = ini.LoadFile("storage.ini");
                    rc = ini.SetValue("Settings", "Database", path.c_str());
	                ini.SaveFile("storage.ini");
                    p_database->initStorage(false);
                }
                    
                // show_file_explorer = true;
                // std::filesystem::path selected_path = std::filesystem::absolute(std::filesystem::current_path());
                // file_explorer.PickFile(std::filesystem::absolute(std::filesystem::current_path()), [&](const std::filesystem::path& selected_path) {/*Store data*/});
            }
            ImGui::Separator();
            /*if(ImGui::BeginMenu("Importieren"))
            {
                if(ImGui::MenuItem("CSV")) 
                {
                    std::string path = FileDialog::OpenFile("CSV (*.csv)\0*.csv");
                    if(path != "")
                    {
                        auto item_strings = CSV::importCSV(path);
                        import_csv.clear();
                        for(auto& string : item_strings)
                        {
                            Item i;
                            if(Item::deserializeCSV(i, string))
                            {
                                import_csv.push_back(i);
                            }
                            show_import_storage_check = true;
                        }
                    }
                }
                ImGui::EndMenu();
            }*/
            if(ImGui::BeginMenu(L_EXPORT))   //whole storage
            {
                if(ImGui::MenuItem(L_CSV)) 
                {
                    std::string path = FileDialog::SaveFile("CSV (*.csv)\0*.csv");
                    if(path != "" && p_database != nullptr)
                    {
                        std::deque<Item> content = p_database->searchItem();
                        std::vector<std::string> result;
                        for(size_t i = 0; i < content.size(); i++)
                        {
                            result.push_back(content[i].serializeCSV());
                        }
                        CSV::exportCSV(path, result, {L_CATEGORY, L_VALUE_FIRST, L_VALUE_SECOND, L_PACKAGE, L_DESCRIPTION, L_MANUFACTOR, L_MANUFACTOR_NO, L_DISTRIBUTOR, L_DISTRIBUTOR_NO, L_PACKAGING, L_PRICE, L_PRICE_PER_PIECE, L_COUNT});
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu(L_EDIT))
        {
            if(ImGui::BeginMenu(L_COMPONENT))
            {
                if(ImGui::MenuItem(L_NEW)) {show_new_item = true;}
                if(p_selected_item != nullptr)
                {
                    if(ImGui::MenuItem(L_UPDATE)) 
                    {
                        show_change_item = true; 
                        m_item_action = ItemAction::UPDATE_ITEM;
                    }
                    
                    if(ImGui::MenuItem(L_RESERVE_COMPONENT)) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::RESERVE_ITEM;
                    }
                    if(ImGui::MenuItem(L_CONSUME_COMPONENT_RESERVATION)) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::RESET_ITEM_RESERVATION;
                    }
                    if(ImGui::MenuItem(L_RELEASE_COMPONENT_RESERVATION)) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::RELEASE_ITEM_RESERVATION;
                    }
                    if(ImGui::MenuItem(L_ADD_COMPONENT_TO_STORAGE)) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::INCREASE_ITEM;
                    }
                    if(ImGui::MenuItem(L_REMOVE_COMPONENT_FROM_STORAGE))
                    {
                        show_remove = true; 
                        m_count_action = CountAction::DECREASE_ITEM;
                    }
                    if(ImGui::MenuItem(L_DISAPEAR))
                    {
                        if(p_selected_item != nullptr)
                            if(p_database->removeItem(p_selected_item->id))
                            {
                                p_selected_item = nullptr;
                                cmsg("[info] Item deleted.");
                            }           
                    }
                }
                ImGui::EndMenu();
            }
            
            if(ImGui::BeginMenu(L_ASSEMBLY))
            {
                if(ImGui::MenuItem(L_NEW)) {show_assemble = true;}
                if(ImGui::MenuItem(L_ADD_COMPONENT_TO_STORAGE_AND_ASSEMBLY))
                {
                    if(p_selected_assemble != nullptr)
                    {
                        m_item_action = ItemAction::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE;
                        show_new_item = true;
                    }
                }
                if(ImGui::MenuItem(L_PRODUCE_ASSEMBLY)) 
                {
                    if(p_selected_assemble != nullptr)
                    {
                        show_remove = true;
                        m_count_action = CountAction::UPDATE_ASSEMBLE;
                    }
                    else
                        cmsg("[warning] No Assemble selected");
                }
                if(ImGui::MenuItem(L_ADD_COMPONENT_TO_ASSEMBLY))
                {
                    if(p_selected_item != nullptr)
                    {
                        if(p_selected_assemble != nullptr)
                        {
                            m_count_action = CountAction::ADD_ITEM_TO_ASSEMBLE;
                            show_remove = true;
                        }
                        else
                        {
                            cmsg("[warning] No Assemble selected");
                        }
                        
                    }
                    else
                        cmsg("[warning] No Item selected");
                }
                ImGui::EndMenu();
            }
            
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu(L_VIEW))
        {
            if(ImGui::MenuItem(L_STORAGE, NULL ,&show_search)) {m_gui_database_updated = false;}
            if(ImGui::MenuItem(L_ASSEMBLY_LIST, NULL, &show_assemble_list)) {m_gui_database_updated = false;}
            if(ImGui::MenuItem(L_CONSOLE, NULL, &show_console)) {}
            if(ImGui::MenuItem(L_INFO_BOX, NULL, &show_info)) {}
            if(ImGui::MenuItem(L_BOM)) 
            {
                if(p_selected_assemble != nullptr)
                {
                    show_bom = true;
                }
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu(L_OPTIONS))
        {
            // if(ImGui::MenuItem("Vollbild", NULL, &fullscreen)) 
            // {
            // }
            ImGui::Separator();
            if(ImGui::MenuItem(L_DARKMODE, NULL, &m_dark_mode)) {m_dark_mode? ImGui::Spectrum::StyleColorsSpectrumDark() : ImGui::Spectrum::StyleColorsSpectrumLight(); }
            if(ImGui::MenuItem(L_ABOUT)) {}
            ImGui::EndMenu();
        }        
        ImGui::EndMainMenuBar();
        
    } 

    if(show_console)
        showConsole();
    if(show_info)
        showInfo();
   
    if(p_database != nullptr)
    {
        if(p_database->isUpdated())
        {
            m_gui_database_updated = false;
        }
        if(show_search)
        showSearch();
        if(show_new_item)
            showNewItem();
        if(show_remove)
            showRemove();
        if(show_assemble)
            showAssemble();
        if(show_bom)
            showBOM();
        if(show_assemble_list)
            showAssembleList();
        if(show_change_item)
            showChangeItem();
        if(show_check_item != StateAlternativePicking::NONE)
            addItemToStorageWithCheck();
        if(show_check_item == StateAlternativePicking::FINISHED)
            addItemToAssembleWithCheck();
        if(m_gui_database_updated) //updated Database is also updated in Gui
            p_database->updateRecognized();
        if(show_import_check)
            importItemToAssembly();
        if(show_import_storage_check)
            importItemToStorage();     
    }
    
    
}

void GuiDatabase::showConsole()
{
    p_console->draw();
}

void GuiDatabase::showSearch()
{
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    if(ImGui::Begin(L_STORAGE, &show_search))
    {
        static std::deque<Item> content;
        static ImVector<unsigned int> selection;
        static Item filter(true);
        static int storage_current_x_idx, storage_current_y_idx;
        static GuiTable table;
        static std::vector<uint8_t> category_filter(g_categories.size()); 
        ImGui::SameLine();
        if(ImGui::Button(L_UPDATE))
        {
            content.resize(0);
            selection.resize(0);
            cmsg("[info] Searching Database");
            // filter.category = g_filter_categories[item_current_idx];
            filter.storage_place = g_storage_x[storage_current_x_idx] + "|" + g_storage_y[storage_current_y_idx];
            
            int i = 0;
            std::string save = filter.main_category;
            filter.main_category = "";
            for(auto cat : g_categories)
            {
                if(category_filter[i])
                    filter.main_category += std::get<0>(cat) + ";";
                i++;
            }        
            content = p_database->searchItem(filter);
            filter.main_category = save;
        }

        ImGui::SameLine();
        if(ImGui::TreeNode(L_FILTER))
        {
            int i = 0;
            for(auto& cat : g_categories)
            {
                ImGui::Checkbox(std::get<0>(cat).c_str(), (bool*) &(category_filter[i]));
                ImGui::SameLine();
                i++;
            }
            ImGui::NewLine();
            if (ImGui::BeginCombo(L_MAIN_CATEGORY, filter.main_category.c_str()))
            {
                for (auto& cat : g_categories)
                {
                    const bool is_selected = (filter.category.compare(std::get<0>(cat)) == 0);
                    if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                        filter.main_category = std::get<0>(cat);

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::BeginCombo(L_CATEGORY, (g_categories[filter.main_category])[filter.category].c_str()))
            {
                for (auto& cat : g_categories[filter.main_category])
                {
                    const bool is_selected = (filter.category.compare(std::get<0>(cat)) == 0);
                    if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                        filter.category = std::get<0>(cat);

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::InputText(L_VALUE_FIRST, &filter.value);
            ImGui::InputText(L_VALUE_SECOND, &filter.value_2);
            ImGui::InputText(L_PACKAGE, &filter.package);
            ImGui::InputText(L_DESCRIPTION, &filter.description);
            ImGui::InputText(L_MANUFACTOR, &filter.manufactor);
            ImGui::InputText(L_MANUFACTOR_NO, &filter.manufactor_number);
            ImGui::InputText(L_DISTRIBUTOR, &filter.distributor);
            ImGui::InputText(L_DISTRIBUTOR_NO, &filter.shop_number);
            ImGui::InputInt(L_PACKAGING, &filter.vpe);
            ImGui::InputInt(L_COUNT, &filter.count);
            ImGui::InputDouble(L_PRICE_PER_PACKAGING, &filter.price, 0.01f, 1.0f, "%.3f");
            if (ImGui::BeginCombo(L_STORAGE_PLACE_X, g_storage_x[storage_current_x_idx].c_str()))
            {
                for (int n = 0; n < g_storage_x.size(); n++)
                {
                    const bool is_selected = (storage_current_x_idx == n);
                    if (ImGui::Selectable(g_storage_x[n].c_str(), is_selected))
                    {
                        storage_current_x_idx = n;
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::NewLine();
            if (ImGui::BeginCombo(L_STORAGE_PLACE_Y, g_storage_y[storage_current_y_idx].c_str()))
            {
                for (int n = 0; n < g_storage_y.size(); n++)
                {
                    const bool is_selected = (storage_current_y_idx == n);
                    if (ImGui::Selectable(g_storage_y[n].c_str(), is_selected))
                    {
                        storage_current_y_idx = n;
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::TreePop();
        }

        table.draw(content, selection, p_selected_item);

        if(p_database->isUpdated()) //Automated update
        {
            m_gui_database_updated = true;
            // filter storage place
            filter.storage_place = g_storage_x[storage_current_x_idx] + "|" + g_storage_y[storage_current_y_idx];
            //prepare filter for main category
            content.clear();
            content = p_database->searchItem(filter);
            cmsg("[info] Automated Update");
        }
    }
    ImGui::End();
}

void GuiDatabase::showNewItem()
{
    static Item item;
    // static unsigned int item_current_idx = 0;
    static unsigned int storage_current_x_idx = 0;
    static unsigned int storage_current_y_idx = 0;
    static int amount;
    static bool enter_price_per_unit;
    if(ImGui::Begin(L_COMPONENT))
    {
        if(m_item_action == ItemAction::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE)
        {
            ImGui::TextUnformatted(L_NOTE_COMPONENT_ADDITION_1);
            ImGui::SameLine();
            ImGui::TextUnformatted(p_selected_assemble->name.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(L_NOTE_COMPONENT_ADDITION_2);
            ImGui::InputInt(std::string(std::string(L_COUNT) + "(" + std::string(L_ASSEMBLY) + ")").c_str(), &amount);      
        }

        if (ImGui::BeginCombo(L_MAIN_CATEGORY, item.main_category.c_str()))
        {
            for (auto& cat : g_categories)
            {
                const bool is_selected = (item.category.compare(std::get<0>(cat)) == 0);
                if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                    item.main_category = std::get<0>(cat);

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(L_CATEGORY, (g_categories[item.main_category])[item.category].c_str()))
        {
            for (auto& cat : g_categories[item.main_category])
            {
                const bool is_selected = (item.category.compare(std::get<0>(cat)) == 0);
                if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                    item.category = std::get<0>(cat);

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if(g_filter_hints.count(std::make_tuple(item.main_category, item.category)) > 0) //if there is an hint entry
        {
            ImGui::InputTextWithHint(L_VALUE_FIRST, std::get<0>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str(), &item.value);
            ImGui::InputTextWithHint(L_VALUE_SECOND, std::get<1>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str(), &item.value_2);
            ImGui::InputTextWithHint(L_PACKAGE, std::get<2>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str() ,&item.package);
            ImGui::InputTextWithHint(L_DESCRIPTION, std::get<3>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str(), &item.description);
        }
        else
        {
            ImGui::InputText(L_VALUE_FIRST, &item.value);
            ImGui::InputText(L_VALUE_SECOND, &item.value_2);
            ImGui::InputText(L_PACKAGE ,&item.package);
            ImGui::InputText(L_DESCRIPTION, &item.description);
        }
        
        ImGui::InputText(L_MANUFACTOR, &item.manufactor);
        ImGui::InputText(L_MANUFACTOR_NO, &item.manufactor_number);
        ImGui::InputText(L_DISTRIBUTOR, &item.distributor);
        ImGui::InputText(L_DISTRIBUTOR_NO, &item.shop_number);
        ImGui::Checkbox(L_SWITCH_PRICE_PACKING_PIECE, &enter_price_per_unit);
        if(enter_price_per_unit)
        {
            ImGui::InputInt(L_PACKAGING, &item.vpe, 1, 10);
            ImGui::TextUnformatted(L_PRICE_PER_PACKAGING);
            ImGui::SameLine();
            ImGui::TextUnformatted(" :");
            ImGui::SameLine();
            ImGui::TextUnformatted(std::to_string(item.price_per_unit*item.vpe).c_str());
            ImGui::SameLine();
            ImGui::InputDouble(L_PRICE_PER_PIECE, &item.price_per_unit, 0.01f, 1.0f, "%.4f");
        }
        else
        {
            ImGui::InputInt(L_PACKAGING, &item.vpe, 1, 10);
            ImGui::InputDouble(L_PRICE_PER_PACKAGING, &item.price, 0.01f, 1.0f, "%.4f");
            ImGui::TextUnformatted(std::string(L_PRICE_PER_PIECE).c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(" :");
            ImGui::SameLine();
            ImGui::TextUnformatted(item.vpe == 0?"-":std::to_string(item.price/item.vpe).c_str());
        }    

        ImGui::InputInt(L_COUNT, &item.count);

        if (ImGui::BeginCombo(L_UNIT, item.unit.c_str()))
        {
            for (auto& cat : g_units)
            {
                const bool is_selected = (item.unit.compare(std::get<0>(cat)) == 0);
                if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                    item.unit = std::get<0>(cat);

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(L_STORAGE_PLACE_X, g_storage_x[storage_current_x_idx].c_str()))
        {
            for (int n = 0; n < g_storage_x.size(); n++)
            {
                const bool is_selected = (storage_current_x_idx == n);
                if (ImGui::Selectable(g_storage_x[n].c_str(), is_selected))
                    storage_current_x_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(L_STORAGE_PLACE_Y, g_storage_y[storage_current_y_idx].c_str()))
        {
            for (int n = 0; n < g_storage_y.size(); n++)
            {
                const bool is_selected = (storage_current_y_idx == n);
                if (ImGui::Selectable(g_storage_y[n].c_str(), is_selected))
                    storage_current_y_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::InputText(L_DOCUMENT_LINK, &item.datasheet);
        ImGui::SameLine();
        if(ImGui::Button(L_DOCUMENT_LINK))
        {
            item.datasheet = FileDialog::OpenFile("pdf (*.pdf), Word-Dokument (*.docx)\0*.pdf\0*.doc\0*.odt\0*.docx\0");
        }
        if(ImGui::Button(L_OK)) 
        {   
            if(enter_price_per_unit)
            {
                item.price = item.price_per_unit*item.vpe;
            }
            item.category = g_categories[item.main_category][item.category];
            item.storage_place = g_storage_x[storage_current_x_idx] + "|" + g_storage_y[storage_current_y_idx];
            
            show_check_item = StateAlternativePicking::CHECK_EXISTANCE;
            item_to_check = item;
            if(m_item_action == ItemAction::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE)
            {
                item_to_check = item;
                count_to_check = amount;
                show_check_item_in_assemble = StateAlternativePicking::CHECK_ALTERNATIVES_ASSEMBLE;
            }
            item.clear();
            show_new_item = false; // close if ready?
        }
        ImGui::SameLine();
        if(ImGui::Button(L_CLOSE)) 
        {
            item.clear();
            show_new_item = false;
            m_item_action = ItemAction::NONE;
        }
    }
    ImGui::End();
}

void GuiDatabase::showChangeItem()
{
    static bool enter_price_per_unit;
    if(ImGui::Begin(L_COMPONENT))
    {
        if (ImGui::BeginCombo(L_MAIN_CATEGORY, p_selected_item->main_category.c_str()))
        {
            for (auto& cat : g_categories)
            {
                const bool is_selected = (p_selected_item->category.compare(std::get<0>(cat)) == 0);
                if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                    p_selected_item->main_category = std::get<0>(cat);

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo(L_CATEGORY, (g_categories[p_selected_item->main_category])[p_selected_item->category].c_str()))
        {
            for (auto& cat : g_categories[p_selected_item->main_category])
            {
                const bool is_selected = (p_selected_item->category.compare(std::get<0>(cat)) == 0);
                if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                    p_selected_item->category = std::get<0>(cat);

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::InputText(L_VALUE_FIRST, &p_selected_item->value);
        ImGui::InputText(L_VALUE_SECOND, &p_selected_item->value_2);
        ImGui::InputText(L_PACKAGE, &p_selected_item->package);
        ImGui::InputText(L_DESCRIPTION, &p_selected_item->description);
        ImGui::InputText(L_MANUFACTOR, &p_selected_item->manufactor);
        ImGui::InputText(L_MANUFACTOR_NO, &p_selected_item->manufactor_number);
        ImGui::InputText(L_DISTRIBUTOR, &p_selected_item->distributor);
        ImGui::InputText(L_DISTRIBUTOR_NO, &p_selected_item->shop_number);
        ImGui::Checkbox(L_SWITCH_PRICE_PACKING_PIECE, &enter_price_per_unit);
        if(enter_price_per_unit)
        {
            ImGui::InputInt(L_PACKAGING, &p_selected_item->vpe, 1, 10);
            ImGui::TextUnformatted(L_PRICE_PER_PACKAGING);
            ImGui::SameLine();
            ImGui::TextUnformatted(": ");
            ImGui::SameLine();
            ImGui::TextUnformatted(std::to_string(p_selected_item->price_per_unit*p_selected_item->vpe).c_str());
            ImGui::SameLine();
            ImGui::InputDouble(L_PRICE_PER_PIECE, &p_selected_item->price_per_unit, 0.01f, 1.0f, "%.4f");
        }
        else
        {
            ImGui::InputInt(L_PACKAGING, &p_selected_item->vpe, 1, 10);
            ImGui::InputDouble(L_PRICE_PER_PACKAGING, &p_selected_item->price, 0.01f, 1.0f, "%.4f");
            ImGui::TextUnformatted(L_PRICE_PER_PIECE);
            ImGui::SameLine();
            ImGui::TextUnformatted(": " + p_selected_item->vpe== 0?"-":std::to_string(p_selected_item->price/p_selected_item->vpe).c_str());
        }        
        ImGui::InputInt(L_COUNT, &p_selected_item->count);
        if (ImGui::BeginCombo(L_UNIT, p_selected_item->unit.c_str()))
        {
            for (auto& cat : g_units)
            {
                const bool is_selected = (p_selected_item->unit.compare(std::get<0>(cat)) == 0);
                if (ImGui::Selectable(std::get<0>(cat).c_str(), is_selected))
                    p_selected_item->unit = std::get<0>(cat);

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::InputText(L_STORAGE, &p_selected_item->storage_place);
        ImGui::InputText(L_DOCUMENT_LINK, &p_selected_item->datasheet);
        ImGui::SameLine();
        if(ImGui::Button(L_DOCUMENT_LINK))
        {
            p_selected_item->datasheet = FileDialog::OpenFile("pdf (*.pdf), Word-Dokument (*.docx)\0*.pdf\0*.doc\0*.odt\0*docx\0");
        }
        if(ImGui::Button(L_OK)) 
        {   
            if(enter_price_per_unit)
            {
                p_selected_item->price = p_selected_item->price_per_unit*p_selected_item->vpe;
            }
            p_database->updateItem(*p_selected_item);
            show_change_item = false; // close if ready?
            m_item_action = ItemAction::NONE;
        }
        ImGui::SameLine();
        if(ImGui::Button(L_CLOSE)) 
        {
            show_change_item = false;
            m_item_action = ItemAction::NONE;
        }
    }
    ImGui::End();
}

void GuiDatabase::showAssemble()
{
    static Assemble assemble;
    if(ImGui::Begin(L_ASSEMBLY))
    {
        ImGui::InputText(L_ASSEMBLY_NAME, &assemble.name);
        ImGui::NewLine();
        if(ImGui::Button(L_OK)) 
        {
            p_database->addAssemble(assemble);
            show_assemble = false;
        }
        ImGui::SameLine();
        if(ImGui::Button(L_CLOSE)) 
        {
            show_assemble = false;
        }
    }
    ImGui::End();
}

void GuiDatabase::showInfo()
{
    if(ImGui::Begin(L_INFO_BOX, &show_info))
    {
        if(p_database != nullptr)
        {
            ImGui::TextUnformatted(L_DATABASE_PATH);
            ImGui::SameLine();
            ImGui::TextUnformatted(p_database->getDatabasePath().c_str());
        }
        else
        {
            ImGui::TextUnformatted(L_DATABASE_PATH);
            ImGui::SameLine(); 
            ImGui::TextUnformatted(": -");
        }
            
            
        if(p_selected_item != nullptr)
        {
            ImGui::TextUnformatted(L_SELECTED_COMPONENT);
            ImGui::SameLine();
            ImGui::TextUnformatted(": [");
            ImGui::SameLine();
            ImGui::TextUnformatted(p_selected_item->category.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted("]: ");
            ImGui::SameLine();
            ImGui::TextUnformatted(p_selected_item->description.c_str());
        }
        else
        {
            ImGui::TextUnformatted(L_SELECTED_COMPONENT);
            ImGui::SameLine();
            ImGui::TextUnformatted(" : -");
        }
           

        ImGui::NewLine();
        if(p_selected_assemble != nullptr)
        {
            ImGui::TextUnformatted(L_SELECTED_ASSEMBLY);
            ImGui::SameLine();
            ImGui::TextUnformatted(": [");
            ImGui::SameLine();
            ImGui::TextUnformatted(p_selected_assemble->name.c_str());
        }
            
        else
        {
            ImGui::TextUnformatted(L_SELECTED_ASSEMBLY);
            ImGui::SameLine();
            ImGui::TextUnformatted(": -");
        }

        ImGui::NewLine();
        if(ImGui::Button(L_CLOSE)) 
        {
            show_info = false;
        }
    }
    ImGui::End();
    
}

void GuiDatabase::showRemove()
{
    static int amount = 0;
    static bool add = false;
    static int tolerance = 0;
    if(m_count_action == CountAction::NONE)
        return;
    if(ImGui::Begin(L_WINDOW_TITLE_CHANGE_COUNT, &show_remove))
    {
        if(m_count_action == CountAction::DECREASE_ITEM)
            ImGui::TextUnformatted(L_NOTE_REMOVE_COMPONENT);
        if(m_count_action == CountAction::INCREASE_ITEM)
            ImGui::TextUnformatted(L_NOTE_ADD_COMPONENT);
        if(m_count_action == CountAction::RESERVE_ITEM)
            ImGui::TextUnformatted(L_RESERVE_COMPONENT);
        if(m_count_action == CountAction::RESET_ITEM_RESERVATION)
            ImGui::TextUnformatted(L_CONSUME_COMPONENT_RESERVATION);
        if(m_count_action == CountAction::RELEASE_ITEM_RESERVATION)
            ImGui::TextUnformatted(L_RELEASE_COMPONENT_RESERVATION);
        else if (m_count_action == CountAction::UPDATE_ASSEMBLE)
            ImGui::TextUnformatted(L_NOTE_PRODUCE_ASSEMBLY);
        else if (m_count_action == CountAction::ADD_ITEM_TO_ASSEMBLE)
            ImGui::TextUnformatted(L_NOTE_ADD_COMPONENT_TO_ASSEMBLY);
        else if(m_count_action == CountAction::RESERVE_ITEM_TO_ASSEMBLE)
            ImGui::TextUnformatted(L_NOTE_ASSEMBLY_RESERVATION);
        else
            ImGui::TextUnformatted("");
        ImGui::InputInt(L_COUNT, &amount);

        if(m_count_action == CountAction::RESERVE_ITEM_TO_ASSEMBLE)
            ImGui::DragInt(L_TOLERANCE, &tolerance, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
        if(ImGui::Button(L_OK)) 
        {
            if(p_selected_assemble != nullptr && p_selected_item != nullptr && m_count_action == CountAction::ADD_ITEM_TO_ASSEMBLE)
            {
                item_to_check = *p_selected_item;
                count_to_check = amount;
                show_check_item = StateAlternativePicking::FINISHED; 
                show_check_item_in_assemble =  StateAlternativePicking::CHECK_ALTERNATIVES_ASSEMBLE;
            }
            if(p_selected_assemble != nullptr && m_count_action == CountAction::UPDATE_ASSEMBLE)
            {
                auto product = p_database->searchAssemble(*p_selected_assemble);
                for(auto& item : product.bom)
                {
                    std::get<0>(item).count -= (std::get<1>(item)*amount);
                    p_database->updateItem(std::get<0>(item));
                }
            }
            if(p_selected_item != nullptr && m_count_action == CountAction::INCREASE_ITEM)
            {
                p_selected_item->count += amount;
                p_database->updateItem(*p_selected_item);
            }
            if(p_selected_item != nullptr && m_count_action == CountAction::DECREASE_ITEM)
            {
                if(p_selected_item->count - amount >= 0)
                {
                    p_selected_item->count -= amount;
                    p_database->updateItem(*p_selected_item);
                }
                else
                    cmsg("[error] Too less items");
            }
            if(p_selected_item != nullptr && m_count_action == CountAction::RESERVE_ITEM)
            {
                
                
                    // p_selected_item->count -= amount;
                    // p_selected_item->reserved += amount;
                    // p_database->updateItem(*p_selected_item);
                p_database->reserveItemToAssemble(-1, *p_selected_item, amount, true);
                if(p_selected_item->count - amount >= 0)
                {
                    
                }
                else
                    cmsg("[warning] More items reserved than stored.");
            }
            if(p_selected_item != nullptr && m_count_action == CountAction::RELEASE_ITEM_RESERVATION)
            {
                if(p_selected_item->reserved - amount >= 0)
                {
                    p_database->reserveItemToAssemble(-1, *p_selected_item, -amount, true);
                    // p_selected_item->count += amount;
                    // p_selected_item->reserved -= amount;
                    // p_database->updateItem(*p_selected_item);
                }
                else
                    cmsg("[error] Cant release more items than reserved");
            }
            if(p_selected_item != nullptr && m_count_action == CountAction::RESET_ITEM_RESERVATION)
            {
                if(p_selected_item->reserved - amount >= 0)
                {
                    p_database->removeReservationFromAssemblePartial(-1, *p_selected_item, amount);
                    // p_selected_item->reserved -= amount;
                    // p_database->updateItem(*p_selected_item);
                }
                else
                    cmsg("[error] Cant release more items than reserved");
            }

            if(p_selected_assemble != nullptr && m_count_action == CountAction::RESERVE_ITEM_TO_ASSEMBLE)
            {
            
                for(size_t i = 0; i < bom_content.size(); i++)
                {
                    int n = (int)std::get<1>(bom_content[i])*((float)amount*(1+(float)tolerance/100));
                    if(bom_export_selection[i]) // item is selected
                    {
                        p_database->reserveItemToAssemble(p_selected_assemble->id, std::get<0>(bom_content[i]), n);
                        if(std::get<0>(bom_content[i]).count >= n){}
                            
                        else
                            cmsg(std::string("[warning] Too less items (ID: " + std::to_string(std::get<0>(bom_content[i]).id) + ")"));
                    }
                }
            }
            show_remove = false;
            m_count_action = CountAction::NONE;
        }
        ImGui::SameLine();
        if(ImGui::Button(L_CLOSE))
        {
            show_remove = false;
            amount = 0;
            m_count_action = CountAction::NONE;
        }
    }
    ImGui::End();
}

void GuiDatabase::showAssembleList()
{
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    if(ImGui::Begin(L_ASSEMBLY_LIST, &show_assemble_list))
    {
        static ImGuiTableFlags table_flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
            //| ImGuiTableFlags_SizingFixedFit;
        static std::deque<Assemble> content;
        static ImVector<unsigned int> selection;
        static std::string status = ""; 
        ImGui::SameLine();
        if(ImGui::Button(L_UPDATE))
        {
            Assemble filter;
            content.resize(0);
            selection.resize(0);
            status = "Searching...";
            status = "Ready...";
            content = p_database->searchAssembles();
            // content = p_database->searchAssemble(filter);
        }
        ImGui::SameLine();
        ImGui::TextUnformatted(status.c_str());
        
        if(ImGui::BeginTable("search_results", 2, table_flags))
        {
            ImGui::TableSetupColumn(L_ID, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_ASSEMBLY_NAME, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
            ImGui::TableHeadersRow();

            if(!content.empty())
            {                
                ImGuiListClipper clipper;
                clipper.Begin(content.size());
                while (clipper.Step())
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                    {   
                        unsigned int id = content[row_n].id;
                        bool selected = selection.contains(id);
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        //ImGui::TextUnformatted(std::get<0>(content.at(row_n))->getName().c_str());                     
                        if (ImGui::Selectable(std::to_string(id).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, 0)))
                        {
                            if (ImGui::GetIO().KeyCtrl)
                            {
                                if (selected)
                                    selection.find_erase_unsorted(id);
                                else
                                    selection.push_back(id);
                            }
                            else
                            {
                                selection.clear();
                                selection.push_back(id);
                                p_selected_assemble = &content[row_n];
                            }
                        }
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(content[row_n].name.c_str());
                    }
            }
            ImGui::EndTable();
        }    
    }
    ImGui::End();   
}

void GuiDatabase::showBOM()
{
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    if(p_selected_assemble == nullptr)
    {
        show_assemble_list = false;
        cmsg("[warning] No Assemble selected");
        return;
    }
    
    static ImVector<unsigned int> selection;
    static std::string status = "";
    std::string title = std::string(L_BOM) + " - " + p_selected_assemble->name;


    if(ImGui::Begin(title.c_str(), &show_bom))
    {
        if(ImGui::Button(L_UPDATE))
        {
            bom_content = p_database->searchAssemble(*p_selected_assemble).bom;
            bom_export_selection.resize(bom_content.size());
        }
        ImGui::SameLine();
        if(ImGui::Button(L_REMOVE))
        {
            if(selection.size() != 0)
            {
                Item i;
                i.id = selection[0];
                p_database->deleteItemFromAssemble(*p_selected_assemble, i);
                selection.clear();
            }
        }
        ImGui::SameLine();
        if(ImGui::Button(L_EXPORT_CSV))
        {
            std::string path = FileDialog::SaveFile("CSV (*.csv)\0*.csv");
            if(path != "")
            {
                std::vector<std::string> result;
                for(size_t i = 0; i < bom_content.size(); i++)
                {
                    if(bom_export_selection[i])
                        result.push_back(std::get<0>(bom_content[i]).serializeCSV());
                }
                CSV::exportCSV(path, result, {L_CATEGORY, L_VALUE_FIRST, L_VALUE_SECOND, L_PACKAGE, L_DESCRIPTION, L_MANUFACTOR, L_MANUFACTOR_NO, L_DISTRIBUTOR, L_DISTRIBUTOR_NO, L_PACKAGING, L_PRICE, L_PRICE_PER_PIECE, L_COUNT});
            }
            
        }
        ImGui::SameLine();
        if(ImGui::Button(L_IMPORT_CSV))
        {
            std::string path = FileDialog::OpenFile("CSV (*.csv)\0*.csv");
            if(path != "")
            {
                auto item_strings = CSV::importCSV(path);
                for(int i = 0; i < import_csv.size(); i++)
                    import_csv.pop();
                for(auto& string : item_strings)
                {
                    Item i;
                    if(Item::deserializeCSV(i, string))
                    {
                        import_csv.push(i);                        
                    }
                }
                show_import_check = true;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button(L_RESERVE_COMPONENT))
        {
            m_count_action = CountAction::RESERVE_ITEM_TO_ASSEMBLE;
            show_remove = true;
        }

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(L_HINT_TOLERANCE);
        
        ImGui::SameLine();
        if(ImGui::Button(L_RELEASE_COMPONENT_RESERVATION))
        {
            for(size_t i = 0; i < bom_content.size(); i++)
            {
                if(bom_export_selection[i]) // item is selected
                    p_database->reserveItemToAssemble(p_selected_assemble->id, std::get<0>(bom_content[i]), 0);               
            }
        }
        ImGui::SameLine();
        if(ImGui::Button(L_CONSUME_COMPONENT_RESERVATION))
        {
            for(size_t i = 0; i < bom_content.size(); i++)
            {
                if(bom_export_selection[i]) // item is selected
                    p_database->removeReservationFromAssemble(p_selected_assemble->id, std::get<0>(bom_content[i]));               
            }
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(L_HINT_CONSUME_RESERVATION);

        if(ImGui::Button(L_MARK_ALL))
        {
            for(size_t i = 0; i < bom_export_selection.size(); i++)
            {
                bom_export_selection[i] = true;                                        
            }
        }
        ImGui::SameLine();
        if(ImGui::Button(L_MARK_NONE))
        {
            for(size_t i = 0; i < bom_export_selection.size(); i++)
            {
                bom_export_selection[i] = false;                                        
            }
        }
        ImGui::SameLine();
        if(ImGui::Button(L_MARK_REVERSE))
        {
            for(size_t i = 0; i < bom_export_selection.size(); i++)
            {
                if(bom_export_selection[i]) // item is selected
                    bom_export_selection[i] = false;
                else
                    bom_export_selection[i] = true;
            }
        }

        static ImGuiTableFlags table_flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
            //| ImGuiTableFlags_SizingFixedFit;
        
        ImGui::SameLine();
        ImGui::TextUnformatted(status.c_str());
        
        if(ImGui::BeginTable("search_results", 19, table_flags))
        {
            ImGui::TableSetupColumn(L_POSITION, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_ID, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("?", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_CATEGORY, ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn(L_VALUE_FIRST, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_VALUE_SECOND, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_PACKAGE, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_DESCRIPTION, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_RESERVED_BOM, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_COUNT_BOM, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_COUNT, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_MANUFACTOR, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_MANUFACTOR_NO, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_DISTRIBUTOR, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_DISTRIBUTOR_NO, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_PACKAGING, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_PRICE_PER_PACKAGING, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_STORAGE_PLACE, ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn(L_DOCUMENT_LINK, ImGuiTableColumnFlags_NoSort);
            
            ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
            ImGui::TableHeadersRow();

            if(!bom_content.empty())
            {                
                ImGuiListClipper clipper;
                clipper.Begin(bom_content.size());
                while (clipper.Step())
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                    {   
                        ImGui::TableNextRow();
                        unsigned int id = std::get<0>(bom_content[row_n]).id;
                        bool selected = selection.contains(id);
                        
                        ImVec4 color (0.0f, 0.0f, 0.0f, 1.0f);
                    
                        //red has priority
                        if(std::get<0>(bom_content[row_n]).reserved > std::get<0>(bom_content[row_n]).count)
                        {
                            color = ImVec4(.7f, .3f, .3f, .5f);
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(color));
                        }                            
                        else if(std::get<1>(bom_content[row_n]) > std::get<0>(bom_content[row_n]).count)
                        {
                            color = ImVec4(.7f, 0.7f, 0.3f, .5f);
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(color));
                        }
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(std::to_string(row_n + 1).c_str()); // begin with 1
                        ImGui::TableSetColumnIndex(1);
                        //ImGui::TextUnformatted(std::get<0>(content.at(row_n))->getName().c_str());                     
                        if (ImGui::Selectable(std::to_string(id).c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0.0f, ImGui::GetFrameHeight())))
                        {
                            if (ImGui::GetIO().KeyCtrl)
                            {
                                if (selected)
                                    selection.find_erase_unsorted(id);
                                else
                                    selection.push_back(id);
                            }
                            else
                            {
                                selection.clear();
                                selection.push_back(id);
                                p_selected_item = &std::get<0>(bom_content[row_n]);
                            }
                        }
                        ImGui::TableSetColumnIndex(2);
                        ImGui::PushID(&(bom_export_selection[row_n]));
                        ImGui::Checkbox("###", &(bom_export_selection[row_n]));
                        ImGui::PopID();
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).category.c_str());
                        ImGui::TableSetColumnIndex(4);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).value.c_str());
                        ImGui::TableSetColumnIndex(5);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).value_2.c_str());
                        ImGui::TableSetColumnIndex(6);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).package.c_str());
                        ImGui::TableSetColumnIndex(7);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).description.c_str());
                        ImGui::TableSetColumnIndex(8);
                        //in deisem Fall ist ist die Reservierung auf das Projekt bezogen
                        ImGui::TextUnformatted(std::to_string(std::get<0>(bom_content[row_n]).reserved).c_str());
                        ImGui::TableSetColumnIndex(9);
                        ImGui::TextUnformatted(std::to_string(std::get<1>(bom_content[row_n])).c_str());
                        ImGui::TableSetColumnIndex(10);
                        ImGui::TextUnformatted(std::to_string(std::get<0>(bom_content[row_n]).count).c_str());
                        ImGui::TableSetColumnIndex(11);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).manufactor.c_str());
                        ImGui::TableSetColumnIndex(12);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).manufactor_number.c_str());
                        ImGui::TableSetColumnIndex(13);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).distributor.c_str());
                        ImGui::TableSetColumnIndex(14);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).shop_number.c_str());
                        ImGui::TableSetColumnIndex(15);
                        ImGui::TextUnformatted(std::to_string(std::get<0>(bom_content[row_n]).vpe).c_str());
                        ImGui::TableSetColumnIndex(16);
                        ImGui::TextUnformatted(std::to_string(std::get<0>(bom_content[row_n]).price).c_str());
                        ImGui::TableSetColumnIndex(17);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).storage_place.c_str());
                        ImGui::TableSetColumnIndex(18);
                        ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).datasheet.c_str());
                        if(std::get<0>(bom_content[row_n]).datasheet == "")
                            ImGui::TextUnformatted(std::get<0>(bom_content[row_n]).datasheet.c_str());
                        else
                            if(ImGui::SmallButton(std::get<0>(bom_content[row_n]).datasheet.c_str()))
                            {
                                //open pdf with standard program
                                try
                                {
                                    #if _WIN32
                                        ShellExecute(0,0 , std::string(g_settings.path_to_datasheet + "\\" + std::get<0>(bom_content[row_n]).datasheet).c_str(), 0, 0, SW_SHOW);
                                    #elif __linux__
                                        
                                    #endif
                                }
                                catch(const std::exception& e)
                                {
                                        if(!std::filesystem::exists(std::string(g_settings.path_to_datasheet + "\\" + std::get<0>(bom_content[row_n]).datasheet)))
                                        {
                                            cmsg("[Error] File not found");
                                            LOG_ERROR("Could not open File:" + std::string(g_settings.path_to_datasheet + "\\" + std::get<0>(bom_content[row_n]).datasheet));
                                        }
                                        else
                                        {
                                            cmsg("[Error] No pdf Program!");
                                            LOG_ERROR("No Program");
                                        }
                                }
                            }
                        }
            }
            ImGui::EndTable();
        }
        if(p_database->isUpdated())
        {
            cmsg("[info] Automated Update");
            m_gui_database_updated = true;
            bom_content.clear();
            bom_content = p_database->searchAssemble(*p_selected_assemble).bom;
            bom_export_selection.resize(bom_content.size());
        }
    }
    ImGui::End();
    
}

void GuiDatabase::addItemToStorageWithCheck()
{   
    static Item search(true);
    static std::deque<Item> content;
    static GuiTable table;
    if(show_check_item == StateAlternativePicking::CHECK_ALTERNATIVES_ASSEMBLE ||
       show_check_item == StateAlternativePicking::SELECT_ALTERNATIVE_ASSEMBLE) //has a extra function 
        return;
    if(show_check_item == StateAlternativePicking::CHECK_EXISTANCE) //state 1: check if already exists
    {   
        search.clear();
        search.manufactor = item_to_check.manufactor;
        search.manufactor_number = item_to_check.manufactor_number;
        content = p_database->searchItem(search);
        if(content.size() > 0)
        {
            ImGui::OpenPopup("Error");
            show_check_item = StateAlternativePicking::ERROR_EXISTS;
        }
        else
            show_check_item = StateAlternativePicking::CHECK_ALTERNATIVES; //go direct to stage 3
    }
    //not really needed; Popup only shows when this stage is reaches 
    //but safe is safe  
    if(show_check_item == StateAlternativePicking::ERROR_EXISTS) 
    {
        //Hard Check/ State 2 / PopUp
        if (item_to_check.manufactor == "" && item_to_check.manufactor_number == "") 
        { 
            ImGui::CloseCurrentPopup(); 
            show_check_item = StateAlternativePicking::CHECK_ALTERNATIVES;
        } // go to state 0
        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted(L_NOTE_EXISTING_COMPONENT_1);
            ImGui::SameLine();
            ImGui::TextUnformatted(item_to_check.manufactor.c_str());
            ImGui::TextUnformatted(L_NOTE_EXISTING_COMPONENT_2);
            ImGui::SameLine();
            ImGui::TextUnformatted(item_to_check.manufactor_number.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted("' (");
            ImGui::SameLine();
            ImGui::TextUnformatted(item_to_check.category.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(", ");
            ImGui::SameLine();
            ImGui::TextUnformatted(item_to_check.value.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(", ");
            ImGui::SameLine();
            ImGui::TextUnformatted(item_to_check.value_2.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(", ");
            ImGui::SameLine();
            ImGui::TextUnformatted(item_to_check.package.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(", ");
            ImGui::SameLine();
            ImGui::TextUnformatted(item_to_check.description.c_str());
            ImGui::SameLine();
            ImGui::TextUnformatted(")");
            ImGui::Separator();

            if (ImGui::Button(L_ABORT, ImVec2(120, 0))) 
            { 
                ImGui::CloseCurrentPopup(); 
                show_check_item = StateAlternativePicking::NONE; 
                show_check_item_in_assemble = StateAlternativePicking::NONE;
            } // go to state 0
            ImGui::SameLine();
            if (ImGui::Button(L_ADD_ANYWAY, ImVec2(150, 0))) 
            { 
                ImGui::CloseCurrentPopup(); 
                show_check_item = StateAlternativePicking::CHECK_ALTERNATIVES;
            } // go to state 0
            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }
    }
    
    //extra state for checking so database doesnt get polled the whole time
    if(show_check_item == StateAlternativePicking::CHECK_ALTERNATIVES) //state 3 : checl for alternatives
    {
        //Softcheck
        search.clear();
        search.category = item_to_check.category;
        search.value = item_to_check.value;
        content = p_database->searchItem(search);
        show_check_item = StateAlternativePicking::SELECT_ALTERNATIVE;
        ImGui::OpenPopup(L_RECOMMENDATION);
    }

    if(show_check_item == StateAlternativePicking::SELECT_ALTERNATIVE) //State 4: Select alternative
    {
        static ImVector<unsigned int> selection;
        static Item* alternative = nullptr;
        
        if(content.size() > 0) //Recommondations
        {
            if (ImGui::BeginPopupModal(L_RECOMMENDATION, NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted(L_NOTE_SIMILIAR_COMPONENTS);
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.category.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.value.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.value_2.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.package.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.description.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(")");
                ImGui::Separator();
                if (ImGui::Button(L_OK, ImVec2(120, 0))) 
                { 
                    //select Item()
                    if(selection.size()>0)
                    {
                        //TODO: What happens if i check another Object: Update amount?
                        // alternative->count += item_to_check.count;
                        // p_database->updateItem(*alternative);
                        selection.clear();          
                        ImGui::CloseCurrentPopup();
                        item_to_check = *alternative; //needed for assembly
                        show_check_item =  StateAlternativePicking::FINISHED;
                    }
                    else
                    {
                        cmsg("[error] No alternative component selected");
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button(L_NEW))
                {
                    //if we add item only to storage, we add the amount, which is entered by user
                    //otherwise (item gets also added to assembly) we dont want the count as amount in storage
                    //becuase count means count per assembly
                    //TODO: test
                    if(show_check_item_in_assemble != StateAlternativePicking::NONE)
                        item_to_check.count = 0;
                    cmsg("[warning] Added Item with 0 count");
                    item_to_check.id = p_database->addItem(item_to_check);
                    ImGui::CloseCurrentPopup();
                    show_check_item = StateAlternativePicking::FINISHED; //go to state 5 in 
                }
                ImGui::SameLine();
                if(ImGui::Button(L_ABORT))
                {
                    show_check_item = StateAlternativePicking::FINISHED;
                    show_check_item_in_assemble = StateAlternativePicking::NONE;
                    // import_csv.clear();
                }

                table.draw(content, selection, alternative);
                
                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }
        }
        else //content.size() <= 0
        {
            auto n = item_to_check.count;
            //If we add the item to assembly, the imported count is for the amount in the assembly
            //The amount of the item is not imported, so set it to 0
            if(show_check_item_in_assemble != StateAlternativePicking::NONE)
                item_to_check.count = 0;
            item_to_check.id = p_database->addItem(item_to_check); 
            show_check_item = StateAlternativePicking::FINISHED; //State 5 in addItemToAssembleWithCheck
            item_to_check.count = n;
        }
    }
}

void GuiDatabase::addItemToAssembleWithCheck()
{
    static Item search(true);
    static std::deque<Item> content;
    static GuiTable table;
    //Catcher: when adding an unit it 
    //if the process was started withot adding it to an assembly the Function addItemWithCheck()
    //ends up here. The following if-statement does the reset of the state machine 
    if(show_check_item_in_assemble == StateAlternativePicking::NONE && show_check_item == StateAlternativePicking::FINISHED) //no need to check for alternatives
    {
        show_check_item = StateAlternativePicking::NONE;
        return;
    }

    if(show_check_item_in_assemble == StateAlternativePicking::CHECK_ALTERNATIVES_ASSEMBLE) //state 1 : check for alternatives
    {
        search.clear();
        // search.manufactor = item_to_check.manufactor;
        // search.manufactor_number = item_to_check.manufactor_number;
        content = p_database->searchItemByID(item_to_check.id);
        if(content.size() <= 0) //Add the "real" Item (Correct ID)
        {
            cmsg("[error] Could not add component");
            show_check_item = StateAlternativePicking::NONE;
            return;
        }
        else
            item_to_check = content[0]; //go direct to stage 3

        search.clear();
        search.category = item_to_check.category;
        search.value = item_to_check.value;
        content = p_database->searchItemInAssemble(*p_selected_assemble, search);
        show_check_item_in_assemble = StateAlternativePicking::SELECT_ALTERNATIVE_ASSEMBLE;
        ImGui::OpenPopup(L_RECOMMENDATION);
        cmsg("[info] Searching for alternatives");
    }

    if(show_check_item_in_assemble == StateAlternativePicking::SELECT_ALTERNATIVE_ASSEMBLE) //State 4: Select alternative
    {
        static ImVector<unsigned int> selection;
        static Item* alternative = nullptr;
        
        if(content.size() > 0) //Recommondations
        {
            if (ImGui::BeginPopupModal(L_RECOMMENDATION, NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::TextUnformatted(L_NOTE_SIMILIAR_COMPONENTS_ASSEMBLY);
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.category.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.value.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.value_2.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.package.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(", ");
                ImGui::SameLine();
                ImGui::TextUnformatted(item_to_check.description.c_str());
                ImGui::SameLine();
                ImGui::TextUnformatted(")");
                ImGui::Separator();
                if (ImGui::Button(L_OK, ImVec2(120, 0))) 
                { 
                    //select Item()
                    if(selection.size()>0)
                    {
                        //TODO: What happens if i check another Object: Update amount?
                        
                        p_database->updateItemInAssemble(*p_selected_assemble, *alternative, count_to_check);
                        selection.clear();          
                        ImGui::CloseCurrentPopup();
                        show_check_item_in_assemble = StateAlternativePicking::NONE;
                    }
                    else
                    {
                        cmsg("[error] No alternative component selected");
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button(L_NEW))
                {
                    if(!p_database->itemExistsInAssemble(*p_selected_assemble, item_to_check))
                    {
                        p_database->addItemToAssemble(*p_selected_assemble, item_to_check, count_to_check);
                        ImGui::CloseCurrentPopup();
                        show_check_item_in_assemble = StateAlternativePicking::NONE;
                    }
                    else
                    {
                        p_database->updateItemInAssemble(*p_selected_assemble, item_to_check, count_to_check);
                        cmsg("[error] Item exits already. Try updating amount");
                        show_check_item_in_assemble = StateAlternativePicking::NONE;
                        show_check_item == StateAlternativePicking::NONE;
                         //just cancel current item. Dont do:
                        // show_import_check = false;
                    }
                    
                }
                ImGui::SameLine();
                if(ImGui::Button(L_ABORT))
                {
                    show_check_item_in_assemble = StateAlternativePicking::NONE;
                    show_check_item == StateAlternativePicking::NONE;
                }
                
                table.draw(content, selection, alternative);

                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }
        }
        else
        {
            p_database->addItemToAssemble(*p_selected_assemble, item_to_check, count_to_check);
            show_check_item_in_assemble = StateAlternativePicking::NONE;
        }
    }
}

void GuiDatabase::importItemToAssembly()
{
    if(show_import_check != true) //check if it exists
        return;
    if(import_csv.size() <= 0) //everything is done
    {
        show_import_check = false; 
        return;
    }
    if(show_check_item == StateAlternativePicking::NONE && show_check_item_in_assemble == StateAlternativePicking::NONE) //everything is done for the last item
    {
        LOG_HISTORY("Next Item | " << std::to_string(import_csv.size()) << " left");
        item_to_check = import_csv.front();
        count_to_check = item_to_check.count;
        import_csv.pop();
        show_check_item = StateAlternativePicking::CHECK_EXISTANCE;
        show_check_item_in_assemble = StateAlternativePicking::CHECK_ALTERNATIVES_ASSEMBLE; // Check in Assemble if similar item exists
    }
                
}

void GuiDatabase::importItemToStorage()
{
    if(show_import_storage_check != true) //check if it exists
        return;
    if(import_csv.size() <= 0) //everything is done
    {
        show_import_storage_check = false; 
        return;
    }
    if(show_check_item == StateAlternativePicking::NONE) //everything is done for the last item
    {
        LOG_HISTORY("Next Item |" << std::to_string(import_csv.size()) << " left");
        item_to_check = import_csv.front();
        import_csv.pop();
        show_check_item = StateAlternativePicking::CHECK_EXISTANCE;
    }           
}

