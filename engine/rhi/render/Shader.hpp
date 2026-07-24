/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Shader.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/21 16:58:34 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/24 17:28:47                                        */
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

	static std::shared_ptr<Shader>	load(Device *device,
										const std::string &shaderName);
};

class	ShaderCache {
	public:
		static void	init(Device *device) { _device = device; }
		static void	clear(void) { _shaders.clear(); }

		static std::shared_ptr<Shader>	get(const std::string &path) {
			auto	it = _shaders.find(path);
			if (it != _shaders.end())
				return it->second;

			auto	ptr = Shader::load(_device, path);
			if (ptr)
				_shaders[path] = ptr;
			return ptr;
		}

	private:

		static Device								*_device;
		static std::unordered_map<std::string,
						std::shared_ptr<Shader>>	_shaders;
};

}
