#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#define GLEW_STATIC

#include <stack>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// #include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "Console.hpp"

#define DEB(msg, lvl)
inline bool console_open;
inline Console console("Console", &console_open); 
void cmsg(std::string msg);
void cmsg(const char* msg);

namespace xgl
{
    namespace program
    {
        class ProgramState;
        class Program
        {
            private:
                
                GLFWwindow* window;
                GLFWimage image;
                std::stack<ProgramState*> states;
            public:
                void init();
                void loop();
                void pushState(ProgramState* state);
                void popState();
                void changeState(ProgramState* state);
                GLFWwindow* getRenderWindow();
                ProgramState* peekState();

                Program();
                ~Program();
        };
    }
    
}

extern xgl::program::Program app;

#endif /*PROGRAM*/