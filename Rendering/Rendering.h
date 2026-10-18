#pragma once

// Data & Enums
#include "Uniforms.h"
#include "Instances.h"
#include "Enums.h"

// Managers
#include "RenderManager.h"
#define BDT_RENDER bdt::RenderManager::Instance()

// Rendering Interfaces
#include "IRenderBus.h"
#include "IRenderPass.h"
#include "IRenderPipeline.h"
#include "IRenderTarget.h"

// Scene
#include "RenderScene.h"
#include "RenderObject.h"

// Render Targets
#include "Framebuffer2D.h"
#include "RenderTexture2D.h"
#include "RenderTextureCube.h"

// Materials
#include "RenderMaterial.h"

// Instancing
#include "InstanceBuffer.h"
#include "StaticRenderBatch.h"
#include "SkinnedRenderBatch.h"
#include "StaticRenderQueue.h"
#include "SkinnedRenderQueue.h"
