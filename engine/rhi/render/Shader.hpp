/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Shader.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/21 16:58:34 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/27 16:44:01                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <memory>

namespace	hel {

class	Device;

struct	Shader {
	std::string				path;
	VkShaderModule			shaderModule {VK_NULL_HANDLE};
	VkShaderStageFlagBits	stage;

	VkPipelineShaderStageCreateInfo	getStageInfo(void) const;

	static std::shared_ptr<Shader>	load(Device *device,
										const std::string &shaderName);
};

}
