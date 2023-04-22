#include "GuiDatabase.hpp"
#include "FileDialog.hpp"
#include "CSV.hpp"


void GuiDatabase::draw(sf::RenderWindow &w)
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
                    p_database->initStorage();
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
                }
                    
                // show_file_explorer = true;
                // std::filesystem::path selected_path = std::filesystem::absolute(std::filesystem::current_path());
                // file_explorer.PickFile(std::filesystem::absolute(std::filesystem::current_path()), [&](const std::filesystem::path& selected_path) {/*Store data*/});
            }
            ImGui::Separator();
            if(ImGui::BeginMenu("Import From"))
            {
                if(ImGui::MenuItem("CSV")) {}
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Export As"))
            {
                if(ImGui::MenuItem("CSV")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Bearbeiten"))
        {
            if(ImGui::BeginMenu("Bauteil"))
            {
                if(ImGui::MenuItem("Bauteil aktualisieren")) 
                {
                    if(p_selected_item != nullptr)
                    {
                        show_change_item = true; 
                        m_action = Action::UPDATE_ITEM;
                    }
                    else
                        cmsg("[warning] No Item selected");
                }
                if(ImGui::MenuItem("->Lager")) {show_item = true; m_action = Action::NEW_ITEM;}
                if(ImGui::MenuItem("Lager->")) 
                {
                    if(p_selected_item != nullptr)
                    {
                        show_remove = true; 
                        m_action = Action::UPDATE_ITEM;
                    }
                    else
                        cmsg("[warning] No Item selected");
                };
                ImGui::EndMenu();
            }
            
            if(ImGui::BeginMenu("Baugruppe"))
            {
                if(ImGui::MenuItem("Neu")) {show_assemble = true; m_action = Action::NEW_ASSEMBLE;}
                if(ImGui::MenuItem("Bauteil->"))
                {
                    if(p_selected_assemble != nullptr)
                    {
                        m_action = Action::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE;
                        show_item = true;
                    }
                }
                if(ImGui::MenuItem("Baugruppe->")) 
                {
                    if(p_selected_assemble != nullptr)
                    {
                        show_remove = true;
                        m_action = Action::UPDATE_ASSEMBLE;
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
                            m_action = Action::ADD_ITEM_TO_ASSEMBLE;
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
                if(fullscreen)
                {
                    w.create(sf::VideoMode(1000,1000), "STORAGE", sf::Style::Fullscreen);
                }                        
                else
                {
                    w.create(sf::VideoMode(1200,800), "STORAGE");
                }
                w.setFramerateLimit(144);
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Darkmode", NULL, &m_dark_mode)) {m_dark_mode? ImGui::StyleColorsDark() : ImGui::StyleColorsLight(); }
            if(ImGui::MenuItem("About")) {}
            ImGui::EndMenu();
        }
        
        
        ImGui::EndMainMenuBar();
        
    } 

    if(show_console)
        showConsole(w);
    if(show_info)
        showInfo(w);
    
    
    
      
    if(p_database->isUpdated())
    {
        m_gui_database_updated = false;
    }
   
    if(p_database != nullptr)
    {
        if(show_search)
        showSearch(w);
        if(show_item)
            showItem(w);
        if(show_remove)
            showRemove(w);
        if(show_assemble)
            showAssemble(w);
        if(show_bom)
            showBOM(w);
        if(show_assemble_list)
            showAssembleList(w);
        if(show_change_item)
            showChangeItem(w);
        if(show_check_item > 0)
            addItemToStorageWithCheck();
        if(show_check_item > 4)
            addItemToAssembleWithCheck();
    }
    if(m_gui_database_updated) //updated Database is also updated in Gui
        p_database->updateRecognized();      
    
}

void GuiDatabase::showConsole(sf::RenderWindow &w)
{
    p_console->draw();
}

void GuiDatabase::showSearch(sf::RenderWindow &w)
{
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    
    if(ImGui::Begin("Lager", &show_search))
    {
        static ImGuiTableFlags table_flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
            //| ImGuiTableFlags_SizingFixedFit;
        static std::deque<Item> content;
        static ImVector<unsigned int> selection;
        static std::string status = "";
        static Item filter;
        static int item_current_idx, storage_current_x_idx, storage_current_y_idx;
        ImGui::SameLine();
        if(ImGui::Button("Search"))
        {
            content.resize(0);
            selection.resize(0);
            status = "Searching...";
            filter.category = g_filter_categories[item_current_idx];
            filter.storage_place = g_filter_storage_x[storage_current_x_idx] + "|" + g_filter_storage_y[storage_current_y_idx];
            content = p_database->searchItem(filter);
            status = "Ready...";
        }
        ImGui::SameLine();
        ImGui::Text(status.c_str());
        if(ImGui::TreeNode("Filter"))
        {
            if (ImGui::BeginCombo("Kategorie", g_filter_categories[item_current_idx].c_str()))
            {
                for (int n = 0; n < g_filter_categories.size(); n++)
                {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(g_filter_categories[n].c_str(), is_selected))
                    {
                        item_current_idx = n;
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::InputText("Value", &filter.value);
            ImGui::InputText("Package", &filter.package);
            ImGui::InputText("Beschreibung", &filter.properties);
            ImGui::InputText("Hersteller", &filter.manufactor);
            ImGui::InputText("Hersteller-Nr.", &filter.manufactor_number);
            ImGui::InputText("Distributor", &filter.distributor);
            ImGui::InputText("Distributor-Nr.", &filter.shop_number);
            ImGui::InputText("Verpackungseinheit", &filter.vpe);
            ImGui::InputInt("Anzahl", &filter.count);
            ImGui::InputDouble("Preis/VPE [Euro]", &filter.price, 0.01f, 1.0f, "%.3f");
            if (ImGui::BeginCombo("Lager-X", g_filter_storage_x[storage_current_x_idx].c_str()))
            {
                for (int n = 0; n < g_filter_storage_x.size(); n++)
                {
                    const bool is_selected = (storage_current_x_idx == n);
                    if (ImGui::Selectable(g_filter_storage_x[n].c_str(), is_selected))
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
            if (ImGui::BeginCombo("Lager-Y", g_filter_storage_y[storage_current_y_idx].c_str()))
            {
                for (int n = 0; n < g_filter_storage_y.size(); n++)
                {
                    const bool is_selected = (storage_current_y_idx == n);
                    if (ImGui::Selectable(g_filter_storage_y[n].c_str(), is_selected))
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
        if(ImGui::BeginTable("search_results", 14, table_flags))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort);
            
            ImGui::TableSetupColumn("Kategorie", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Wert ", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Package ", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Beschreibung ", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Anzahl", ImGuiTableColumnFlags_NoSort);
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
                // if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
                // if (sorts_specs->SpecsDirty)
                // {
                //     //order of sorted list: forward or backward
                //     if (content->size() > 1)
                //         std::reverse(content->begin(), content->end());
                //     sorts_specs->SpecsDirty = false;
                // }
                ImGuiListClipper clipper;
                clipper.Begin(content.size());
                while (clipper.Step())
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                    {   
                        ImGui::TableNextRow();
                        bool colored = false;
                        ImVec4 color(0.0f, 0.0f, 0.0f, 1.0f);
                        if(content[row_n].count < 0)
                        {
                            color = ImVec4(0.7f, 0.3f, 0.3f, .5f);
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,  ImGui::GetColorU32(color));
                        }
                            
                        else if(content[row_n].count == 0)
                        {
                            color = ImVec4(0.7f, .7f, 0.3f, .5f);
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,  ImGui::GetColorU32(color));
                        }
                        
                        unsigned int id = content[row_n].id;
                        bool selected = selection.contains(id);
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
                                p_selected_item = &content[row_n];
                            }
                        }
                        
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(content[row_n].category.c_str());
                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextUnformatted(content[row_n].value.c_str());
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextUnformatted(content[row_n].package.c_str());
                        ImGui::TableSetColumnIndex(4);
                        ImGui::TextUnformatted(content[row_n].properties.c_str());
                        ImGui::TableSetColumnIndex(5);
                        ImGui::TextUnformatted(std::to_string(content[row_n].count).c_str());
                        ImGui::TableSetColumnIndex(6);
                        ImGui::TextUnformatted(content[row_n].manufactor.c_str());
                        ImGui::TableSetColumnIndex(7);
                        ImGui::TextUnformatted(content[row_n].manufactor_number.c_str());
                        ImGui::TableSetColumnIndex(8);
                        ImGui::TextUnformatted(content[row_n].distributor.c_str());
                        ImGui::TableSetColumnIndex(9);
                        ImGui::TextUnformatted(content[row_n].shop_number.c_str());
                        ImGui::TableSetColumnIndex(10);
                        ImGui::TextUnformatted(content[row_n].vpe.c_str());
                        ImGui::TableSetColumnIndex(11);
                        ImGui::TextUnformatted(std::to_string(content[row_n].price).c_str());
                        ImGui::TableSetColumnIndex(12);
                        ImGui::TextUnformatted(content[row_n].storage_place.c_str());
                        ImGui::TableSetColumnIndex(13);
                        ImGui::TextUnformatted(content[row_n].datasheet.c_str());
                        
                    }
            }
            ImGui::EndTable();
        }

        if(p_database->isUpdated()) //Automated update
        {
            m_gui_database_updated = true;
            filter.category = g_filter_categories[item_current_idx];
            filter.storage_place = g_filter_storage_x[storage_current_x_idx] + "|" + g_filter_storage_y[storage_current_y_idx];
            content = p_database->searchItem(filter);
            cmsg("[info] Automated Update");
        }
    }
    ImGui::End();
}

/**
 * @brief Used to add a Item to Database
 * 
 * @param w 
 */
void GuiDatabase::showItem(sf::RenderWindow &w)
{
    static Item item;
    static unsigned int item_current_idx = 0;
    static unsigned int storage_current_x_idx = 0;
    static unsigned int storage_current_y_idx = 0;
    static int amount;
    if(ImGui::Begin("Bauteil"))
    {
        if(m_action == Action::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE)
        {
            ImGui::Text("Dieses Bauteil wird zum Lager und zur Baugruppe '");
            ImGui::SameLine();
            ImGui::Text(p_selected_assemble->name.c_str());
            ImGui::SameLine();
            ImGui::Text("' hinzugefügt");
            ImGui::InputInt("Anzahl (Baugruppe)", &amount);
        }

        if (ImGui::BeginCombo("Kategorie", g_categories[item_current_idx].c_str()))
        {
            for (int n = 0; n < g_categories.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(g_categories[n].c_str(), is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::InputTextWithHint("Value", std::get<0>(g_filter_hints.at(g_categories[item_current_idx])).c_str(), &item.value);
        ImGui::InputTextWithHint("Package", std::get<1>(g_filter_hints.at(g_categories[item_current_idx])).c_str() ,&item.package);
        ImGui::InputTextWithHint("Beschreibung", std::get<2>(g_filter_hints.at(g_categories[item_current_idx])).c_str(), &item.properties);
        ImGui::InputText("Hersteller", &item.manufactor);
        ImGui::InputText("Hersteller-Nr.", &item.manufactor_number);
        ImGui::InputText("Distributor", &item.distributor);
        ImGui::InputText("Distributor-Nr.", &item.shop_number);
        ImGui::InputText("Verpackungseinheit", &item.vpe);
        ImGui::InputInt("Anzahl", &item.count);
        ImGui::InputDouble("Preis/VPE [Euro]", &item.price, 0.01f, 1.0f, "%.3f");
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
            item.datasheet = FileDialog::OpenFile("pdf (*.pdf), Word-Dokument (*.docx)\0*.pdf\0*.doc\0*.odt\0*docx\0");
        }
        if(ImGui::Button("OK")) 
        {   
            item.category = g_categories[item_current_idx];
            item.storage_place = g_storage_x[storage_current_x_idx] + "|" + g_storage_y[storage_current_y_idx];
            
            show_check_item = 1;
            item_to_check = item;
            if(m_action == Action::ADD_ITEM_TO_ASSEMBLE_AND_STORAGE)
            {
                item_to_check = item;
                count_to_check = amount;
                show_check_item_in_assemble++;
            }
            item.clear();
            show_item = false; // close if ready?
        }
        ImGui::SameLine();
        if(ImGui::Button("Schließen")) 
        {
            show_item = false;
        }
    }
    ImGui::End();
}

void GuiDatabase::showChangeItem(sf::RenderWindow &w)
{
    static unsigned int item_current_idx = 0;
    static unsigned int storage_current_x_idx = 0;
    static unsigned int storage_current_y_idx = 0;
    if(ImGui::Begin("Bauteil"))
    {
        ImGui::Text(p_selected_item->category.c_str());
        ImGui::InputText("Value", &p_selected_item->value);
        ImGui::InputText("Package", &p_selected_item->package);
        ImGui::InputText("Beschreibung", &p_selected_item->properties);
        ImGui::Text(p_selected_item->manufactor.c_str());
        ImGui::InputText("Hersteller-Nr.", &p_selected_item->manufactor_number);
        ImGui::InputText("Distributor", &p_selected_item->distributor);
        ImGui::InputText("Distributor-Nr.", &p_selected_item->shop_number);
        ImGui::InputText("Verpackungseinheit", &p_selected_item->vpe);
        ImGui::InputInt("Anzahl", &p_selected_item->count);
        ImGui::InputDouble("Preis/VPE [Euro]", &p_selected_item->price, 0.01f, 1.0f, "%.3f");
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

        ImGui::NewLine();
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
        ImGui::InputText("Datenblatt", &p_selected_item->datasheet);
        ImGui::SameLine();
        if(ImGui::Button("Datenblatt..."))
        {
            p_selected_item->datasheet = FileDialog::OpenFile("pdf (*.pdf), Word-Dokument (*.docx)\0*.pdf\0*.doc\0*.odt\0*docx\0");
        }
        if(ImGui::Button("OK")) 
        {   
            p_selected_item->category = g_categories[item_current_idx];
            p_selected_item->storage_place = g_storage_x[storage_current_x_idx] + "|" + g_storage_y[storage_current_y_idx];
            p_database->updateItem(*p_selected_item);
            show_change_item = false; // close if ready?
        }
        ImGui::SameLine();
        if(ImGui::Button("Schließen")) 
        {
            show_change_item = false;
        }
    }
    ImGui::End();
}

void GuiDatabase::showAssemble(sf::RenderWindow &w)
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

void GuiDatabase::showInfo(sf::RenderWindow &w)
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
            ImGui::Text(p_selected_item->properties.c_str());
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

void GuiDatabase::showRemove(sf::RenderWindow &w)
{
    static int amount = 0;
    static bool add = false;
    if(ImGui::Begin("Entfernen", &show_remove))
    {
        if(m_action == Action::UPDATE_ITEM)
            ImGui::Text("Bauteil entfernen");
        else if (m_action == Action::UPDATE_ASSEMBLE)
            ImGui::Text("Gerät produziert");
        else if (m_action == Action::ADD_ITEM_TO_ASSEMBLE)
            ImGui::Text("Bauteile zur Baugruppe hinzufügen");
        else
            ImGui::Text("");
        if(m_action == Action::UPDATE_ITEM) 
            ImGui::Checkbox("Hinzufügen/Entfernen", &add);
        ImGui::InputInt("Anzahl", &amount);
        if(ImGui::Button("OK")) 
        {
            if(p_selected_item != nullptr && p_selected_item != nullptr)
            {
                if(m_action == Action::UPDATE_ITEM)
                {
                    if(add)
                        (p_selected_item->count) += amount;
                    else
                        (p_selected_item->count) -= amount;
                    p_database->updateItem(*p_selected_item);
                }
                    
                else if (m_action == Action::UPDATE_ASSEMBLE)
                {
                    *p_selected_assemble = p_database->searchAssemble(*p_selected_assemble);
                    for(auto& item : p_selected_assemble->bom)
                    {
                        std::get<0>(item).count -= (std::get<1>(item)*amount);
                        p_database->updateItem(std::get<0>(item));
                    }
                }
                else if (m_action == Action::ADD_ITEM_TO_ASSEMBLE)
                {
                    // int i = p_database->itemExistsInAssemble(*p_selected_assemble, *p_selected_item);
                    // if(i > 0)
                    // {
                    //     cmsg("[warning] Bauteil existiert bereits. Anzahl aktualisiert");
                    //     p_database->updateItemInAssemble(*p_selected_assemble, *p_selected_item, amount);
                    // }
                    // else
                    // {
                    item_to_check = *p_selected_item;
                    count_to_check = amount;
                    show_check_item = 5; 
                    show_check_item_in_assemble++;
                        //old: p_database->addItemToAssembleWithCheck(*p_selected_assemble, *p_selected_item, amount);
                    // }
                }
            }
            show_remove = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Schließen"))
        {
            show_remove = false;
        }
    }
    ImGui::End();
}

void GuiDatabase::showAssembleList(sf::RenderWindow &w)
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

void GuiDatabase::showBOM(sf::RenderWindow &w)
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
                CSV::exportCSV(path, result, {"Kategorie", "Value", "Package", "Beschreibung", "Hersteller", "Hersteller-Nr.", "Distributor", "Bestellnummer", "Verpackungseinheit", "Preis", "Anzahl"});
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
        
        if(ImGui::BeginTable("search_results", 16, table_flags))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Exp?", ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupColumn("Kategorie", ImGuiTableColumnFlags_DefaultSort);
            ImGui::TableSetupColumn("Wert ", ImGuiTableColumnFlags_NoSort);
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
                        ImGui::TextUnformatted(std::get<0>(content[row_n]).package.c_str());
                        ImGui::TableSetColumnIndex(5);
                        ImGui::TextUnformatted(std::get<0>(content[row_n]).properties.c_str());
                        ImGui::TableSetColumnIndex(6);
                        ImGui::Text(std::to_string(std::get<1>(content[row_n])).c_str());
                        ImGui::TableSetColumnIndex(7);
                        ImGui::Text(std::to_string(std::get<0>(content[row_n]).count).c_str());
                        ImGui::TableSetColumnIndex(8);
                        ImGui::Text(std::get<0>(content[row_n]).manufactor.c_str());
                        ImGui::TableSetColumnIndex(9);
                        ImGui::Text(std::get<0>(content[row_n]).manufactor_number.c_str());
                        ImGui::TableSetColumnIndex(10);
                        ImGui::Text(std::get<0>(content[row_n]).distributor.c_str());
                        ImGui::TableSetColumnIndex(11);
                        ImGui::Text(std::get<0>(content[row_n]).shop_number.c_str());
                        ImGui::TableSetColumnIndex(12);
                        ImGui::Text(std::get<0>(content[row_n]).vpe.c_str());
                        ImGui::TableSetColumnIndex(13);
                        ImGui::Text(std::to_string(std::get<0>(content[row_n]).price).c_str());
                        ImGui::TableSetColumnIndex(14);
                        ImGui::Text(std::get<0>(content[row_n]).storage_place.c_str());
                        ImGui::TableSetColumnIndex(15);
                        ImGui::Text(std::get<0>(content[row_n]).datasheet.c_str());
                    }
            }
            ImGui::EndTable();
        }
        if(p_database->isUpdated())
        {
            cmsg("[info] Automated Update");
            m_gui_database_updated = true;
            content = p_database->searchAssemble(*p_selected_assemble).bom;
            export_csv.resize(content.size());
        }
    }
    ImGui::End();
    
}

void GuiDatabase::addItemToStorageWithCheck()
{   
    static Item search;
    static std::deque<Item> content;
    if(show_check_item>4)
        return;
    if(show_check_item == 1) //state 1: check if already exists
    {   
        search.clear();
        search.manufactor = item_to_check.manufactor;
        search.manufactor_number = item_to_check.manufactor_number;
        content = p_database->searchItem(search);
        if(content.size() > 0)
        {
            ImGui::OpenPopup("Error");
            show_check_item++;
        }
        else
            show_check_item = 3; //go direct to stage 3
    }
        
    //Hard Check/ State 2 / PopUp
    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Es existiert bereits ein Bauteil vom selben Hersteller\nund mit der selben Herstellernummer");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); show_check_item = 0;} // go to state 3
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
        
    if(show_check_item == 3) //state 3 : checl for alternatives
    {
        //Softcheck
        search.clear();
        search.category = item_to_check.category;
        search.value = item_to_check.value;
        content = p_database->searchItem(search);
        show_check_item++;
        ImGui::OpenPopup("Bauteilvorschläge");
    }
    if(show_check_item == 4) //State 4: Select alternative
    {
        static ImVector<unsigned int> selection;
        static Item* alternative = nullptr;
        
        if(content.size() > 0) //Recommondations
        {
            if (ImGui::BeginPopupModal("Bauteilvorschläge", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Es existiern ähnliche Bauteile im Lager");
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
                        show_check_item++;
                    }
                    else
                    {
                        cmsg("[error] Kein Alternativbauteil ausgewählt");
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button("Neu anlegen"))
                {
                    p_database->addItem(item_to_check);
                    ImGui::CloseCurrentPopup();
                    show_check_item++; //go to state 5 in 
                }
                static ImGuiTableFlags table_flags =
                ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
                | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
                | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
                | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
                if(ImGui::BeginTable("search_results", 14, table_flags))
                {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort);
                    
                    ImGui::TableSetupColumn("Kategorie", ImGuiTableColumnFlags_DefaultSort);
                    ImGui::TableSetupColumn("Wert ", ImGuiTableColumnFlags_NoSort);
                    ImGui::TableSetupColumn("Package ", ImGuiTableColumnFlags_NoSort);
                    ImGui::TableSetupColumn("Beschreibung ", ImGuiTableColumnFlags_NoSort);
                    ImGui::TableSetupColumn("Anzahl", ImGuiTableColumnFlags_NoSort);
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
                        // if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
                        // if (sorts_specs->SpecsDirty)
                        // {
                        //     //order of sorted list: forward or backward
                        //     if (content->size() > 1)
                        //         std::reverse(content->begin(), content->end());
                        //     sorts_specs->SpecsDirty = false;
                        // }
                        ImGuiListClipper clipper;
                        clipper.Begin(content.size());
                        while (clipper.Step())
                            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                            {   
                                ImGui::TableNextRow();
                                bool colored = false;
                                ImVec4 color(0.0f, 0.0f, 0.0f, 1.0f);
                                if(content[row_n].count < 0)
                                {
                                    color = ImVec4(0.7f, 0.3f, 0.3f, .5f);
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,  ImGui::GetColorU32(color));
                                }
                                    
                                else if(content[row_n].count == 0)
                                {
                                    color = ImVec4(0.7f, .7f, 0.3f, .5f);
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,  ImGui::GetColorU32(color));
                                }
                                
                                unsigned int id = content[row_n].id;
                                bool selected = selection.contains(id);
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
                                        alternative = &content[row_n];
                                    }
                                }
                                
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextUnformatted(content[row_n].category.c_str());
                                ImGui::TableSetColumnIndex(2);
                                ImGui::TextUnformatted(content[row_n].value.c_str());
                                ImGui::TableSetColumnIndex(3);
                                ImGui::TextUnformatted(content[row_n].package.c_str());
                                ImGui::TableSetColumnIndex(4);
                                ImGui::TextUnformatted(content[row_n].properties.c_str());
                                ImGui::TableSetColumnIndex(5);
                                ImGui::TextUnformatted(std::to_string(content[row_n].count).c_str());
                                ImGui::TableSetColumnIndex(6);
                                ImGui::TextUnformatted(content[row_n].manufactor.c_str());
                                ImGui::TableSetColumnIndex(7);
                                ImGui::TextUnformatted(content[row_n].manufactor_number.c_str());
                                ImGui::TableSetColumnIndex(8);
                                ImGui::TextUnformatted(content[row_n].distributor.c_str());
                                ImGui::TableSetColumnIndex(9);
                                ImGui::TextUnformatted(content[row_n].shop_number.c_str());
                                ImGui::TableSetColumnIndex(10);
                                ImGui::TextUnformatted(content[row_n].vpe.c_str());
                                ImGui::TableSetColumnIndex(11);
                                ImGui::TextUnformatted(std::to_string(content[row_n].price).c_str());
                                ImGui::TableSetColumnIndex(12);
                                ImGui::TextUnformatted(content[row_n].storage_place.c_str());
                                ImGui::TableSetColumnIndex(13);
                                ImGui::TextUnformatted(content[row_n].datasheet.c_str());
                                
                            }
                    }
                    ImGui::EndTable();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }
        }
        else
        {
            p_database->addItem(item_to_check); 
            show_check_item++; //State 5 in addItemToAssembleWithCheck
        }
    }
}

void GuiDatabase::addItemToAssembleWithCheck()
{
    static Item search;
    static std::deque<Item> content;
    if(show_check_item_in_assemble == 0 && show_check_item > 4) //no need to check for alternatives
    {
        show_check_item = 0;
        return;
    }
    if(show_check_item_in_assemble == 1) //state 1 : checl for alternatives
    {
        search.clear();
        search.manufactor = item_to_check.manufactor;
        search.manufactor_number = item_to_check.manufactor_number;
        content = p_database->searchItem(search);
        if(content.size() <= 0) //Add the "real" Item (Correct ID)
        {
            cmsg("[error] Bauelement konnte nichthinzugefügt werden");
            show_check_item = 0;
            return;
        }
        else
            item_to_check = content[0]; //go direct to stage 3

        search.clear();
        search.category = item_to_check.category;
        search.value = item_to_check.value;
        content = p_database->searchItemInAssemble(*p_selected_assemble, search);
        show_check_item_in_assemble++;
        ImGui::OpenPopup("Bauteilvorschläge");
        cmsg("[info] Suche nach Alternativen");
    }

    if(show_check_item_in_assemble == 2) //State 4: Select alternative
    {
        static ImVector<unsigned int> selection;
        static Item* alternative = nullptr;
        
        if(content.size() > 0) //Recommondations
        {
            if (ImGui::BeginPopupModal("Bauteilvorschläge", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Es existiern ähnliche Bauteile in dieser Baugruppe");
                ImGui::Separator();
                if (ImGui::Button("OK", ImVec2(120, 0))) 
                { 
                    //select Item()
                    if(selection.size()>0)
                    {
                        //TODO: What happens if i check another Object: Update amount?
                        
                        p_database->addItemToAssemble(*p_selected_assemble, *alternative, count_to_check);
                        selection.clear();          
                        ImGui::CloseCurrentPopup();
                        show_check_item_in_assemble = 0;
                    }
                    else
                    {
                        cmsg("[error] Kein Alternativbauteil ausgewählt");
                    }
                }
                ImGui::SameLine();
                if(ImGui::Button("Neu anlegen"))
                {
                    p_database->addItemToAssemble(*p_selected_assemble, item_to_check, count_to_check);
                    ImGui::CloseCurrentPopup();
                    show_check_item_in_assemble = 0;
                }
                static ImGuiTableFlags table_flags =
                ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
                | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
                | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
                | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
                if(ImGui::BeginTable("search_results", 14, table_flags))
                {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort);
                    ImGui::TableSetupColumn("Kategorie", ImGuiTableColumnFlags_DefaultSort);
                    ImGui::TableSetupColumn("Wert ", ImGuiTableColumnFlags_NoSort);
                    ImGui::TableSetupColumn("Package ", ImGuiTableColumnFlags_NoSort);
                    ImGui::TableSetupColumn("Beschreibung ", ImGuiTableColumnFlags_NoSort);
                    ImGui::TableSetupColumn("Anzahl", ImGuiTableColumnFlags_NoSort);
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
                        // if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
                        // if (sorts_specs->SpecsDirty)
                        // {
                        //     //order of sorted list: forward or backward
                        //     if (content->size() > 1)
                        //         std::reverse(content->begin(), content->end());
                        //     sorts_specs->SpecsDirty = false;
                        // }
                        ImGuiListClipper clipper;
                        clipper.Begin(content.size());
                        while (clipper.Step())
                            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                            {   
                                ImGui::TableNextRow();
                                bool colored = false;
                                ImVec4 color(0.0f, 0.0f, 0.0f, 1.0f);
                                if(content[row_n].count < 0)
                                {
                                    color = ImVec4(0.7f, 0.3f, 0.3f, .5f);
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,  ImGui::GetColorU32(color));
                                }
                                    
                                else if(content[row_n].count == 0)
                                {
                                    color = ImVec4(0.7f, .7f, 0.3f, .5f);
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,  ImGui::GetColorU32(color));
                                }
                                
                                unsigned int id = content[row_n].id;
                                bool selected = selection.contains(id);
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
                                        alternative = &content[row_n];
                                    }
                                }
                                
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextUnformatted(content[row_n].category.c_str());
                                ImGui::TableSetColumnIndex(2);
                                ImGui::TextUnformatted(content[row_n].value.c_str());
                                ImGui::TableSetColumnIndex(3);
                                ImGui::TextUnformatted(content[row_n].package.c_str());
                                ImGui::TableSetColumnIndex(4);
                                ImGui::TextUnformatted(content[row_n].properties.c_str());
                                ImGui::TableSetColumnIndex(5);
                                ImGui::TextUnformatted(std::to_string(content[row_n].count).c_str());
                                ImGui::TableSetColumnIndex(6);
                                ImGui::TextUnformatted(content[row_n].manufactor.c_str());
                                ImGui::TableSetColumnIndex(7);
                                ImGui::TextUnformatted(content[row_n].manufactor_number.c_str());
                                ImGui::TableSetColumnIndex(8);
                                ImGui::TextUnformatted(content[row_n].distributor.c_str());
                                ImGui::TableSetColumnIndex(9);
                                ImGui::TextUnformatted(content[row_n].shop_number.c_str());
                                ImGui::TableSetColumnIndex(10);
                                ImGui::TextUnformatted(content[row_n].vpe.c_str());
                                ImGui::TableSetColumnIndex(11);
                                ImGui::TextUnformatted(std::to_string(content[row_n].price).c_str());
                                ImGui::TableSetColumnIndex(12);
                                ImGui::TextUnformatted(content[row_n].storage_place.c_str());
                                ImGui::TableSetColumnIndex(13);
                                ImGui::TextUnformatted(content[row_n].datasheet.c_str());
                                
                            }
                    }
                    ImGui::EndTable();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::EndPopup();
            }
        }
        else
        {
            p_database->addItemToAssemble(*p_selected_assemble, item_to_check, count_to_check);
            show_check_item_in_assemble = 0;
        }
    }
}

