#include "Gui/GuiDatabase.hpp"
#include "FileDialog.hpp"
#include "CSV.hpp"
#include "Gui/GuiTable.hpp"

void GuiDatabase::draw()
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New Database..."))
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

            if(ImGui::MenuItem("Load Database..."))
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
            if(ImGui::BeginMenu("Import From"))
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
            }
            if(ImGui::BeginMenu("Export As"))   //whole storage
            {
                if(ImGui::MenuItem("CSV")) 
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
                        CSV::exportCSV(path, result, {"Kategorie", "Value 1", "Value 2", "Package", "Beschreibung", "Hersteller", "Hersteller-Nr.", "Distributor", "Bestellnummer", "Verpackungseinheit", "Preis", "Preis/Stck", "Anzahl"});
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Bearbeiten"))
        {
            if(ImGui::BeginMenu("Bauteil"))
            {
                if(ImGui::MenuItem("Neu")) {show_new_item = true;}
                if(p_selected_item != nullptr)
                {
                    if(ImGui::MenuItem("Aktualisieren")) 
                    {
                        show_change_item = true; 
                        m_item_action = ItemAction::UPDATE_ITEM;
                    }
                    
                    if(ImGui::MenuItem("Reservieren")) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::RESERVE_ITEM;
                    }
                    if(ImGui::MenuItem("Reservierung verbraucht")) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::RESET_ITEM_RESERVATION;
                    }
                    if(ImGui::MenuItem("Reserviereung frei geben")) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::RELEASE_ITEM_RESERVATION;
                    }
                    if(ImGui::MenuItem("->Lager")) 
                    {
                        show_remove = true; 
                        m_count_action = CountAction::INCREASE_ITEM;
                    }
                    if(ImGui::MenuItem("<-Lager"))
                    {
                        show_remove = true; 
                        m_count_action = CountAction::DECREASE_ITEM;
                    }
                }
                ImGui::EndMenu();
            }
            
            if(ImGui::BeginMenu("Baugruppe"))
            {
                if(ImGui::MenuItem("Neu")) {show_assemble = true;}
                if(ImGui::MenuItem("Bauteil->"))
                {
                    if(p_selected_assemble != nullptr)
                    {
                        m_item_action = ItemAction::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE;
                        show_new_item = true;
                    }
                }
                if(ImGui::MenuItem("Baugruppe->")) 
                {
                    if(p_selected_assemble != nullptr)
                    {
                        show_remove = true;
                        m_count_action = CountAction::UPDATE_ASSEMBLE;
                    }
                    else
                        cmsg("[warning] No Assemble selected");
                }
                if(ImGui::MenuItem("Lager->"))
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
                
                if(ImGui::MenuItem("BOM")) 
                {
                    if(p_selected_assemble != nullptr)
                    {
                        show_bom = true;
                    }
                }
                ImGui::EndMenu();
            }
            
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Ansicht"))
        {
            if(ImGui::MenuItem("Lager", NULL ,&show_search)) {m_gui_database_updated = false;}
            if(ImGui::MenuItem("Baugruppen", NULL, &show_assemble_list)) {m_gui_database_updated = false;}
            if(ImGui::MenuItem("Konsole", NULL, &show_console)) {}
            if(ImGui::MenuItem("Info Box", NULL, &show_info)) {}
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Options"))
        {
            if(ImGui::MenuItem("Fullscreen", NULL, &fullscreen)) 
            {
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Darkmode", NULL, &m_dark_mode)) {m_dark_mode? ImGui::StyleColorsDark() : ImGui::StyleColorsLight(); }
            if(ImGui::MenuItem("About")) {}
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
    if(ImGui::Begin("Lager", &show_search))
    {
        static std::deque<Item> content;
        static ImVector<unsigned int> selection;
        static Item filter(true);
        static int storage_current_x_idx, storage_current_y_idx;
        static GuiTable table;
        static std::vector<uint8_t> category_filter(g_categories.size()); 
        ImGui::SameLine();
        if(ImGui::Button("Aktualisieren"))
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
            cmsg("[info] Finsished Searching");
        }
        ImGui::SameLine();
        if(ImGui::Button("Auflösen"))
        {
            cmsg("[warning] Not implemented yet");
        }
        ImGui::SameLine();
        if(ImGui::TreeNode("Filter"))
        {
            int i = 0;
            for(auto& cat : g_categories)
            {
                ImGui::Checkbox(std::get<0>(cat).c_str(), (bool*) &(category_filter[i]));
                ImGui::SameLine();
                i++;
            }
            ImGui::NewLine();
            if (ImGui::BeginCombo("Oberkategorie", filter.main_category.c_str()))
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
            if (ImGui::BeginCombo("Kategorie", (g_categories[filter.main_category])[filter.category].c_str()))
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
            ImGui::InputText("Value 1", &filter.value);
            ImGui::InputText("Value 2", &filter.value_2);
            ImGui::InputText("Package", &filter.package);
            ImGui::InputText("Beschreibung", &filter.description);
            ImGui::InputText("Hersteller", &filter.manufactor);
            ImGui::InputText("Hersteller-Nr.", &filter.manufactor_number);
            ImGui::InputText("Distributor", &filter.distributor);
            ImGui::InputText("Distributor-Nr.", &filter.shop_number);
            ImGui::InputInt("Verpackungseinheit", &filter.vpe);
            ImGui::InputInt("Anzahl", &filter.count);
            ImGui::InputDouble("Preis/VPE [Euro]", &filter.price, 0.01f, 1.0f, "%.3f");
            if (ImGui::BeginCombo("Lager-X", g_storage_x[storage_current_x_idx].c_str()))
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
            if (ImGui::BeginCombo("Lager-Y", g_storage_y[storage_current_y_idx].c_str()))
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
    if(ImGui::Begin("Bauteil"))
    {
        if(m_item_action == ItemAction::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE)
        {
            ImGui::Text("Dieses Bauteil wird zum Lager und zur Baugruppe '");
            ImGui::SameLine();
            ImGui::Text(p_selected_assemble->name.c_str());
            ImGui::SameLine();
            ImGui::Text("' hinzugefügt");
            ImGui::InputInt("Anzahl (Baugruppe)", &amount);
        }

        if (ImGui::BeginCombo("Oberkategorie", item.main_category.c_str()))
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

        if (ImGui::BeginCombo("Kategorie", (g_categories[item.main_category])[item.category].c_str()))
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
            ImGui::InputTextWithHint("Value 1", std::get<0>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str(), &item.value);
            ImGui::InputTextWithHint("Value 2", std::get<1>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str(), &item.value_2);
            ImGui::InputTextWithHint("Package", std::get<2>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str() ,&item.package);
            ImGui::InputTextWithHint("Beschreibung", std::get<3>(g_filter_hints.at(std::make_tuple(item.main_category, item.category))).c_str(), &item.description);
        }
        else
        {
            ImGui::InputText("Value 1", &item.value);
            ImGui::InputText("Value 2", &item.value_2);
            ImGui::InputText("Package" ,&item.package);
            ImGui::InputText("Beschreibung", &item.description);
        }
        
        ImGui::InputText("Hersteller", &item.manufactor);
        ImGui::InputText("Hersteller-Nr.", &item.manufactor_number);
        ImGui::InputText("Distributor", &item.distributor);
        ImGui::InputText("Distributor-Nr.", &item.shop_number);
        ImGui::Checkbox("Eingabe: Preis/Stück", &enter_price_per_unit);
        if(enter_price_per_unit)
        {
            ImGui::InputInt("Verpackungseinheit", &item.vpe, 1, 10);
            ImGui::Text(std::string("Preis/VPE [Euro]: " + std::to_string(item.price_per_unit*item.vpe)).c_str());
            ImGui::InputDouble("Preis/Stk [Euro]", &item.price_per_unit, 0.01f, 1.0f, "%.4f");
        }
        else
        {
            ImGui::InputInt("Verpackungseinheit", &item.vpe, 1, 10);
            ImGui::InputDouble("Preis/VPE [Euro]", &item.price, 0.01f, 1.0f, "%.4f");
            ImGui::Text(std::string("Preis/Stk [Euro]:").c_str());
            ImGui::SameLine();
            ImGui::Text(item.vpe == 0?"-":std::to_string(item.price/item.vpe).c_str());
        }    

        ImGui::InputInt("Anzahl", &item.count);

        if (ImGui::BeginCombo("Einheit", item.unit.c_str()))
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

        if (ImGui::BeginCombo("Lager-X", g_storage_x[storage_current_x_idx].c_str()))
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

        if (ImGui::BeginCombo("Lager-Y", g_storage_y[storage_current_y_idx].c_str()))
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
        ImGui::InputText("Datenblatt", &item.datasheet);
        ImGui::SameLine();
        if(ImGui::Button("Datenblatt..."))
        {
            item.datasheet = FileDialog::OpenFile("pdf (*.pdf), Word-Dokument (*.docx)\0*.pdf\0*.doc\0*.odt\0*.docx\0");
        }
        if(ImGui::Button("OK")) 
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
        if(ImGui::Button("Schließen")) 
        {
            item.clear();
            show_new_item = false;
        }
    }
    ImGui::End();
}

void GuiDatabase::showChangeItem()
{
    static bool enter_price_per_unit;
    if(ImGui::Begin("Bauteil"))
    {
        if (ImGui::BeginCombo("Oberkategorie", p_selected_item->main_category.c_str()))
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
        if (ImGui::BeginCombo("Kategorie", (g_categories[p_selected_item->main_category])[p_selected_item->category].c_str()))
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
        ImGui::InputText("Value 1", &p_selected_item->value);
        ImGui::InputText("Value 2", &p_selected_item->value_2);
        ImGui::InputText("Package", &p_selected_item->package);
        ImGui::InputText("Beschreibung", &p_selected_item->description);
        ImGui::InputText("Hersteller", &p_selected_item->manufactor);
        ImGui::InputText("Hersteller-Nr.", &p_selected_item->manufactor_number);
        ImGui::InputText("Distributor", &p_selected_item->distributor);
        ImGui::InputText("Distributor-Nr.", &p_selected_item->shop_number);
        ImGui::Checkbox("Eingabe: Preis/Stück", &enter_price_per_unit);
        if(enter_price_per_unit)
        {
            ImGui::InputInt("Verpackungseinheit", &p_selected_item->vpe, 1, 10);
            ImGui::Text(std::string("Preis/VPE [Euro]: " + std::to_string(p_selected_item->price_per_unit*p_selected_item->vpe)).c_str());
            ImGui::InputDouble("Preis/Stk [Euro]", &p_selected_item->price_per_unit, 0.01f, 1.0f, "%.4f");
        }
        else
        {
            ImGui::InputInt("Verpackungseinheit", &p_selected_item->vpe, 1, 10);
            ImGui::InputDouble("Preis/VPE [Euro]", &p_selected_item->price, 0.01f, 1.0f, "%.4f");
            ImGui::Text(std::string("Preis/Stk [Euro]:" + p_selected_item->vpe== 0?"-":std::to_string(p_selected_item->price/p_selected_item->vpe)).c_str());
        }        
        ImGui::InputInt("Anzahl", &p_selected_item->count);
        if (ImGui::BeginCombo("Einheit", p_selected_item->unit.c_str()))
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
        ImGui::InputText("Lager", &p_selected_item->storage_place);
        ImGui::InputText("Datenblatt", &p_selected_item->datasheet);
        ImGui::SameLine();
        if(ImGui::Button("Datenblatt..."))
        {
            p_selected_item->datasheet = FileDialog::OpenFile("pdf (*.pdf), Word-Dokument (*.docx)\0*.pdf\0*.doc\0*.odt\0*docx\0");
        }
        if(ImGui::Button("OK")) 
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
        if(ImGui::Button("Schließen")) 
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
    if(ImGui::Begin("Bauteil"))
    {
        ImGui::InputText("Name", &assemble.name);
        ImGui::NewLine();
        if(ImGui::Button("OK")) 
        {
            p_database->addAssemble(assemble);
            show_assemble = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Schließen")) 
        {
            show_assemble = false;
        }
    }
    ImGui::End();
}

void GuiDatabase::showInfo()
{
    if(ImGui::Begin("Info", &show_info))
    {
        if(p_database != nullptr)
        {
            ImGui::Text("Aktuelle Datenbank: ");
            ImGui::SameLine();
            ImGui::Text(p_database->getDatabasePath().c_str());
        }
        else
            ImGui::Text("Aktuelle Datenbank: -");
            
        if(p_selected_item != nullptr)
        {
            ImGui::Text("Ausgewähltes Bauteil: [");
            ImGui::SameLine();
            ImGui::Text(p_selected_item->category.c_str());
            ImGui::SameLine();
            ImGui::Text("]: ");
            ImGui::SameLine();
            ImGui::Text(p_selected_item->description.c_str());
        }
        else
            ImGui::Text("Ausgewähltes Bauteil: -");

        ImGui::NewLine();
        if(p_selected_assemble != nullptr)
        {
            ImGui::Text("Ausgewählte Gruppe: ");
            ImGui::SameLine();
            ImGui::Text(p_selected_assemble->name.c_str());
        }
            
        else
            ImGui::Text("Ausgewählte Baugruppe: -");

        ImGui::NewLine();
        if(ImGui::Button("Schließen")) 
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
    if(m_count_action == CountAction::NONE)
        return;
    if(ImGui::Begin("Entfernen", &show_remove))
    {
        if(m_count_action == CountAction::DECREASE_ITEM)
            ImGui::Text("Bauteil entfernen");
        if(m_count_action == CountAction::INCREASE_ITEM)
            ImGui::Text("Bauteil hinzufügen");
        if(m_count_action == CountAction::RESERVE_ITEM)
            ImGui::Text("Bauteil reservieren");
        if(m_count_action == CountAction::RESET_ITEM_RESERVATION)
            ImGui::Text("Reservierung verbraucht");
        if(m_count_action == CountAction::RELEASE_ITEM_RESERVATION)
            ImGui::Text("Reservierung freigeben");
        else if (m_count_action == CountAction::UPDATE_ASSEMBLE)
            ImGui::Text("Gerät produziert");
        else if (m_count_action == CountAction::ADD_ITEM_TO_ASSEMBLE)
            ImGui::Text("Bauteile zur Baugruppe hinzufügen");
        else
            ImGui::Text("");
        ImGui::InputInt("Anzahl", &amount);
        if(ImGui::Button("OK")) 
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
                if(p_selected_item->count - amount >= 0)
                {
                    p_selected_item->count -= amount;
                    p_selected_item->reserved += amount;
                    p_database->updateItem(*p_selected_item);
                }
                else
                    cmsg("[error] Too less items");
            }
            if(p_selected_item != nullptr && m_count_action == CountAction::RELEASE_ITEM_RESERVATION)
            {
                if(p_selected_item->reserved - amount >= 0)
                {
                    p_selected_item->count += amount;
                    p_selected_item->reserved -= amount;
                    p_database->updateItem(*p_selected_item);
                }
                else
                    cmsg("[error] Cant release more items than reserved");
            }
            if(p_selected_item != nullptr && m_count_action == CountAction::RESET_ITEM_RESERVATION)
            {
                if(p_selected_item->reserved - amount >= 0)
                {
                    p_selected_item->reserved -= amount;
                    p_database->updateItem(*p_selected_item);
                }
                else
                    cmsg("[error] Cant release more items than reserved");
            }
            show_remove = false;
            m_count_action = CountAction::NONE;
        }
        ImGui::SameLine();
        if(ImGui::Button("Schließen"))
        {
            show_remove = false;
            m_count_action = CountAction::NONE;
        }
    }
    ImGui::End();
}

void GuiDatabase::showAssembleList()
{
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    if(ImGui::Begin("Baugruppen", &show_assemble_list))
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
        if(ImGui::Button("Aktualisieren"))
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
        ImGui::Text(status.c_str());
        
        if(ImGui::BeginTable("search_results", 2, table_flags))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Name ", ImGuiTableColumnFlags_NoSort);
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
                        //ImGui::Text(std::get<0>(content.at(row_n))->getName().c_str());                     
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

    static std::deque<std::tuple<Item, int>> content = p_selected_assemble->bom;
    
    static ImVector<unsigned int> selection;
    static std::string status = "";
    static std::string title = "BOM - " + p_selected_assemble->name;
    static std::deque<bool> export_csv(content.size());

    if(ImGui::Begin(title.c_str(), &show_bom))
    {
        if(ImGui::Button("Aktualisieren"))
        {
            content = p_database->searchAssemble(*p_selected_assemble).bom;
            export_csv.resize(content.size());
        }
        ImGui::SameLine();
        if(ImGui::Button("Entfernen"))
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
        if(ImGui::Button("Export CSV"))
        {
            std::string path = FileDialog::SaveFile("CSV (*.csv)\0*.csv");
            if(path != "")
            {
                std::vector<std::string> result;
                for(size_t i = 0; i < content.size(); i++)
                {
                    if(export_csv[i])
                        result.push_back(std::get<0>(content[i]).serializeCSV());
                }
                CSV::exportCSV(path, result, {"Kategorie", "Value 1", "Value 2", "Package", "Beschreibung", "Hersteller", "Hersteller-Nr.", "Distributor", "Bestellnummer", "Verpackungseinheit", "Preis", "Anzahl"});
            }
            
        }
        ImGui::SameLine();
        if(ImGui::Button("Import CSV"))
        {
            std::string path = FileDialog::OpenFile("CSV (*.csv)\0*.csv");
            if(path != "")
            {
                auto item_strings = CSV::importCSV(path);
                for(auto& string : item_strings)
                {
                    Item i;
                    if(Item::deserializeCSV(i, string))
                    {
                        import_csv.push_back(i);                        
                    }
                }
                show_import_check = true;
            }
        }
        static ImGuiTableFlags table_flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
            //| ImGuiTableFlags_SizingFixedFit;
        
        ImGui::SameLine();
        ImGui::Text(status.c_str());
        
        if(ImGui::BeginTable("search_results", 17, table_flags))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Exp?", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Kategorie", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Value 1", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Value 2", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Package ", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Beschreibung ", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Anzahl (benötigt)", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Anzahl (vorhanden)", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Hersteller", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Hersteller-Nr.", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Distributor", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Bestell-Nr.", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Verpackungseinheit", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Preis/VPE [Euro]", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Lagerort", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Datenblatt", ImGuiTableColumnFlags_NoSort);
            
            ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
            ImGui::TableHeadersRow();

            if(!content.empty())
            {                
                ImGuiListClipper clipper;
                clipper.Begin(content.size());
                while (clipper.Step())
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                    {   
                        ImGui::TableNextRow();
                        unsigned int id = std::get<0>(content[row_n]).id;
                        bool selected = selection.contains(id);
                        
                        ImVec4 color (0.0f, 0.0f, 0.0f, 1.0f);
                    
                        if(std::get<1>(content[row_n]) > std::get<0>(content[row_n]).count)
                        {
                            color = ImVec4(.7f, .3f, .3f, 1.0f);
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(color));
                        }                            
                        else if(std::get<1>(content[row_n]) == std::get<0>(content[row_n]).count)
                        {
                            color = ImVec4(.7f, 0.7f, 0.3f, 1.0f);
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(color));
                        }
                        ImGui::TableSetColumnIndex(0);
                        //ImGui::Text(std::get<0>(content.at(row_n))->getName().c_str());                     
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
                                p_selected_item = &std::get<0>(content[row_n]);
                            }
                        }
                        ImGui::TableSetColumnIndex(1);
                        ImGui::PushID(&(export_csv[row_n]));
                        ImGui::Checkbox("###", &(export_csv[row_n]));
                        ImGui::PopID();
                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextUnformatted(std::get<0>(content[row_n]).category.c_str());
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextUnformatted(std::get<0>(content[row_n]).value.c_str());
                        ImGui::TableSetColumnIndex(4);
                        ImGui::TextUnformatted(std::get<0>(content[row_n]).value_2.c_str());
                        ImGui::TableSetColumnIndex(5);
                        ImGui::TextUnformatted(std::get<0>(content[row_n]).package.c_str());
                        ImGui::TableSetColumnIndex(6);
                        ImGui::TextUnformatted(std::get<0>(content[row_n]).description.c_str());
                        ImGui::TableSetColumnIndex(7);
                        ImGui::Text(std::to_string(std::get<1>(content[row_n])).c_str());
                        ImGui::TableSetColumnIndex(8);
                        ImGui::Text(std::to_string(std::get<0>(content[row_n]).count).c_str());
                        ImGui::TableSetColumnIndex(9);
                        ImGui::Text(std::get<0>(content[row_n]).manufactor.c_str());
                        ImGui::TableSetColumnIndex(10);
                        ImGui::Text(std::get<0>(content[row_n]).manufactor_number.c_str());
                        ImGui::TableSetColumnIndex(11);
                        ImGui::Text(std::get<0>(content[row_n]).distributor.c_str());
                        ImGui::TableSetColumnIndex(12);
                        ImGui::Text(std::get<0>(content[row_n]).shop_number.c_str());
                        ImGui::TableSetColumnIndex(13);
                        ImGui::Text(std::to_string(std::get<0>(content[row_n]).vpe).c_str());
                        ImGui::TableSetColumnIndex(14);
                        ImGui::Text(std::to_string(std::get<0>(content[row_n]).price).c_str());
                        ImGui::TableSetColumnIndex(15);
                        ImGui::Text(std::get<0>(content[row_n]).storage_place.c_str());
                        ImGui::TableSetColumnIndex(16);
                        ImGui::Text(std::get<0>(content[row_n]).datasheet.c_str());
                    }
            }
            ImGui::EndTable();
        }
        if(p_database->isUpdated())
        {
            cmsg("[info] Automated Update");
            m_gui_database_updated = true;
            content.clear();
            content = p_database->searchAssemble(*p_selected_assemble).bom;
            export_csv.resize(content.size());
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
            ImGui::Text("Es existiert bereits ein Bauteil vom Hersteller '");
            ImGui::SameLine();
            ImGui::Text(item_to_check.manufactor.c_str());
            ImGui::SameLine();
            ImGui::Text("'\nund mit der Herstellernummer'");
            ImGui::SameLine();
            ImGui::Text(item_to_check.manufactor_number.c_str());
            ImGui::SameLine();
            ImGui::Text("'");
            ImGui::Separator();
            if (ImGui::Button("Abbrechen", ImVec2(120, 0))) 
            { 
                ImGui::CloseCurrentPopup(); 
                show_check_item = StateAlternativePicking::NONE; 
                show_check_item_in_assemble = StateAlternativePicking::NONE;
            } // go to state 0
            ImGui::SameLine();
            if (ImGui::Button("Trotzdem hinzufügen", ImVec2(150, 0))) 
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
        ImGui::OpenPopup("Bauteilvorschläge");
    }

    if(show_check_item == StateAlternativePicking::SELECT_ALTERNATIVE) //State 4: Select alternative
    {
        static ImVector<unsigned int> selection;
        static Item* alternative = nullptr;
        
        if(content.size() > 0) //Recommondations
        {
            if (ImGui::BeginPopupModal("Bauteilvorschläge", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Es existiern ähnliche Bauteile im Lager (");
                ImGui::SameLine();
                ImGui::Text(item_to_check.category.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.value.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.value_2.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.package.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.description.c_str());
                ImGui::SameLine();
                ImGui::Text(")");
                ImGui::Separator();
                if (ImGui::Button("OK", ImVec2(120, 0))) 
                { 
                    //select Item()
                    if(selection.size()>0)
                    {
                        //TODO: What happens if i check another Object: Update amount?
                        alternative->count += item_to_check.count;
                        p_database->updateItem(*alternative);
                        selection.clear();          
                        ImGui::CloseCurrentPopup();
                        item_to_check = *alternative; //needed for assembly
                        show_check_item =  StateAlternativePicking::FINISHED;
                    }
                    else
                    {
                        cmsg("[error] Kein Alternativbauteil ausgewählt");
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button("Neu anlegen"))
                {
                    item_to_check.id = p_database->addItem(item_to_check);
                    ImGui::CloseCurrentPopup();
                    show_check_item = StateAlternativePicking::FINISHED; //go to state 5 in 
                }
                if(ImGui::Button("Abbrechen"))
                {
                    show_check_item = StateAlternativePicking::NONE;
                }

                table.draw(content, selection, alternative);
                
                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }
        }
        else
        {
            item_to_check.id = p_database->addItem(item_to_check); 
            show_check_item = StateAlternativePicking::FINISHED; //State 5 in addItemToAssembleWithCheck
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

    if(show_check_item_in_assemble == StateAlternativePicking::CHECK_ALTERNATIVES_ASSEMBLE) //state 1 : checl for alternatives
    {
        search.clear();
        // search.manufactor = item_to_check.manufactor;
        // search.manufactor_number = item_to_check.manufactor_number;
        content = p_database->searchItemByID(item_to_check.id);
        if(content.size() <= 0) //Add the "real" Item (Correct ID)
        {
            cmsg("[error] Bauelement konnte nicht hinzugefügt werden");
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
        ImGui::OpenPopup("Bauteilvorschläge");
        cmsg("[info] Suche nach Alternativen");
    }

    if(show_check_item_in_assemble == StateAlternativePicking::SELECT_ALTERNATIVE_ASSEMBLE) //State 4: Select alternative
    {
        static ImVector<unsigned int> selection;
        static Item* alternative = nullptr;
        
        if(content.size() > 0) //Recommondations
        {
            if (ImGui::BeginPopupModal("Bauteilvorschläge", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Es existiern ähnliche Bauteile im Lager (");
                ImGui::SameLine();
                ImGui::Text(item_to_check.category.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.value.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.value_2.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.package.c_str());
                ImGui::SameLine();
                ImGui::Text(", ");
                ImGui::SameLine();
                ImGui::Text(item_to_check.description.c_str());
                ImGui::SameLine();
                ImGui::Text(")");
                ImGui::Separator();
                if (ImGui::Button("OK", ImVec2(120, 0))) 
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
                        cmsg("[error] Kein Alternativbauteil ausgewählt");
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button("Neu anlegen"))
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
                        cmsg("[error] Item exits already. Trying to update amount");
                        show_check_item_in_assemble = StateAlternativePicking::NONE;
                    }
                    
                }
                ImGui::SameLine();
                if(ImGui::Button("Abbrechen"))
                {
                    show_check_item_in_assemble = StateAlternativePicking::NONE;
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
        item_to_check = import_csv.back();
        import_csv.pop_back();
        show_check_item = StateAlternativePicking::CHECK_EXISTANCE;
        show_check_item_in_assemble = StateAlternativePicking::SELECT_ALTERNATIVE_ASSEMBLE;
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
        item_to_check = import_csv.back();
        import_csv.pop_back();
        show_check_item = StateAlternativePicking::CHECK_EXISTANCE;
    }           
}

