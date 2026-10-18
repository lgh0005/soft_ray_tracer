#pragma once
#include "Pawn.h"

namespace bdt
{
    BDT_CLASS_PTR(ModelRenderer)

    class ModelActor : public Pawn
    {
        using Super = Pawn;

    public:
        explicit ModelActor(const String& tag);
        virtual ~ModelActor() override;
        virtual EActorType GetActorType() const override;

    public:
        ModelRenderer* GetModelRenderer() const;
    };
}