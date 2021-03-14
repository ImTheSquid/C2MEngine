#pragma once

#include <glm/glm.hpp>

namespace c2m { namespace common {

struct RGBA {
    float r, g, b, a;

    RGBA() {
        r = 1;
        g = 1;
        b = 1;
        a = 1;
    }

    RGBA(float r, float b, float g, float a) : r(r), g(g), b(b), a(a) {}
    
    const glm::vec4 asVec4() {
        return glm::vec4(r, g, b, a);
    }
};

}}