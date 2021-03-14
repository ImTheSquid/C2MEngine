#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace c2m { namespace client { namespace gl {

class Camera {
public:
    static glm::mat4 getLookMat() {
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        return view;
    };

    // Returns modifyable object of the projection matrix
    static glm::mat4& getProjectionAddr() { return projection; };

    static glm::mat4& getUiAddr() { return uiProjection; };

    // Returns copy of the projection matrix
    static glm::mat4 getProjectionMat() { return projection; };

    static glm::mat4 getUiMat() { return uiProjection; };

    static void setCameraPos(glm::vec3 pos) { cameraPos = pos; };

    static void changeCameraPos(glm::vec3 difference) { cameraPos += difference; };

private:
    // Camera factors
    inline static glm::vec3 cameraPos = glm::vec3(0.f, 0.f, 3.f);
    inline static glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
    inline static glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

    // Projection
    inline static glm::mat4 projection = glm::mat4(1.f);
    inline static glm::mat4 uiProjection = glm::mat4(1.f);

    // View/camera
    inline static glm::mat4 view = glm::mat4(1.f);
};

}}}