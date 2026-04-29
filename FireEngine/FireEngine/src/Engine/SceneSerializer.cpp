#include "SceneSerializer.h"

#include "Scene/Components.h"

#include <fstream>
#include <sstream>
#include <vector>

namespace
{
    std::vector<std::string> Split(const std::string& line, char delimiter)
    {
        std::vector<std::string> parts;
        std::stringstream stream(line);
        std::string item;
        while (std::getline(stream, item, delimiter)) parts.push_back(item);
        return parts;
    }
}

bool SceneSerializer::Save(const Registry& registry, const std::string& path)
{
    std::ofstream output(path);
    if (!output.is_open()) return false;

    output << "# FireEngineScene v1\n";
    Registry& mutableRegistry = const_cast<Registry&>(registry);
    for (Entity entity : mutableRegistry.GetEntities())
    {
        output << "Entity|" << entity.GetId() << "\n";
        if (entity.HasComponent<NameComponent>())
        {
            output << "Name|" << entity.GetComponent<NameComponent>().name << "\n";
        }
        if (entity.HasComponent<TransformComponent>())
        {
            const Transform& t = entity.GetComponent<TransformComponent>().transform;
            output << "Transform|" << t.position.x << "|" << t.position.y << "|" << t.position.z
                   << "|" << t.rotationDegrees.x << "|" << t.rotationDegrees.y << "|" << t.rotationDegrees.z
                   << "|" << t.scale.x << "|" << t.scale.y << "|" << t.scale.z << "\n";
        }
        if (entity.HasComponent<LightComponent>())
        {
            const LightComponent& l = entity.GetComponent<LightComponent>();
            output << "Light|" << static_cast<int>(l.type) << "|" << l.color.x << "|" << l.color.y << "|" << l.color.z
                   << "|" << l.direction.x << "|" << l.direction.y << "|" << l.direction.z
                   << "|" << l.position.x << "|" << l.position.y << "|" << l.position.z
                   << "|" << l.intensity << "|" << l.range << "\n";
        }
        if (entity.HasComponent<CameraComponent>())
        {
            const CameraComponent& c = entity.GetComponent<CameraComponent>();
            output << "Camera|" << (c.primary ? 1 : 0) << "\n";
        }
        if (entity.HasComponent<MaterialComponent>())
        {
            const MaterialComponent& m = entity.GetComponent<MaterialComponent>();
            output << "Material|" << (m.useModelMaterial ? 1 : 0)
                   << "|" << m.overrideMaterial.albedo.x << "|" << m.overrideMaterial.albedo.y << "|" << m.overrideMaterial.albedo.z
                   << "|" << m.overrideMaterial.shininess << "|" << m.overrideMaterial.metallic << "|" << m.overrideMaterial.roughness
                   << "|" << m.overrideMaterial.ao
                   << "|" << m.overrideMaterial.emissive.x << "|" << m.overrideMaterial.emissive.y << "|" << m.overrideMaterial.emissive.z << "\n";
        }
        output << "EndEntity\n";
    }
    return true;
}

bool SceneSerializer::Load(Registry& registry, const std::string& path)
{
    std::ifstream input(path);
    if (!input.is_open()) return false;

    for (Entity e : registry.GetEntities()) registry.DestroyEntity(e);

    Entity current;
    std::string line;
    while (std::getline(input, line))
    {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> p = Split(line, '|');
        if (p.empty()) continue;

        if (p[0] == "Entity") current = registry.CreateEntity();
        else if (p[0] == "Name" && p.size() >= 2) current.AddComponent<NameComponent>().name = p[1];
        else if (p[0] == "Transform" && p.size() >= 10)
        {
            TransformComponent& t = current.AddComponent<TransformComponent>();
            t.transform.position = {std::stof(p[1]), std::stof(p[2]), std::stof(p[3])};
            t.transform.rotationDegrees = {std::stof(p[4]), std::stof(p[5]), std::stof(p[6])};
            t.transform.scale = {std::stof(p[7]), std::stof(p[8]), std::stof(p[9])};
        }
        else if (p[0] == "Light" && p.size() >= 13)
        {
            LightComponent& l = current.AddComponent<LightComponent>();
            l.type = static_cast<LightType>(std::stoi(p[1]));
            l.color = {std::stof(p[2]), std::stof(p[3]), std::stof(p[4])};
            l.direction = {std::stof(p[5]), std::stof(p[6]), std::stof(p[7])};
            l.position = {std::stof(p[8]), std::stof(p[9]), std::stof(p[10])};
            l.intensity = std::stof(p[11]);
            l.range = std::stof(p[12]);
        }
        else if (p[0] == "Camera" && p.size() >= 2)
        {
            CameraComponent& c = current.AddComponent<CameraComponent>();
            c.primary = std::stoi(p[1]) != 0;
        }
        else if (p[0] == "Material" && p.size() >= 12)
        {
            MaterialComponent& m = current.AddComponent<MaterialComponent>();
            m.useModelMaterial = std::stoi(p[1]) != 0;
            m.overrideMaterial.albedo = {std::stof(p[2]), std::stof(p[3]), std::stof(p[4])};
            m.overrideMaterial.shininess = std::stof(p[5]);
            m.overrideMaterial.metallic = std::stof(p[6]);
            m.overrideMaterial.roughness = std::stof(p[7]);
            m.overrideMaterial.ao = std::stof(p[8]);
            m.overrideMaterial.emissive = {std::stof(p[9]), std::stof(p[10]), std::stof(p[11])};
        }
    }
    return true;
}
