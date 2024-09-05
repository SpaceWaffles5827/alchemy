#include "../alchemy/SATCollision.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <cmath>

float projectOntoAxis(const glm::vec2 &axis, const glm::vec2 &point) {
    return glm::dot(axis, point);
}

bool checkOverlap(const std::vector<float> &projectionsA,
                  const std::vector<float> &projectionsB) {
    float minA = *std::min_element(projectionsA.begin(), projectionsA.end());
    float maxA = *std::max_element(projectionsA.begin(), projectionsA.end());
    float minB = *std::min_element(projectionsB.begin(), projectionsB.end());
    float maxB = *std::max_element(projectionsB.begin(), projectionsB.end());

    return !(minA > maxB || minB > maxA); // No overlap if true
}

bool SATCollision(const Collider &colliderA, const glm::vec2 &positionA,
                  const Collider &colliderB, const glm::vec2 &positionB) {
    auto vertsA = colliderA.getWorldVertices(positionA);
    auto vertsB = colliderB.getWorldVertices(positionB);

    // Generate normals to edges (axes) for both shapes
    std::vector<glm::vec2> axes;

    for (int i = 0; i < vertsA.size(); ++i) {
        glm::vec2 edge = vertsA[(i + 1) % vertsA.size()] - vertsA[i];
        glm::vec2 normal = glm::normalize(glm::vec2(-edge.y, edge.x));
        axes.push_back(normal);
    }

    for (int i = 0; i < vertsB.size(); ++i) {
        glm::vec2 edge = vertsB[(i + 1) % vertsB.size()] - vertsB[i];
        glm::vec2 normal = glm::normalize(glm::vec2(-edge.y, edge.x));
        axes.push_back(normal);
    }

    // Project both shapes onto each axis and check for overlap
    for (const auto &axis : axes) {
        std::vector<float> projectionsA, projectionsB;
        for (const auto &vert : vertsA)
            projectionsA.push_back(projectOntoAxis(axis, vert));
        for (const auto &vert : vertsB)
            projectionsB.push_back(projectOntoAxis(axis, vert));

        if (!checkOverlap(projectionsA, projectionsB)) {
            return false; // Separating axis found, no collision
        }
    }

    return true; // All axes overlap, collision
}
