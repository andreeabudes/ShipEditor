#include "lab_m1/tema1/object2d.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill) {
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

// fac o inima din 2 semicercuri si un triunghi
Mesh* object2D::CreateHeart(const std::string& name, glm::vec3 center, float size, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float s = size / 2.0f;
    int circleResolution = 40;
    // cat de mari sunt cercurile
    float radius = s * 0.54f;

    // triunghiul de jos (cu varful in jos)
    glm::vec3 A = center + glm::vec3(-s, 0, 0);        
    glm::vec3 B = center + glm::vec3(s, 0, 0);         
    glm::vec3 C = center + glm::vec3(0, -s * 1.2f, 0); 

    vertices.push_back(VertexFormat(A, color));
    vertices.push_back(VertexFormat(B, color));
    vertices.push_back(VertexFormat(C, color));

    indices.insert(indices.end(), { 0, 1, 2 });

    // cercul din stanga
    glm::vec3 leftCenter = center + glm::vec3(-s * 0.47f, 0, 0);
    unsigned int startLeft = vertices.size();
    vertices.push_back(VertexFormat(leftCenter, color));

    for (int i = 0; i <= circleResolution; i++) {
        // semicerc
        float angle = glm::pi<float>() * (float)i / circleResolution;
        float x = leftCenter.x + radius * cos(angle);
        float y = leftCenter.y + radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));

        if (i > 0)
            indices.insert(indices.end(), { startLeft, startLeft + i, startLeft + i + 1 });
    }

    // cercul din dreapta
    glm::vec3 rightCenter = center + glm::vec3(s * 0.47f, 0, 0);
    unsigned int startRight = vertices.size();
    vertices.push_back(VertexFormat(rightCenter, color));

    for (int i = 0; i <= circleResolution; i++) {
        float angle = glm::pi<float>() * (float)i / circleResolution;
        float x = rightCenter.x + radius * cos(angle);
        float y = rightCenter.y + radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));

        if (i > 0)
            indices.insert(indices.end(), { startRight, startRight + i, startRight + i + 1 });
    }

    Mesh* heart = new Mesh(name);
    heart->InitFromData(vertices, indices);
    return heart;
}

Mesh* object2D::CreateSemicircle(const std::string& name, float radius, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    int segments = 30;
    vertices.emplace_back(glm::vec3(0, 0, 0), color);

    for (int i = 0; i <= segments; i++) {
        float angle = glm::pi<float>() * i / segments;
        vertices.emplace_back(glm::vec3(radius * cos(angle), radius * sin(angle), 0), color);
    }

    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* semicircle = new Mesh(name);
    semicircle->InitFromData(vertices, indices);
    return semicircle;
}

Mesh* object2D::CreateCircle(const std::string& name, float radius, glm::vec3 color, int segments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // centrul cercului
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));

    float angleStep = 2 * M_PI / segments;

    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
    }

    // triunghiuri tip "triangle fan"
    for (int i = 1; i <= segments; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* circle = new Mesh(name);
    circle->InitFromData(vertices, indices);
    return circle;
}

