#include <iostream>
#include <thread>

#include "client/engine.hpp"
#include "client/input/keyboardMouse.hpp"

int main(int argc, char **argv) {
    c2m::client::Engine::init("Test", 200, 200, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SDL_WINDOW_RESIZABLE);
    
    bool run = true;
    while (run) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
                break;
            }
        }
    }
    return 0;
}