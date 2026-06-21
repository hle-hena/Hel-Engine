/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ShaderManager.hpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/21 16:58:34 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/21 17:12:51                                        */
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
#include <unordered_map>

namespace	hel {

class	Device;

struct	Shader {
	std::string				path;
	VkShaderModule			shaderModule {VK_NULL_HANDLE};
	VkShaderStageFlagBits	stage;

	VkPipelineShaderStageCreateInfo	getStageInfo(void) const;

	static std::shared_ptr<Shader>	load(Device &device,
										const std::string &path);
};

class	ShaderManager {
	ShaderManager(Device &device)	: _device(device) {}

	std::shared_ptr<Shader>	get(std::string_view shaderName);

	private:
		Device							&_device;
		std::unordered_map<
			std::string_view,
			std::shared_ptr<Shader>>	_shaders{};
};

}
