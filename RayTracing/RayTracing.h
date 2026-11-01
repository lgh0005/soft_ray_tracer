#pragma once

// Common
#include "Enums.h"

// RayTracing
#include "AABB.h"

// Data
#include "RTData.h"
#include "RTUniforms.h"
#include "RTGeometry.h"

// Accelerates
#include "TLAS.h"
#include "BLAS.h"
#include "Octree.h"
#include "KDTree.h"
#include "BSPTree.h"

// Managers
#include "InstanceManager.h"
#define BDT_INSTANCE bdt::InstanceManager::Instance()