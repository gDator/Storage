#include "ProgramStateDatabase.hpp"
#include "utils.hpp"
#include "Item.hpp"

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
    ImFont* ubuntu = io.Fonts->AddFontFromFileTTF("./res/UbuntuMono.ttf", 15);
    assert(ubuntu != NULL);

    p_gui = new GuiDatabase(&console);
    console_open = true;
    //Hardcoded to take away flexability
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    std::map<std::string, std::string> electrical;
    std::map<std::string, std::string> mechanical;
    std::map<std::string, std::string> cable;
    std::map<std::string, std::string> none ;
    electrical.emplace("None", "None");
    electrical.emplace("R", "R");
    electrical.emplace("C","C");
    electrical.emplace("L", "L");
    electrical.emplace("J", "J");
    electrical.emplace("U", "U");
    electrical.emplace("D", "D");
    electrical.emplace("K", "K");
    electrical.emplace("Q", "Q");
    mechanical.emplace("None", "None");
    cable.emplace("None", "None");
    none.emplace("None", "None");
    g_categories.emplace("E", electrical);
    g_categories.emplace("M", mechanical);
    g_categories.emplace("K", cable);
    g_categories.emplace("None", none);
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
    // ImGui::ShowDemoWindow();
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

    //Init Hints
    for(auto& entry : g_categories)
    {
        std::string hint_main_category = "Hints-" + std::get<0>(entry);
        for(auto& cat : std::get<1>(entry))
        {
            std::string result(ini.GetValue(hint_main_category.c_str(), std::get<0>(cat).c_str(), ""));
            if(result == "")
                continue;
            std::vector<std::string> hint;
            hint.clear();
            split(result, ';', hint);
            hint.resize(4); 
            LOG_INFO("found hints: " << hint[0] << ", " << hint[1]<< "," <<hint[2] << "," << hint[3]);
            g_filter_hints.emplace(std::make_tuple(std::get<0>(entry), std::get<0>(cat)), std::make_tuple(hint[0], hint[1], hint[2], hint[3]));
        }
        
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

    //

    
}
}
}