#pragma once

// Common Dependencies
#include <Core/Core.h>
#include <Resources/Resource.h>

// Core GPU Resources
#include <Core/GLBuffer.h>
#include <Core/GLStorageBuffer.h>
#include <Core/GLVertexArray.h>
#include <Core/GLVertices.h>
#include <Core/GLMesh.h>
#include <Core/GLSimpleMesh.h>
#include <Core/GLStaticMesh.h>
#include <Core/GLSkinnedMesh.h>
#include <Core/GLScreenMesh.h>
#include <Core/GLTexture2D.h>
#include <Core/GLTextureCube.h>
#include <Core/GLSampler.h>
#include <Core/GLGraphicsProgram.h>
#include <Core/GLComputeProgram.h>

// Animation Resources
#include <Resources/Skeleton.h>
#include <Resources/Pose.h>

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
#include "ImageBasedLight.h"
#include "Framebuffer2D.h"
#include "FramebufferCube.h"
#include "RenderTexture2D.h"
#include "RenderTextureArray2D.h"
#include "RenderTextureCube.h"
#include "RenderScreen.h"
#include "RenderViewport.h"

// Materials
#include "RenderMaterial.h"

// Instancing
#include "RenderBatch.h"
#include "StaticRenderBatch.h"
#include "SkinnedRenderBatch.h"
#include "StaticRenderQueue.h"
#include "SkinnedRenderQueue.h"
