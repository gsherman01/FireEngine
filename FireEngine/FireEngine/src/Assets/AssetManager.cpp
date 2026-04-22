#include "AssetManager.h"

#include "AssetImporter.h"

#include <algorithm>
#include <cctype>
#include <memory>

AssetManager::AssetManager() : m_importer(std::make_unique<AssetImporter>())
{
}

AssetManager::~AssetManager() = default;

std::shared_ptr<Model> AssetManager::LoadModel(const std::filesystem::path& path)
{
    const std::string key = NormalizePathKey(path);
    auto found = m_modelCache.find(key);
    if (found != m_modelCache.end())
    {
        return found->second;
    }

    const std::shared_ptr<Model> model = m_importer->ImportModel(path, *this);
    if (model != nullptr)
    {
        m_modelCache[key] = model;
    }

    return model;
}

std::shared_ptr<Texture> AssetManager::LoadTexture(const std::filesystem::path& path, bool flipVertical)
{
    const std::string key = NormalizePathKey(path);
    auto found = m_textureCache.find(key);
    if (found != m_textureCache.end())
    {
        return found->second;
    }

    auto texture = std::make_shared<Texture>();
    if (!texture->LoadFromFile(path.string(), flipVertical))
    {
        return nullptr;
    }

    m_textureCache[key] = texture;
    return texture;
}

std::shared_ptr<Texture> AssetManager::LoadCubemap(const std::string& cacheKey, const std::vector<std::string>& faces)
{
    auto found = m_textureCache.find(cacheKey);
    if (found != m_textureCache.end())
    {
        return found->second;
    }

    auto texture = std::make_shared<Texture>();
    if (!texture->LoadCubemap(faces))
    {
        return nullptr;
    }

    m_textureCache[cacheKey] = texture;
    return texture;
}

void AssetManager::RegisterModel(const std::filesystem::path& path, const std::shared_ptr<Model>& model)
{
    if (model == nullptr)
    {
        return;
    }

    m_modelCache[NormalizePathKey(path)] = model;
}

void AssetManager::RegisterTexture(const std::filesystem::path& path, const std::shared_ptr<Texture>& texture)
{
    if (texture == nullptr)
    {
        return;
    }

    m_textureCache[NormalizePathKey(path)] = texture;
}

std::string AssetManager::NormalizePathKey(const std::filesystem::path& path)
{
    std::filesystem::path absPath = std::filesystem::absolute(path).lexically_normal();
    std::string key = absPath.string();
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return key;
}
