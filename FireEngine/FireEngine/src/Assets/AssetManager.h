#pragma once

#include "Renderer/Texture.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct TextureAsset
{
    std::string id;
    std::string sourcePath;
    std::unique_ptr<Texture> texture;
};

struct AudioAsset
{
    std::string id;
    std::string sourcePath;
    std::size_t byteSize = 0;
};

struct MeshAsset
{
    std::string id;
    std::string sourcePath;
    std::size_t meshCount = 0;
    std::size_t vertexCount = 0;
    std::size_t indexCount = 0;
};

struct AnimationAsset
{
    std::string id;
    std::string sourcePath;
    std::vector<std::string> clipNames;
};

class AssetManager
{
public:
    TextureAsset& RegisterTexture(TextureAsset asset)
    {
        auto [it, _] = m_textures.emplace(asset.id, std::move(asset));
        return it->second;
    }

    AudioAsset& RegisterAudio(AudioAsset asset)
    {
        auto [it, _] = m_audio.emplace(asset.id, std::move(asset));
        return it->second;
    }

    MeshAsset& RegisterMesh(MeshAsset asset)
    {
        auto [it, _] = m_meshes.emplace(asset.id, std::move(asset));
        return it->second;
    }

    AnimationAsset& RegisterAnimation(AnimationAsset asset)
    {
        auto [it, _] = m_animations.emplace(asset.id, std::move(asset));
        return it->second;
    }

    const TextureAsset* FindTexture(const std::string& id) const
    {
        auto found = m_textures.find(id);
        return found != m_textures.end() ? &found->second : nullptr;
    }

    const AudioAsset* FindAudio(const std::string& id) const
    {
        auto found = m_audio.find(id);
        return found != m_audio.end() ? &found->second : nullptr;
    }

    const MeshAsset* FindMesh(const std::string& id) const
    {
        auto found = m_meshes.find(id);
        return found != m_meshes.end() ? &found->second : nullptr;
    }

    const AnimationAsset* FindAnimation(const std::string& id) const
    {
        auto found = m_animations.find(id);
        return found != m_animations.end() ? &found->second : nullptr;
    }

    std::size_t GetTextureCount() const { return m_textures.size(); }
    std::size_t GetAudioCount() const { return m_audio.size(); }
    std::size_t GetMeshCount() const { return m_meshes.size(); }
    std::size_t GetAnimationCount() const { return m_animations.size(); }

private:
    std::unordered_map<std::string, TextureAsset> m_textures;
    std::unordered_map<std::string, AudioAsset> m_audio;
    std::unordered_map<std::string, MeshAsset> m_meshes;
    std::unordered_map<std::string, AnimationAsset> m_animations;
};
