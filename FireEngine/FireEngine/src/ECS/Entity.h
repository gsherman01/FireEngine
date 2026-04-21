#pragma once

#include <cstdint>

using EntityId = std::uint32_t;
static constexpr EntityId InvalidEntityId = 0;

class Registry;

class Entity
{
public:
    Entity() = default;
    Entity(EntityId id, Registry* registry)
        : m_id(id)
        , m_registry(registry)
    {
    }

    EntityId GetId() const { return m_id; }
    bool IsValid() const;

    template <typename T, typename... TArgs>
    T& AddComponent(TArgs&&... args);

    template <typename T>
    bool HasComponent() const;

    template <typename T>
    T& GetComponent();

    explicit operator bool() const { return IsValid(); }

private:
    EntityId m_id = InvalidEntityId;
    Registry* m_registry = nullptr;
};
