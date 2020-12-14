#pragma once
#include <array>

#include <glm/glm.hpp>

struct AABB;

struct Plane {
    float distanceToPoint(const glm::vec3& point) const;

    float distanceToOrigin;
    glm::vec3 normal;
};

class ViewFrustum {
public:
    void update(const glm::mat4& projViewMatrix) noexcept;

    bool isBoxInFrustum(const AABB& box) const noexcept;

private:
    std::array<Plane, 6> m_planes;
};