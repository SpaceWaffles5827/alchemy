#include "../alchemy/Collider.h"

Collider::Collider(const std::vector<glm::vec2> &verts) : vertices(verts) {}

std::vector<glm::vec2>
Collider::getWorldVertices(const glm::vec2 &position) const {
    std::vector<glm::vec2> worldVerts;
    for (const auto &vert : vertices) {
        worldVerts.push_back(vert + position); // Offset by position
    }
    return worldVerts;
}
