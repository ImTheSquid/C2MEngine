#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <exception>
#include <memory>
#include <iostream>
#include <GL/glew.h>

#include "input/keyboardMouse.hpp"

namespace c2m {
namespace client {

class Engine {
public:
    static bool init(const std::string& title, const int width, const int height, const int x = SDL_WINDOWPOS_CENTERED, const int y = SDL_WINDOWPOS_CENTERED, const uint32_t flags = 0) {
        window = SDL_CreateWindow(title.c_str(), x, y, width, height, flags | SDL_WINDOW_OPENGL);
        
        if (window == nullptr) {
            std::cout << "An error occured while creating the window: " << SDL_GetError() << std::endl;
            return false;
        }

        // Initialize OpenGL
        glContext = SDL_GL_CreateContext(window);
        if (glContext == NULL) {
            std::cout << "An error occured while initializing OpenGL: " << SDL_GetError() << std::endl;
            return false;
        }

        GLenum error = glewInit();
        if (error != GLEW_OK) {
            std::cout << "An error occured while initializing GLEW: " << glewGetErrorString(error) << std::endl;
            return false;
        }

        input = new input::KeyboardMouse(getWindow());

        return true;
    }

    static std::shared_ptr<SDL_Window> getWindow() {
        if (window == nullptr) {
            throw new std::exception("Window not initialized.");
        }
        return std::shared_ptr<SDL_Window>(window, [](SDL_Window *win) { SDL_DestroyWindow(win); });
    }

    static std::shared_ptr<input::KeyboardMouse> getInput() {
        if (input == nullptr) {
            throw new std::exception("Window not initialized.");
        }
        return std::shared_ptr<input::KeyboardMouse>(input);
    }

private:
    inline static SDL_Window *window = nullptr;
    inline static input::KeyboardMouse *input = nullptr;
    inline static SDL_GLContext glContext = 0;
};

}
}