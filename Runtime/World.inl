#pragma once
#include "StaticModelActor.h"
#include "SkinnedModelActor.h"
#include "PrimitiveActor.h"
#include "PointLightActor.h"
#include "SpotLightActor.h"
#include "DirectionalLightActor.h"

namespace bdt
{
    template<typename T, typename... TArgs>
    T* RegisterActor(
        List<std::unique_ptr<T>>& storage,
        List<Actor*>& actors,
        TArgs&&... args)
    {
        auto actor = std::make_unique<T>(std::forward<TArgs>(args)...);
        T* ptr = actor.get();
        storage.push_back(std::move(actor));
        actors.push_back(ptr);
        return ptr;
    }

    template<typename... TArgs>
    StaticModelActor* World::AddStaticModelActor(TArgs&&... args)
    {
        return RegisterActor(m_staticModelActors, m_actors, std::forward<TArgs>(args)...);
    }

    template<typename... TArgs>
    SkinnedModelActor* World::AddSkinnedModelActor(TArgs&&... args)
    {
        return RegisterActor(m_skinnedModelActors, m_actors, std::forward<TArgs>(args)...);
    }

    template<typename... TArgs>
    PrimitiveActor* World::AddPrimitiveActor(TArgs&&... args)
    {
        return RegisterActor(m_staticMeshActors, m_actors, std::forward<TArgs>(args)...);
    }

    template<typename... TArgs>
    PointLightActor* World::AddPointLightActor(TArgs&&... args)
    {
        return RegisterActor(m_pointLightActors, m_actors, std::forward<TArgs>(args)...);
    }

    template<typename... TArgs>
    SpotLightActor* World::AddSpotLightActor(TArgs&&... args)
    {
        return RegisterActor(m_spotLightActors, m_actors, std::forward<TArgs>(args)...);
    }

    template<typename... TArgs>
    DirectionalLightActor* World::AddDirectionalLightActor(TArgs&&... args)
    {
        return RegisterActor(m_directionalLightActors, m_actors, std::forward<TArgs>(args)...);
    }
}