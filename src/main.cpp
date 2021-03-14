#include <iostream>
#include <gl/glew.h>
#include <SDL2/SDL_opengles2.h>
#include <signal.h>
#include <assert.h>

#include "client/engine.hpp"
#include "client/input/keyboardMouse.hpp"
#include "root.hpp"
#include "client/gl/drawing/drawable.hpp"
#include "client/gl/shaders/shader.hpp"

int main(int argc, char **argv) {
    Root *r = new Root();
    c2m::client::Engine::init("Test", 500, 500, 0, 0, std::shared_ptr<Root>(r));
    c2m::client::Engine::startAndWait();
    return 0;
}