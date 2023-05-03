#ifndef PROGRAM_STATE_TEST_HPP
#define PROGRAM_STATE_TEST_HPP

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "ProgramState.hpp"
#include "Console.hpp"
#include "GuiDatabase.hpp"
#include "Item.hpp"
#include <functional>
#include <memory>
#include <mutex>

namespace xgl
{
    namespace program
    {
        class ProgramStateDatabase : public ProgramState
        {
            friend class GuiDatabase;
            private: 
                int error;
            public:
                ImFontAtlas fonts;
                unsigned char* pixels;
                int width, height;
                GuiDatabase* p_gui;                         
                void draw(const float dt, GLFWwindow *window);
                void update(const float dt);
                void handleInput(GLFWwindow *window);
                void handleKeyboard();
                void handleMouse();
                ProgramStateDatabase();
        };
    }
}

#endif /*PROGRAMM_STATE_TEST_HPP*/