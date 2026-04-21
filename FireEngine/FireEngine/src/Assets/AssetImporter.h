#pragma once

#include "AssetManager.h"

#include <filesystem>
#include <optional>
#include <string>

class AssetImporter
{
public:
    std::optional<std::string> ImportTexturePng(const std::filesystem::path& path, AssetManager& manager) const;
    std::optional<std::string> ImportAudio(const std::filesystem::path& path, AssetManager& manager) const;
    std::optional<std::string> ImportMeshAndAnimations(const std::filesystem::path& path, AssetManager& manager) const;

private:
    static std::string NormalizeId(const std::filesystem::path& path);
    static bool IsAudioExtension(const std::string& extension);
    static bool IsMeshExtension(const std::string& extension);
};
