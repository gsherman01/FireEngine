#pragma once

#include <filesystem>
#include <memory>

class AssetManager;
class Model;

class AssetImporter
{
public:
    std::shared_ptr<Model> ImportModel(const std::filesystem::path& path, AssetManager& manager) const;
};
