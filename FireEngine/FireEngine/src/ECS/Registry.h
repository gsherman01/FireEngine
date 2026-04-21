#pragma once

#include "Entity.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class Registry
{
public:
    using Signature = std::uint64_t;

    Entity CreateEntity()
    {
        const EntityId entityId = ++m_nextEntity;
        m_aliveEntities.insert(entityId);
        m_signatures[entityId] = 0;
        RefreshEntitySystemMembership(entityId);
        return Entity(entityId, this);
    }

    void DestroyEntity(EntityId entity)
    {
        if (!HasEntity(entity))
        {
            return;
        }

        for (auto& [_, storage] : m_componentStorages)
        {
            storage->Remove(entity);
        }

        m_signatures.erase(entity);
        m_aliveEntities.erase(entity);

        for (auto& [_, system] : m_systemStorages)
        {
            system.entities.erase(entity);
        }
    }

    void DestroyEntity(const Entity& entity)
    {
        DestroyEntity(entity.GetId());
    }

    bool HasEntity(EntityId entity) const
    {
        return m_aliveEntities.find(entity) != m_aliveEntities.end();
    }

    std::vector<Entity> GetEntities()
    {
        std::vector<Entity> entities;
        entities.reserve(m_aliveEntities.size());
        for (EntityId id : m_aliveEntities)
        {
            entities.emplace_back(id, this);
        }
        return entities;
    }

    template <typename T, typename... TArgs>
    T& AddComponent(EntityId entity, TArgs&&... args)
    {
        assert(HasEntity(entity));
        T& component = GetStorage<T>().Add(entity, T(std::forward<TArgs>(args)...));
        m_signatures[entity] |= GetComponentMask<T>();
        RefreshEntitySystemMembership(entity);
        return component;
    }

    template <typename T>
    bool RemoveComponent(EntityId entity)
    {
        if (!HasEntity(entity))
        {
            return false;
        }

        ComponentStorage<T>& storage = GetStorage<T>();
        if (!storage.Remove(entity))
        {
            return false;
        }

        m_signatures[entity] &= ~GetComponentMask<T>();
        RefreshEntitySystemMembership(entity);
        return true;
    }

    template <typename T>
    bool HasComponent(EntityId entity) const
    {
        if (!HasEntity(entity))
        {
            return false;
        }

        auto storageIt = m_componentStorages.find(std::type_index(typeid(T)));
        if (storageIt == m_componentStorages.end())
        {
            return false;
        }

        return static_cast<const ComponentStorage<T>*>(storageIt->second.get())->Has(entity);
    }

    template <typename T>
    T& GetComponent(EntityId entity)
    {
        assert(HasEntity(entity));
        return GetStorage<T>().Get(entity);
    }

    Signature GetSignature(EntityId entity) const
    {
        auto signatureIt = m_signatures.find(entity);
        return signatureIt != m_signatures.end() ? signatureIt->second : 0;
    }

    template <typename TFirst, typename... TRest>
    std::vector<Entity> View()
    {
        std::vector<Entity> result;
        auto storageIt = m_componentStorages.find(std::type_index(typeid(TFirst)));
        if (storageIt == m_componentStorages.end())
        {
            return result;
        }

        const std::vector<EntityId>& firstEntities = static_cast<ComponentStorage<TFirst>*>(storageIt->second.get())->GetEntities();
        result.reserve(firstEntities.size());

        for (EntityId entityId : firstEntities)
        {
            if ((HasComponent<TRest>(entityId) && ...))
            {
                result.emplace_back(entityId, this);
            }
        }

        return result;
    }

    template <typename TSystem, typename... TArgs>
    TSystem& RegisterSystem(TArgs&&... args)
    {
        const std::type_index type = std::type_index(typeid(TSystem));
        SystemStorage& storage = m_systemStorages[type];

        if (!storage.instance)
        {
            storage.instance = MakeSystem<TSystem>(std::forward<TArgs>(args)...);
        }

        return *static_cast<TSystem*>(storage.instance.get());
    }

    template <typename TSystem>
    TSystem& GetSystem()
    {
        return *static_cast<TSystem*>(m_systemStorages.at(std::type_index(typeid(TSystem))).instance.get());
    }

    template <typename TSystem, typename... TComponents>
    void SetSystemSignature()
    {
        SystemStorage& system = m_systemStorages[std::type_index(typeid(TSystem))];
        system.requiredMask = (GetComponentMask<TComponents>() | ... | 0);
        RefreshSystemMembership(system);
    }

    template <typename TSystem>
    const std::unordered_set<EntityId>& GetSystemEntities() const
    {
        return m_systemStorages.at(std::type_index(typeid(TSystem))).entities;
    }

private:
    struct IComponentStorage
    {
        virtual ~IComponentStorage() = default;
        virtual bool Remove(EntityId entity) = 0;
    };

    template <typename T>
    class ComponentStorage final : public IComponentStorage
    {
    public:
        T& Add(EntityId entity, T&& component)
        {
            auto found = m_lookup.find(entity);
            if (found != m_lookup.end())
            {
                m_components[found->second] = std::move(component);
                return m_components[found->second];
            }

            const std::size_t index = m_components.size();
            m_lookup[entity] = index;
            m_entities.push_back(entity);
            m_components.push_back(std::move(component));
            return m_components.back();
        }

        bool Has(EntityId entity) const
        {
            return m_lookup.find(entity) != m_lookup.end();
        }

        T& Get(EntityId entity)
        {
            return m_components.at(m_lookup.at(entity));
        }

        const std::vector<EntityId>& GetEntities() const
        {
            return m_entities;
        }

        bool Remove(EntityId entity) override
        {
            auto found = m_lookup.find(entity);
            if (found == m_lookup.end())
            {
                return false;
            }

            const std::size_t removedIndex = found->second;
            const std::size_t lastIndex = m_components.size() - 1;

            if (removedIndex != lastIndex)
            {
                m_components[removedIndex] = std::move(m_components[lastIndex]);
                const EntityId movedEntity = m_entities[lastIndex];
                m_entities[removedIndex] = movedEntity;
                m_lookup[movedEntity] = removedIndex;
            }

            m_components.pop_back();
            m_entities.pop_back();
            m_lookup.erase(found);
            return true;
        }

    private:
        std::vector<T> m_components;
        std::vector<EntityId> m_entities;
        std::unordered_map<EntityId, std::size_t> m_lookup;
    };

    struct SystemStorage
    {
        Signature requiredMask = 0;
        std::unordered_set<EntityId> entities;
        std::unique_ptr<void, void(*)(void*)> instance{nullptr, [](void*){}};
    };

    template <typename T>
    static std::size_t GetComponentType()
    {
        static const std::size_t typeId = s_nextComponentType++;
        return typeId;
    }

    template <typename T>
    static Signature GetComponentMask()
    {
        return Signature(1) << GetComponentType<T>();
    }

    template <typename T>
    ComponentStorage<T>& GetStorage()
    {
        const std::type_index type = std::type_index(typeid(T));
        auto found = m_componentStorages.find(type);

        if (found == m_componentStorages.end())
        {
            auto storage = std::make_unique<ComponentStorage<T>>();
            ComponentStorage<T>* rawStorage = storage.get();
            m_componentStorages[type] = std::move(storage);
            return *rawStorage;
        }

        return *static_cast<ComponentStorage<T>*>(found->second.get());
    }

    void RefreshEntitySystemMembership(EntityId entity)
    {
        const Signature entityMask = GetSignature(entity);
        for (auto& [_, system] : m_systemStorages)
        {
            if ((entityMask & system.requiredMask) == system.requiredMask)
            {
                system.entities.insert(entity);
            }
            else
            {
                system.entities.erase(entity);
            }
        }
    }

    void RefreshSystemMembership(SystemStorage& system)
    {
        system.entities.clear();
        for (EntityId entityId : m_aliveEntities)
        {
            const Signature entityMask = GetSignature(entityId);
            if ((entityMask & system.requiredMask) == system.requiredMask)
            {
                system.entities.insert(entityId);
            }
        }
    }

    template <typename TSystem, typename... TArgs>
    static std::unique_ptr<void, void(*)(void*)> MakeSystem(TArgs&&... args)
    {
        return {new TSystem(std::forward<TArgs>(args)...), [](void* ptr) { delete static_cast<TSystem*>(ptr); }};
    }

private:
    inline static std::size_t s_nextComponentType = 0;

    EntityId m_nextEntity = InvalidEntityId;
    std::unordered_set<EntityId> m_aliveEntities;
    std::unordered_map<EntityId, Signature> m_signatures;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> m_componentStorages;
    std::unordered_map<std::type_index, SystemStorage> m_systemStorages;
};

inline bool Entity::IsValid() const
{
    return m_registry != nullptr && m_id != InvalidEntityId && m_registry->HasEntity(m_id);
}

template <typename T, typename... TArgs>
T& Entity::AddComponent(TArgs&&... args)
{
    return m_registry->AddComponent<T>(m_id, std::forward<TArgs>(args)...);
}

template <typename T>
bool Entity::HasComponent() const
{
    return m_registry->HasComponent<T>(m_id);
}

template <typename T>
T& Entity::GetComponent()
{
    return m_registry->GetComponent<T>(m_id);
}
