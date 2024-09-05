#pragma once
#include "Collider.h"

bool SATCollision(const Collider &colliderA, const glm::vec2 &positionA,
                  const Collider &colliderB, const glm::vec2 &positionB);
