#pragma once
#include <glm/glm.hpp>
#include <vector>

class Collider {
  public:
    Collider(const std::vector<glm::vec2> &vertices);

    // Returns the vertices adjusted by the object's position in the world
    std::vector<glm::vec2> getWorldVertices(const glm::vec2 &position) const;

  private:
    std::vector<glm::vec2> vertices; // Customizable collider vertices
};
