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
            ImGuiContext* im_context = ImGui::GetCurrentContext();
            if(im_context == nullptr)
                ImGui::CreateContext();
            ImGuiIO io = ImGui::GetIO();
            ImFont* ubuntu = io.Fonts->AddFontFromFileTTF("./res/UbuntuMono.ttf", 15);
            ImGui::SFML::UpdateFontTexture();

            p_gui = new GuiDatabase(&console, &view);
            view.setRenderWindow(app.getRenderWindow());
            console_open = true;

            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        }

        /*
            @name: draw
            @func: MainLoop of the Game; manage all the incoming states
            @args: none
            @return: none
        */
        void ProgramStateDatabase::draw(const float dt, sf::RenderWindow &window)
        {
            //DEB(lg, trace) << "ProgramStateTask::draw()";
            //program->getRenderWindow().clear(xgl::graphics::getXglImguiColors(xgl::graphics::WindowBg));
            view.active();
            p_gui->draw(window);
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) //added this! ---
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
            // ImPlot::ShowDemoWindow();
            // ImGui::ShowDemoWindow();
        }

        /*
            @name: update
            @func: 
            @args: @dt is the time between two update processes
            @return: none
        */
        void ProgramStateDatabase::update(const float dt, sf::Time elapsed)
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
        void ProgramStateDatabase::handleInput(sf::RenderWindow &window)
        {
            //DEB(lg, trace) << "ProgramStateTask::handleInput()";
            sf::Event event;

            //Process all Events
            while(window.pollEvent(event))
            {
                //Redirect to ImGUI
                if(ImGui::GetCurrentContext()->IO.WantCaptureMouse != 0)
                    ImGui::SFML::ProcessEvent(event);
                else
                {
                    //Redirect to CodeMap
                    view.handleInput(event);
                }
                    

                //Process in SFML
                switch(event.type)
                {
                    //Close Window
                    case sf::Event::Closed:
                    {
                        window.close();
                        break;
                    }

                    //Resize Window: View gets same size as window
                    case sf::Event::Resized:
                    {
                        break;
                    }

                    //Keyboard active
                    case sf::Event::KeyPressed:
                    {
                        handleKeyboard(event);
                        break;
                    }
                    
                    default:
                        handleMouse(event);
                        break;
                }
            }
            return;
        }

        /*
            @name: handleKeyboard
            @func: Process all Keyboard Inputs
            @args: @event
            @return: none
        */
        void ProgramStateDatabase::handleKeyboard(sf::Event event)
        {
            //DEB(lg, trace) << "ProgramStateTask::handleKeyboard()";
            switch(event.type)
            {
                //Close Window
                case sf::Event::Closed:
                {}
            }
        }

        /*
            @name: handleMouse
            @func: Process all Mouse Inputs
            @args: @event
            @return: none
        */
        void ProgramStateDatabase::handleMouse(sf::Event event)
        {
            //DEB(lg, trace) << "ProgramStateTask::handleMouse()";
        }
    }
}