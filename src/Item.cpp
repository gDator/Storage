#include "Item.hpp"
#include "logger.hpp"


void Item::clear()
{
    if(m_filter_item)
    {
        id = 0; 
        count = 0;
        reserved = 0;
        price = 0.0;
        price_per_unit = 0;
        vpe = 0;
        value = "";
        value_2 = "";
        package = "";
        description = "";
        unit = "None";
        manufactor = "";
        manufactor_number = "";
        distributor = "";
        shop_number = "";
        storage_place = "None|None";
        category = "None";
        main_category = "None";
        datasheet = "";
    }
    else
    {
        id = 0; 
        count = 0;
        price = 0.0;
        price_per_unit = 0;
        reserved = 0;
        vpe = 1;
        value = "";
        value_2 = "";
        package = "";
        description = "";
        unit = "None";
        manufactor = "";
        manufactor_number = "";
        distributor = "";
        shop_number = "";
        storage_place = "None|None";
        category = "None";
        main_category = "None";
        datasheet = "";
    }


}

std::string Item::serializeCSV() const 
{
    std::string csv;
    csv += main_category + ";";
    csv += category + ";";
    csv += value + ";";
    csv += value_2 + ";";
    csv += package + ";";
    csv += description + ";";
    csv += manufactor + ";";
    csv += manufactor_number + ";";
    csv += distributor + ";";
    csv += shop_number + ";";
    csv += std::to_string(vpe) + ";";
    csv += std::to_string(price) + ";";
    csv += std::to_string(price_per_unit) + ";";
    csv += std::to_string(count);
    csv += storage_place + ";";
    csv += datasheet;
    csv += unit + ";";
    return csv;
}

bool Item::deserializeCSV(Item& i, std::vector<std::string> data)
{
    i.clear();
    if(data.size() != 16)
    {
        LOG_ERROR("CSV data has not right amount of columns: " << data.size());
        return false;
    }
    i.main_category = data[0];   
    i.category = data[1];
    i.value = data[2];
    i.value_2 = data[3];
    i.package = data[4];
    i.description = data[5];
    i.manufactor = data[6];
    i.manufactor_number = data[7];
    i.distributor = data[8];
    i.shop_number = data[9];
    i.storage_place = data[13];
    i.datasheet = data[14];
    i.unit = data[15];
    try
    {
        i.vpe = std::stoi(data[10]);
        i.price = std::stof(data[11]);
        i.price_per_unit = i.price / i.vpe;
        i.count = std::stoi(data[12]);
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("couldnt import price or count");
        return false;
    }        
    return true;
}

void sortItems(std::deque<Item>& content)
{
    auto sort_specs = ImGui::TableGetSortSpecs();
    std::sort(content.begin(), content.end(), [&sort_specs] (Item& a, Item& b) 
    {
    for (int n = 0; n < sort_specs->SpecsCount; n++)
    {
        const ImGuiTableColumnSortSpecs* sort_spec = &sort_specs->Specs[0];
        int delta = 0;
        switch(sort_spec->ColumnIndex)
        {
            case 0: delta = a.id - b.id; break;
            case 1: delta = (a.main_category.compare(b.main_category)); break;
            case 2: delta = (a.category.compare(b.category)); break;
            case 3: delta = (a.value.compare(b.value)); break;
            case 4: delta = (a.value_2.compare(b.value_2)); break;
            case 5: delta = (a.package.compare(b.package)); break;
            case 6: delta = (a.description.compare(b.description)); break;
            case 7: delta = (a.unit.compare(b.unit)); break;
            case 8: delta = a.count - b.count; break;
            case 9: delta = a.reserved - b.reserved; break;
            case 10: delta = (a.manufactor.compare(b.manufactor)); break;
            case 11: delta = (a.manufactor_number.compare(b.manufactor_number)); break;
            case 12: delta = (a.distributor.compare(b.distributor)); break;
            case 13: delta = (a.shop_number.compare(b.shop_number)); break;
            case 14: delta = a.vpe - b.vpe; break;
            case 15: delta = (a.price - b.price); break;
            case 16: if(a.vpe != 0 && b.vpe!= 0) delta = (a.price/a.vpe - b.price/b.vpe); break;                                    
        }

        if (delta > 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? true : false;
        if (delta < 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? false : true;
    }
        return false;
    });
}
