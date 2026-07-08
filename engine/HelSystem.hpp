/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: HelSystem.hpp                                                       */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/15 19:04:55 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/06 11:00:12                                        */
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

#include "ecs/ISystem.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Registry.hpp"
#include "platform/input/InputState.hpp"
#include "platform/window/Window.hpp"

#include "core/Frame.hpp"
#include "core/PhaseDependency.hpp"
#include "core/SystemManager.hpp"

#include "core/DrawQueue.hpp"
#include "core/ReadQueue.hpp"
#include "core/RenderQueue.hpp"

#include "api/vulkan/Device.hpp"
#include "api/vulkan/Descriptors.hpp"
#include "api/vulkan/Renderer.hpp"
#include "api/vulkan/PipelineMap.hpp"
#include "api/vulkan/Shader.hpp"
#include "api/vulkan/Sampler.hpp"
#include "api/vulkan/Buffer.hpp"
#include "api/vulkan/Image.hpp"

using enum hel::ImageDep::Usage;

// IWYU pragma: end_exports
