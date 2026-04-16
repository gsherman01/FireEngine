#pragma once

class Registry;

class CameraSystem
{
public:
    void Update(Registry& registry, float deltaTime) const;
};
