#pragma once
#include <glm.hpp>

namespace CrossCraft {
class AABB {
  public:
    AABB(glm::vec3 position, glm::vec3 extent) : pos(position), ext(extent) {}

    auto getMin() -> glm::vec3 { return pos; }
    auto getMax() -> glm::vec3 { return pos + ext; }

    static auto intersect(AABB &a, AABB &b) -> bool {
        auto amin = a.getMin();
        auto amax = a.getMax();
        auto bmin = b.getMin();
        auto bmax = b.getMax();

        return (amin.x <= bmax.x && amax.x >= bmin.x) &&
               (amin.y <= bmax.y && amax.y >= bmin.y) &&
               (amin.z <= bmax.z && amax.z >= bmin.z);
    }

    static auto intersectVec(AABB &a, AABB &b) -> glm::vec3 {
        auto amax = a.getMax();
        auto bmin = b.getMin();

        return amax - bmin;
    }

    auto getCenter() -> glm::vec3 { return (getMin() + getMax()) * 0.5f; }

    glm::vec3 pos, ext;
};
} // namespace CrossCraft