#include "Gui/GuiTable.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "Language/Language.hpp"
#include "Global.hpp"

#ifdef _WIN32
#include "Windows.h"
#include <shellapi.h>
#elif __linux__
#endif

void GuiTable::draw(std::deque<Item>& content, ImVector<unsigned int>& selection, Item*& selected_item)
{
static ImGuiTableFlags table_flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
        | ImGuiTableFlags_Sortable //| ImGuiTableFlags_SortMulti
        | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

if(ImGui::BeginTable("search_results", 19, table_flags))
{
    ImGui::TableSetupColumn(L_ID, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_MAIN_CATEGORY, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_CATEGORY, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_VALUE_FIRST, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_VALUE_SECOND, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_PACKAGE, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_DESCRIPTION, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_UNIT, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_COUNT, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_RESERVED, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_MANUFACTOR, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_MANUFACTOR_NO, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_DISTRIBUTOR, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_DISTRIBUTOR_NO, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_PACKAGING, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_PRICE_PER_PACKAGING, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_PRICE_PER_PIECE, ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn(L_STORAGE_PLACE, ImGuiTableColumnFlags_NoSort);
    ImGui::TableSetupColumn(L_DOCUMENT_LINK, ImGuiTableColumnFlags_NoSort);
    ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
    ImGui::TableHeadersRow();
    // Sorting
    if (ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs())
        if (sort_specs->SpecsDirty && content.size() > 1)
        {
            sortItems(content);   
            sort_specs->SpecsDirty = false;
        }

    if(!content.empty())
    {
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
                        selected_item = &content[row_n];
                    }
                }
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(content[row_n].main_category.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(content[row_n].category.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(content[row_n].value.c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::TextUnformatted(content[row_n].value_2.c_str());
                ImGui::TableSetColumnIndex(5);
                ImGui::TextUnformatted(content[row_n].package.c_str());
                ImGui::TableSetColumnIndex(6);
                ImGui::TextUnformatted(content[row_n].description.c_str());
                ImGui::TableSetColumnIndex(7);
                ImGui::TextUnformatted(content[row_n].unit.c_str());
                ImGui::TableSetColumnIndex(8);
                ImGui::TextUnformatted(std::to_string(content[row_n].count).c_str());
                ImGui::TableSetColumnIndex(9);
                ImGui::TextUnformatted(std::to_string(content[row_n].reserved).c_str());
                ImGui::TableSetColumnIndex(10);
                ImGui::TextUnformatted(content[row_n].manufactor.c_str());
                ImGui::TableSetColumnIndex(11);
                ImGui::TextUnformatted(content[row_n].manufactor_number.c_str());
                ImGui::TableSetColumnIndex(12);
                ImGui::TextUnformatted(content[row_n].distributor.c_str());
                ImGui::TableSetColumnIndex(13);
                ImGui::TextUnformatted(content[row_n].shop_number.c_str());
                ImGui::TableSetColumnIndex(14);
                ImGui::TextUnformatted(std::to_string(content[row_n].vpe).c_str());
                ImGui::TableSetColumnIndex(15);
                ImGui::TextUnformatted(std::to_string(content[row_n].price).c_str());
                ImGui::TableSetColumnIndex(16);
                ImGui::TextUnformatted(std::to_string(content[row_n].price_per_unit).c_str());
                ImGui::TableSetColumnIndex(17);
                ImGui::TextUnformatted(content[row_n].storage_place.c_str());
                ImGui::TableSetColumnIndex(18);
                if(content[row_n].datasheet == "")
                    ImGui::TextUnformatted(content[row_n].datasheet.c_str());
                else
                    if(ImGui::SmallButton(content[row_n].datasheet.c_str()))
                    {
                        //open pdf with standard program
                        try
                        {
                            #ifdef _WIN32
                                ShellExecute(0,0 , std::string(g_settings.path_to_datasheet + "\\" + content[row_n].datasheet).c_str(), 0, 0, SW_SHOW);
                            #endif
                        }
                        catch(const std::exception& e)
                        {
                                if(!std::filesystem::exists(std::string(g_settings.path_to_datasheet + "\\" + content[row_n].datasheet)))
                                {
                                    cmsg("[Error] File not found");
                                    LOG_ERROR("Could not open File:" + std::string(g_settings.path_to_datasheet + "\\" + content[row_n].datasheet));
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
}