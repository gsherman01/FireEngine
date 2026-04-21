#include "AssetImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <cctype>
#include <fstream>

std::optional<std::string> AssetImporter::ImportTexturePng(const std::filesystem::path& path, AssetManager& manager) const
{
    if (!std::filesystem::exists(path) || path.extension() != ".png")
    {
        return std::nullopt;
    }

    auto texture = std::make_unique<Texture>();
    if (!texture->LoadFromFile(path.string(), true))
    {
        return std::nullopt;
    }

    TextureAsset asset;
    asset.id = NormalizeId(path);
    asset.sourcePath = path.string();
    asset.texture = std::move(texture);

    manager.RegisterTexture(std::move(asset));
    return NormalizeId(path);
}

std::optional<std::string> AssetImporter::ImportAudio(const std::filesystem::path& path, AssetManager& manager) const
{
    if (!std::filesystem::exists(path))
    {
        return std::nullopt;
    }

    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (!IsAudioExtension(extension))
    {
        return std::nullopt;
    }

    AudioAsset asset;
    asset.id = NormalizeId(path);
    asset.sourcePath = path.string();
    asset.byteSize = static_cast<std::size_t>(std::filesystem::file_size(path));

    manager.RegisterAudio(std::move(asset));
    return NormalizeId(path);
}

std::optional<std::string> AssetImporter::ImportMeshAndAnimations(const std::filesystem::path& path, AssetManager& manager) const
{
    if (!std::filesystem::exists(path))
    {
        return std::nullopt;
    }

    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (!IsMeshExtension(extension))
    {
        return std::nullopt;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path.string(),
        aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_ImproveCacheLocality);

    if (scene == nullptr)
    {
        return std::nullopt;
    }

    const std::string id = NormalizeId(path);

    MeshAsset meshAsset;
    meshAsset.id = id;
    meshAsset.sourcePath = path.string();
    meshAsset.meshCount = scene->mNumMeshes;

    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        const aiMesh* mesh = scene->mMeshes[meshIndex];
        meshAsset.vertexCount += mesh->mNumVertices;
        meshAsset.indexCount += mesh->mNumFaces * 3;
    }

    manager.RegisterMesh(std::move(meshAsset));

    if (scene->HasAnimations())
    {
        AnimationAsset animationAsset;
        animationAsset.id = id;
        animationAsset.sourcePath = path.string();
        animationAsset.clipNames.reserve(scene->mNumAnimations);

        for (unsigned int animIdx = 0; animIdx < scene->mNumAnimations; ++animIdx)
        {
            const aiAnimation* animation = scene->mAnimations[animIdx];
            animationAsset.clipNames.emplace_back(animation->mName.C_Str());
        }

        manager.RegisterAnimation(std::move(animationAsset));
    }

    return id;
}

std::string AssetImporter::NormalizeId(const std::filesystem::path& path)
{
    std::string id = path.stem().string();
    std::transform(id.begin(), id.end(), id.begin(), [](unsigned char c) {
        if (std::isalnum(c))
        {
            return static_cast<char>(std::tolower(c));
        }

        return '_';
    });

    return id;
}

bool AssetImporter::IsAudioExtension(const std::string& extension)
{
    return extension == ".wav" || extension == ".mp3" || extension == ".ogg";
}

bool AssetImporter::IsMeshExtension(const std::string& extension)
{
    return extension == ".fbx" || extension == ".obj" || extension == ".gltf" || extension == ".glb";
}
