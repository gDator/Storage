#include <stack>

#include "ProgramState.hpp"
#include "program.hpp"
#include "logger.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


xgl::program::Program app;
void cmsg(std::string msg)
{   
    console.printLine(msg);
}

void cmsg(const char* msg)
{   
    console.printLine(msg);
}

namespace xgl
{
namespace program
{               

Program::Program()
{
    if (!glfwInit())
        return;
    //DEB(lg, trace) << "Program::Program()";
    initLog("log/log", 1); //only log file
    window = glfwCreateWindow(1280, 720, "Storage", NULL, NULL);
    int channels;
    image.width = 32;
    image.height = 32;
    image.pixels = stbi_load("res/icon.png", &image.width, &image.height, &channels, 0);
    glfwSetWindowIcon(window, 1, &image);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

Program::~Program()
{
    while(!this->states.empty())
        popState();
}


/*
    @name: pushState
    @func: Add a new GameState
    @args: @state which should ne added 
    @return: none
*/
void Program::pushState(ProgramState* state)
{
    //DEB(lg, trace) << "Program::pushState()";
    this->states.push(state);
    return;
}


/*
    @name: popState
    @func: remove the topmost GameState
    @args: none
    @return: none
*/

void Program::popState()
{
    //DEB(lg, trace) << "Program::popState";
    //delete this->states.top();
    this->states.pop();

    return;
}

/*
    @name: changeState
    @func: Changing the State
    @args: @state is the state to which should be changed 
    @return: none
*/

void Program::changeState(ProgramState* state)
{
    //DEB(lg, trace) << "Program::changeState()";
    if(!this->states.empty())
        popState();
    pushState(state);
    return;
}

/*
    @name: peekState
    @func: Looking at the next state to work with
    @args: none
    @return: the first @GameState
*/

ProgramState* Program::peekState()
{
    //DEB(lg, trace) << "Program::peekState()";
    if(this->states.empty())
        return nullptr;
    return this->states.top();
}

/*
    @name: loop
    @func: MainLoop of the Game; manage all the incoming states
    @args: none
    @return: none
*/
void Program::loop()
{
    //DEB(lg, trace) << "Program::loop()";
    ImGuiIO& io = ImGui::GetIO();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    while(!glfwWindowShouldClose(window))
    {
        float dt = 1e-3;
        if(peekState() == nullptr)
        {
            std::cout<< "error: no state\n"; 
            break;
        }
        glfwPollEvents();
        peekState() -> handleInput(window);
        peekState() -> update(dt);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        peekState() -> draw(dt, window);
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);

        // if(dt != 0)
        //     std::cout << "FPS: " << (int)1/dt << "\r";
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

/*
    @name: init
    @func: initializerfunction
    @args: none
    @return: none
*/
void Program::init()
{
    //DEB(lg, trace) << "Program::init()";
}

GLFWwindow* Program::getRenderWindow()
{
    return window;
} 


}
}
