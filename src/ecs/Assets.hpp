/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Assets.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 15:35:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/26 18:23:39                                        */
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

namespace	hel {

struct	Shader {
	std::string				_path;
	VkShaderModule			_shaderModule {VK_NULL_HANDLE};
	VkShaderStageFlagBits	_stage;
};

}
