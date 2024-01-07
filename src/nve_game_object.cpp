#include "nve_game_object.hpp"

namespace nve
{

    glm::mat4 TransformComponent::mat4()
    {
        //* this is simplified code, which is not as optimized but does the same thing
        // auto transform = glm::translate(glm::mat4{1.f}, translation);
        // transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f});
        // transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f});
        // transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f});
        // transform = glm::scale(transform, scale);
        // return transform;
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale * (c1 * c3 + s1 * s2 * s3),
                scale * (c2 * s3),
                scale * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale * (c3 * s1 * s2 - c1 * s3),
                scale * (c2 * c3),
                scale * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale * (c2 * s1),
                scale * (-s2),
                scale * (c1 * c2),
                0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f}};
    }

    NveGameObject NveGameObject::makePointLight(float intesity, float radius, glm::vec3 color)
    {
        auto gameObj = createGameObject();
        gameObj.color = color;
        gameObj.transform.scale = radius;
        gameObj.pointLight = std::make_unique<PointLightComponent>();
        gameObj.pointLight->lightIntensity = intesity;
        return gameObj;
    }

} // namespace nve
