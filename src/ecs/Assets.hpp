/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Assets.hpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 15:35:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/03 11:04:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <string>
# include <vector>
# include <vulkan/vulkan.h>
# include <glm/glm.hpp>

namespace	hel {

struct	Shader {
	std::string				_path;
	VkShaderModule			_shaderModule {VK_NULL_HANDLE};
	VkShaderStageFlagBits	_stage;

	VkPipelineShaderStageCreateInfo	getStageInfo(void) const {
		VkPipelineShaderStageCreateInfo	info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.stage = _stage;
		info.module = _shaderModule;
		info.pName = "main";
		return (info);
	}
};

struct	Vertex {
	glm::vec3	position{0.};
	glm::vec3	color{1.};

	static std::vector<VkVertexInputBindingDescription>	getBindingDescriptions(void) {
		std::vector<VkVertexInputBindingDescription>	bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return (bindingDescriptions);
	}

	static std::vector<VkVertexInputAttributeDescription>	getAttributeDescriptions(void) {
		std::vector<VkVertexInputAttributeDescription>	attributeDescriptions{};

		attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
		attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});

		return (attributeDescriptions);
	}
};

}
