#pragma once

class Registry;

class AnimationSystem
{
public:
    void Update(Registry& registry, float deltaTime) const;
};
