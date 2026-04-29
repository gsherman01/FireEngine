#pragma once

#include "ECS/Registry.h"

#include <string>

class SceneSerializer
{
public:
    static bool Save(const Registry& registry, const std::string& path);
    static bool Load(Registry& registry, const std::string& path);
};
