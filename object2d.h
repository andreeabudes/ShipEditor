#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateHeart(const std::string& name, glm::vec3 center, float size, glm::vec3 color);
    Mesh* CreateSemicircle(const std::string& name, float radius, glm::vec3 color);
    Mesh* CreateCircle(const std::string& name, float radius, glm::vec3 color, int segments = 40);

}
