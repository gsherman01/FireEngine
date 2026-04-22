#pragma once

#include "Renderer/Model.h"
#include "Renderer/Texture.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class AssetImporter;

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    std::shared_ptr<Model> LoadModel(const std::filesystem::path& path);
    std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path, bool flipVertical = true);
    std::shared_ptr<Texture> LoadCubemap(const std::string& cacheKey, const std::vector<std::string>& faces);

    void RegisterModel(const std::filesystem::path& path, const std::shared_ptr<Model>& model);
    void RegisterTexture(const std::filesystem::path& path, const std::shared_ptr<Texture>& texture);

    std::size_t GetModelCount() const { return m_modelCache.size(); }
    std::size_t GetTextureCount() const { return m_textureCache.size(); }

private:
    static std::string NormalizePathKey(const std::filesystem::path& path);

    std::unique_ptr<AssetImporter> m_importer;
    std::unordered_map<std::string, std::shared_ptr<Model>> m_modelCache;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureCache;
};
