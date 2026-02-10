/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Shader.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 15:53:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/10 15:56:28                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <string>
# include <vulkan/vulkan.h>
# include <memory>

namespace	hel {

class	Device;

struct	Shader {
	std::string				path;
	VkShaderModule			shaderModule {VK_NULL_HANDLE};
	VkShaderStageFlagBits	stage;

	VkPipelineShaderStageCreateInfo	getStageInfo(void) const;

	static std::shared_ptr<Shader>	load(Device &device, const std::string &path);
};

}
