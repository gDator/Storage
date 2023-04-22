#ifndef PROGRAM_STATE_TEST_HPP
#define PROGRAM_STATE_TEST_HPP

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "ProgramState.hpp"
#include "Console.hpp"
#include "ViewHandler.hpp"
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
                sf::Texture font_texture;
                unsigned char* pixels;
                int width, height;
                sf::ViewHandler view;
                GuiDatabase* p_gui;                         
                void draw(const float dt, sf::RenderWindow &window);
                void update(const float dt, sf::Time elapsed);
                void handleInput(sf::RenderWindow &window);
                void handleKeyboard(sf::Event event);
                void handleMouse(sf::Event event);
                ProgramStateDatabase();
        };
    }
}

#endif /*PROGRAMM_STATE_TEST_HPP*/