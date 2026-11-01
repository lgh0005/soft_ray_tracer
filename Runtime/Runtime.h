#pragma once

// Module Dependencies
#include <Core/Core.h>
#include <Resources/Resource.h>
#include <Rendering/Rendering.h>

// Common
#include "Enums.h"

// Components
#include "SceneComponent.h"
#include "RootComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "MovementComponent.h"
#include "CharacterMovement.h"
#include "FloatingMovement.h"
#include "AnimComponent.h"
#include "PrimitiveRenderer.h"
#include "BoxRenderer.h"
#include "SphereRenderer.h"
#include "ModelRenderer.h"
#include "StaticModelRenderer.h"
#include "SkinnedModelRenderer.h"

// Actors
#include "Actor.h"
#include "Pawn.h"
#include "CameraActor.h"
#include "LightActor.h"
#include "DirectionalLightActor.h"
#include "PointLightActor.h"
#include "SpotLightActor.h"
#include "PrimitiveActor.h"
#include "ModelActor.h"
#include "StaticModelActor.h"
#include "SkinnedModelActor.h"

// Mechanics
#include "AnimController.h"
#include "AnimState.h"
#include "Frustum.h"

// World
#include "World.h"

// Managers
#include "WorldManager.h"
#define BDT_WORLD bdt::WorldManager::Instance()

// Application
#include "App.h"
