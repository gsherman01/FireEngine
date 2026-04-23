#include "AssetImporter.h"

#include "AssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/gtc/type_ptr.hpp>

namespace
{
    void AddBoneWeight(Vertex& vertex, int boneId, float weight)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (vertex.boneWeights[i] == 0.0f)
            {
                vertex.boneIds[i] = boneId;
                vertex.boneWeights[i] = weight;
                return;
            }
        }
    }

    int ResolveBoneIndex(Model& model, const aiBone* bone)
    {
        const std::string boneName = bone->mName.C_Str();
        auto found = model.boneNameToIndex.find(boneName);
        if (found != model.boneNameToIndex.end())
        {
            return found->second;
        }

        const int newIndex = static_cast<int>(model.bones.size());
        BoneInfo info;
        const aiMatrix4x4& offset = bone->mOffsetMatrix;
        info.offset = glm::transpose(glm::make_mat4(&offset.a1));

        model.boneNameToIndex[boneName] = newIndex;
        model.bones.push_back(info);
        return newIndex;
    }

    Material BuildMaterial(const aiMaterial* material, const std::filesystem::path& modelDir, AssetManager& manager)
    {
        Material result;

        auto loadTexture = [&](aiTextureType type) -> std::shared_ptr<Texture> {
            if (material->GetTextureCount(type) == 0)
            {
                return nullptr;
            }

            aiString texturePath;
            if (material->GetTexture(type, 0, &texturePath) != aiReturn_SUCCESS)
            {
                return nullptr;
            }

            const std::filesystem::path resolved = modelDir / texturePath.C_Str();
            return manager.LoadTexture(resolved);
        };

        result.diffuseTexture = loadTexture(aiTextureType_DIFFUSE);
        result.specularTexture = loadTexture(aiTextureType_SPECULAR);
        result.normalTexture = loadTexture(aiTextureType_NORMALS);

        aiColor3D color(1.0f, 1.0f, 1.0f);
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS)
        {
            result.albedo = glm::vec3(color.r, color.g, color.b);
        }

        float shininess = 32.0f;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS)
        {
            result.shininess = shininess;
        }

        return result;
    }

    void ImportAnimations(const aiScene* scene, Model& model)
    {
        if (scene->mNumAnimations == 0)
        {
            return;
        }

        model.animations.reserve(scene->mNumAnimations);
        for (unsigned int animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex)
        {
            const aiAnimation* sourceAnimation = scene->mAnimations[animationIndex];
            if (sourceAnimation == nullptr)
            {
                continue;
            }

            AnimationClip clip;
            clip.name = sourceAnimation->mName.length > 0
                ? sourceAnimation->mName.C_Str()
                : ("Animation " + std::to_string(animationIndex));
            clip.durationTicks = sourceAnimation->mDuration;
            clip.ticksPerSecond = sourceAnimation->mTicksPerSecond > 0.0 ? sourceAnimation->mTicksPerSecond : 25.0;

            clip.channels.reserve(sourceAnimation->mNumChannels);
            for (unsigned int channelIndex = 0; channelIndex < sourceAnimation->mNumChannels; ++channelIndex)
            {
                const aiNodeAnim* sourceChannel = sourceAnimation->mChannels[channelIndex];
                if (sourceChannel == nullptr)
                {
                    continue;
                }

                const std::string boneName = sourceChannel->mNodeName.C_Str();
                auto foundBone = model.boneNameToIndex.find(boneName);
                if (foundBone == model.boneNameToIndex.end())
                {
                    continue;
                }

                BoneAnimationChannel channel;
                channel.boneIndex = foundBone->second;

                channel.positions.reserve(sourceChannel->mNumPositionKeys);
                for (unsigned int i = 0; i < sourceChannel->mNumPositionKeys; ++i)
                {
                    const aiVectorKey& sourceKey = sourceChannel->mPositionKeys[i];
                    channel.positions.push_back({
                        sourceKey.mTime,
                        glm::vec3(sourceKey.mValue.x, sourceKey.mValue.y, sourceKey.mValue.z)});
                }

                channel.rotations.reserve(sourceChannel->mNumRotationKeys);
                for (unsigned int i = 0; i < sourceChannel->mNumRotationKeys; ++i)
                {
                    const aiQuatKey& sourceKey = sourceChannel->mRotationKeys[i];
                    channel.rotations.push_back({
                        sourceKey.mTime,
                        glm::quat(sourceKey.mValue.w, sourceKey.mValue.x, sourceKey.mValue.y, sourceKey.mValue.z)});
                }

                channel.scales.reserve(sourceChannel->mNumScalingKeys);
                for (unsigned int i = 0; i < sourceChannel->mNumScalingKeys; ++i)
                {
                    const aiVectorKey& sourceKey = sourceChannel->mScalingKeys[i];
                    channel.scales.push_back({
                        sourceKey.mTime,
                        glm::vec3(sourceKey.mValue.x, sourceKey.mValue.y, sourceKey.mValue.z)});
                }

                clip.channels.push_back(std::move(channel));
            }

            model.animations.push_back(std::move(clip));
        }
    }
}

std::shared_ptr<Model> AssetImporter::ImportModel(const std::filesystem::path& path, AssetManager& manager) const
{
    if (!std::filesystem::exists(path))
    {
        return nullptr;
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path.string(),
        aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace |
            aiProcess_GenSmoothNormals |
            aiProcess_ImproveCacheLocality |
            aiProcess_LimitBoneWeights);

    if (scene == nullptr || scene->mRootNode == nullptr)
    {
        return nullptr;
    }

    auto model = std::make_shared<Model>();
    model->sourcePath = path.string();

    const std::filesystem::path modelDir = path.parent_path();
    model->materials.reserve(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        model->materials.push_back(BuildMaterial(scene->mMaterials[i], modelDir, manager));
    }

    model->meshes.reserve(scene->mNumMeshes);
    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        const aiMesh* sourceMesh = scene->mMeshes[meshIndex];

        std::vector<Vertex> vertices(sourceMesh->mNumVertices);
        std::vector<unsigned int> indices;
        indices.reserve(sourceMesh->mNumFaces * 3);

        for (unsigned int vertexIndex = 0; vertexIndex < sourceMesh->mNumVertices; ++vertexIndex)
        {
            Vertex vertex{};
            vertex.position[0] = sourceMesh->mVertices[vertexIndex].x;
            vertex.position[1] = sourceMesh->mVertices[vertexIndex].y;
            vertex.position[2] = sourceMesh->mVertices[vertexIndex].z;

            if (sourceMesh->HasNormals())
            {
                vertex.normal[0] = sourceMesh->mNormals[vertexIndex].x;
                vertex.normal[1] = sourceMesh->mNormals[vertexIndex].y;
                vertex.normal[2] = sourceMesh->mNormals[vertexIndex].z;
            }

            if (sourceMesh->HasTextureCoords(0))
            {
                vertex.uv[0] = sourceMesh->mTextureCoords[0][vertexIndex].x;
                vertex.uv[1] = sourceMesh->mTextureCoords[0][vertexIndex].y;
            }

            if (sourceMesh->HasTangentsAndBitangents())
            {
                vertex.tangent[0] = sourceMesh->mTangents[vertexIndex].x;
                vertex.tangent[1] = sourceMesh->mTangents[vertexIndex].y;
                vertex.tangent[2] = sourceMesh->mTangents[vertexIndex].z;
            }

            for (int i = 0; i < 4; ++i)
            {
                vertex.boneIds[i] = -1;
                vertex.boneWeights[i] = 0.0f;
            }

            vertices[vertexIndex] = vertex;
        }

        for (unsigned int faceIndex = 0; faceIndex < sourceMesh->mNumFaces; ++faceIndex)
        {
            const aiFace& face = sourceMesh->mFaces[faceIndex];
            for (unsigned int i = 0; i < face.mNumIndices; ++i)
            {
                indices.push_back(face.mIndices[i]);
            }
        }

        for (unsigned int boneIndex = 0; boneIndex < sourceMesh->mNumBones; ++boneIndex)
        {
            const aiBone* bone = sourceMesh->mBones[boneIndex];
            const int resolvedBoneIndex = ResolveBoneIndex(*model, bone);
            for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
            {
                const aiVertexWeight& weight = bone->mWeights[weightIndex];
                if (weight.mVertexId < vertices.size())
                {
                    AddBoneWeight(vertices[weight.mVertexId], resolvedBoneIndex, weight.mWeight);
                }
            }
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(vertices, indices);

        ModelMesh modelMesh;
        modelMesh.mesh = mesh;
        modelMesh.materialIndex = sourceMesh->mMaterialIndex < model->materials.size() ? sourceMesh->mMaterialIndex : 0;
        model->meshes.push_back(modelMesh);
    }

    ImportAnimations(scene, *model);

    manager.RegisterModel(path, model);
    return model;
}
