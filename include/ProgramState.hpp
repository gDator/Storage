#ifndef PROGRAMM_STATE_HPP
#define PROGRAMM_STATE_HPP

#include "program.hpp"
namespace xgl
{
namespace program
{
    class ProgramState
    {
        public:
            virtual void draw(const float dt, GLFWwindow* window) = 0;
            virtual void update(const float dt) = 0;
            virtual void handleInput(GLFWwindow* window) = 0;
            virtual void handleKeyboard() = 0;
            virtual void handleMouse() = 0;
    };
}
}

#endif /*PROGRAMM_STATE*/