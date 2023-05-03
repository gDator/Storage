#include "programStateDatabase.hpp"
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

            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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
            p_gui->draw(window);
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
    }
}