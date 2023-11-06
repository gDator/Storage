#include "ProgramStateDatabase.hpp"
#include "utils.hpp"
#include "Item.hpp"
#include "imgui_spectrum.h"
#include "Global.hpp"

namespace xgl
{
namespace program
{
/*Konstruktor of Task state*/
ProgramStateDatabase::ProgramStateDatabase()
{
    //DEB(lg, trace) << "ProgramStateDatabase::ProgramStateDatabase()";
    //createTree();
    ImGuiIO io = ImGui::GetIO();
    ImGui::GetIO().Fonts->Clear();
    ImGui::Spectrum::LoadFont();
    // ImFont* ubuntu = io.Fonts->AddFontFromFileTTF("./res/UbuntuMono.ttf", 15);
    // assert(ubuntu != NULL);
    ImGui::Spectrum::StyleColorsSpectrumDark();

    p_gui = new GuiDatabase(&console);
    console_open = true;
    //Hardcoded to take away flexability
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // std::map<std::string, std::string> electrical;
    // std::map<std::string, std::string> mechanical;
    // std::map<std::string, std::string> cable;
    // std::map<std::string, std::string> none ;
    // electrical.emplace("None", "None");
    // electrical.emplace("R", "R");
    // electrical.emplace("C","C");
    // electrical.emplace("L", "L");
    // electrical.emplace("J", "J");
    // electrical.emplace("U", "U");
    // electrical.emplace("D", "D");
    // electrical.emplace("K", "K");
    // electrical.emplace("Q", "Q");
    // mechanical.emplace("None", "None");
    // cable.emplace("None", "None");
    // none.emplace("None", "None");
    // g_categories.emplace("E", electrical);
    // g_categories.emplace("M", mechanical);
    // g_categories.emplace("K", cable);
    // g_categories.emplace("None", none);
    init();
}

/*
    @name: draw
    @func: MainLoop of the Game; manage all the incoming states
    @args: none
    @return: none
*/
void ProgramStateDatabase::draw(const float dt, GLFWwindow *window)
{
    //DEB(lg, trace) << "ProgramStateTask::draw()";
    //program->getRenderWindow().clear(xgl::graphics::getXglImguiColors(xgl::graphics::WindowBg));
    p_gui->draw();
    // ImPlot::ShowDemoWindow();
    //ImGui::ShowDemoWindow();
}

/*
    @name: update
    @func: 
    @args: @dt is the time between two update processes
    @return: none
*/
void ProgramStateDatabase::update(const float dt)
{
    //DEB(lg, trace) << "ProgramStateTask::update()";
}

/*
    @name: handleInput
    @func: Handle Inputs in Main Menu; Event can be:
            @Window closed
            @Window resized
            @key pressed

    @args: none
    @return: none
*/
void ProgramStateDatabase::handleInput(GLFWwindow *window)
{
    //DEB(lg, trace) << "ProgramStateTask::handleInput()";
}

/*
    @name: handleKeyboard
    @func: Process all Keyboard Inputs
    @args: @event
    @return: none
*/
void ProgramStateDatabase::handleKeyboard()
{
    //DEB(lg, trace) << "ProgramStateTask::handleKeyboard()";

}

/*
    @name: handleMouse
    @func: Process all Mouse Inputs
    @args: @event
    @return: none
*/
void ProgramStateDatabase::handleMouse()
{
    //DEB(lg, trace) << "ProgramStateTask::handleMouse()";
}

void ProgramStateDatabase::init()
{
    CSimpleIniA ini;
    auto result = "";
    if(!std::filesystem::exists("./storage.ini"))
    {
        cmsg("[error] Initialization failed!");
        LOG_ERROR("Initialization failed!");
        return;
    }
    ini.LoadFile("storage.ini");

    //load Main categories
    std::string cat_string(ini.GetValue("Settings", "Main_Categories", ""));
    if(cat_string != "")
    {
        std::vector<std::string> main_cats;
        main_cats.clear();
        split(cat_string, ';', main_cats);
        for(auto entry : main_cats)
        {
            std::map<std::string, std::string> _category;
            _category.emplace("None", "None");
            std::string hint_main_category = "Hints-" + entry;
            std::list<CSimpleIniA::Entry> sub_categories;
            if(!ini.GetAllKeys(hint_main_category.c_str(), sub_categories))
                continue;
            for(auto c : sub_categories)
            {
                std::string cat(c.pItem);
                _category.emplace(cat, cat);
                std::string result(ini.GetValue(hint_main_category.c_str(), cat.c_str(), ""));
                if(result == "")
                    continue;
                std::vector<std::string> hint;
                hint.clear();
                split(result, ';', hint);
                hint.resize(4); 
                LOG_INFO("found hints: " << hint[0] << ", " << hint[1]<< "," <<hint[2] << "," << hint[3]);
                g_filter_hints.emplace(std::make_tuple(entry, cat), std::make_tuple(hint[0], hint[1], hint[2], hint[3]));
            }
            g_categories.emplace(entry, _category);
        }
        std::map<std::string, std::string> none;
        none.emplace("None", "None");
        g_categories.emplace("None", none);

    }
    
    //load catgeories
    //Init Hints
    for(auto& entry : g_categories)
    {
        
        
    }

    //Load storage places
    result = ini.GetValue("Storage", "Storage-X", "");
    if(result != "")
    {
        split(result, ';', g_storage_x);
    }

    result = ini.GetValue("Storage", "Storage-Y", "");
    if(result != "")
    {
        split(result, ';', g_storage_y);
    }

    result = ini.GetValue("Settings", "Units", "");
    if(result != "")
    {
        std::vector<std::string> units;
        split(result, ';', units);
        for(auto& u : units)
        {
            g_units.emplace(u, u);
        }
    }

    result = ini.GetValue("Settings", "Datasheet_Path", "");
    if(result != "")
    {
        g_settings.path_to_datasheet = result;
    }
}
}
}