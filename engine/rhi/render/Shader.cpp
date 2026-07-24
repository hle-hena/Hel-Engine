/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Shader.cpp                                                          */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/21 16:58:44 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/07/24 17:07:57                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include "rhi/render/Shader.hpp"
#include "rhi/context/Device.hpp"
#include "utils/str_utils.hpp"

#include <vector>

namespace	hel {

VkPipelineShaderStageCreateInfo		Shader::getStageInfo(void) const {
	VkPipelineShaderStageCreateInfo	info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.stage = stage;
	info.module = shaderModule;
	info.pName = "main";
	return (info);
}

std::shared_ptr<Shader>	Shader::load(Device *device, const std::string &shaderName) {
	std::string	path = "shaders/" + shaderName + ".spv";
	std::vector<char>	code = readFile(path);
	if (code.size() == 0)
		return (nullptr);//TODO -> give a fallback asset.
	VkShaderStageFlagBits	stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	if (path.find(".vert") != std::string::npos) stage = VK_SHADER_STAGE_VERTEX_BIT;
	else if (path.find(".frag") != std::string::npos) stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	if (stage == VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
		return (nullptr);//TODO -> give a fallback asset.
	VkShaderModule		shaderModule {VK_NULL_HANDLE};
	VkShaderModuleCreateInfo	createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
	if (vkCreateShaderModule(device->getLogical(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		return (nullptr);//TODO -> give a fallback asset.
	VkDevice	logicalDevice = device->getLogical();
	return (std::shared_ptr<Shader>(
		new Shader{path, shaderModule, stage},
		[logicalDevice](Shader *s){
			if (s->shaderModule != VK_NULL_HANDLE)
				vkDestroyShaderModule(logicalDevice, s->shaderModule, nullptr);
			delete s;
		}
	));
}

}
