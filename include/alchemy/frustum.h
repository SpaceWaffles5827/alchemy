#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

class Frustum {
public:
    void update(const glm::mat4& projectionViewMatrix) {
        glm::mat4 mat = projectionViewMatrix;

        planes[0] = glm::vec4(mat[0][3] + mat[0][0], mat[1][3] + mat[1][0], mat[2][3] + mat[2][0], mat[3][3] + mat[3][0]); // Left
        planes[1] = glm::vec4(mat[0][3] - mat[0][0], mat[1][3] - mat[1][0], mat[2][3] - mat[2][0], mat[3][3] - mat[3][0]); // Right
        planes[2] = glm::vec4(mat[0][3] + mat[0][1], mat[1][3] + mat[1][1], mat[2][3] + mat[2][1], mat[3][3] + mat[3][1]); // Bottom
        planes[3] = glm::vec4(mat[0][3] - mat[0][1], mat[1][3] - mat[1][1], mat[2][3] - mat[2][1], mat[3][3] - mat[3][1]); // Top
        planes[4] = glm::vec4(mat[0][3] + mat[0][2], mat[1][3] + mat[1][2], mat[2][3] + mat[2][2], mat[3][3] + mat[3][2]); // Near
        planes[5] = glm::vec4(mat[0][3] - mat[0][2], mat[1][3] - mat[1][2], mat[2][3] - mat[2][2], mat[3][3] - mat[3][2]); // Far

        for (auto& plane : planes) {
            float length = glm::length(glm::vec3(plane));
            plane /= length;
        }
    }

    bool isInFrustum(const glm::vec3& pos, float radius) const {
        for (const auto& plane : planes) {
            if (glm::dot(glm::vec3(plane), pos) + plane.w + radius < 0) {
                return false;
            }
        }
        return true;
    }

private:
    std::array<glm::vec4, 6> planes;
};
