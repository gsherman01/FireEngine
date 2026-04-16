#pragma once

#include "Entity.h"

#include <memory>
#include <utility>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Registry
{
public:
    Entity CreateEntity()
    {
        return ++m_nextEntity;
    }

    template <typename T, typename... TArgs>
    T& AddComponent(Entity entity, TArgs&&... args)
    {
        return GetStorage<T>().Add(entity, T(std::forward<TArgs>(args)...));
    }

    template <typename T>
    bool HasComponent(Entity entity)
    {
        return GetStorage<T>().Has(entity);
    }

    template <typename T>
    T& GetComponent(Entity entity)
    {
        return GetStorage<T>().Get(entity);
    }

    template <typename TFirst, typename... TRest>
    std::vector<Entity> View()
    {
        const std::vector<Entity> firstEntities = GetStorage<TFirst>().GetEntities();
        std::vector<Entity> result;
        result.reserve(firstEntities.size());

        for (Entity entity : firstEntities)
        {
            if ((GetStorage<TRest>().Has(entity) && ...))
            {
                result.push_back(entity);
            }
        }

        return result;
    }

private:
    struct IComponentStorage
    {
        virtual ~IComponentStorage() = default;
    };

    template <typename T>
    class ComponentStorage final : public IComponentStorage
    {
    public:
        T& Add(Entity entity, T&& component)
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

        bool Has(Entity entity) const
        {
            return m_lookup.find(entity) != m_lookup.end();
        }

        T& Get(Entity entity)
        {
            return m_components.at(m_lookup.at(entity));
        }

        std::vector<Entity> GetEntities() const
        {
            return m_entities;
        }

    private:
        std::vector<T> m_components;
        std::vector<Entity> m_entities;
        std::unordered_map<Entity, std::size_t> m_lookup;
    };

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

    Entity m_nextEntity = InvalidEntity;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>> m_componentStorages;
};
