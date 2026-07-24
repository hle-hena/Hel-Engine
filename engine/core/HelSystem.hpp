/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HelSystem.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/15 19:04:55 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/24 15:29:14                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

// IWYU pragma: begin_exports

#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "utils/mathUtils.hpp"

#include "core/ecs/ISystem.hpp"
#include "core/ecs/Entity.hpp"
#include "core/ecs/Registry.hpp"
#include "core/input/InputState.hpp"
#include "rhi/window/Window.hpp"

#include "core/Frame.hpp"

#include "core/scheduler/PhaseDependency.hpp"
#include "core/scheduler/SystemManager.hpp"
#include "core/scheduler/DrawQueue.hpp"
#include "core/scheduler/ReadQueue.hpp"
#include "core/scheduler/RenderQueue.hpp"

#include "rhi/context/Device.hpp"
#include "rhi/render/Renderer.hpp"
#include "rhi/render/PipelineMap.hpp"
#include "rhi/render/Shader.hpp"
#include "rhi/resources/Descriptors.hpp"
#include "rhi/resources/Sampler.hpp"
#include "rhi/resources/Buffer.hpp"
#include "rhi/resources/Image.hpp"

using enum hel::ImageDep::Usage;

// IWYU pragma: end_exports
