/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ShaderManager.cpp                                                   */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/06/21 16:58:44 by pop-os                                    */
/*                                                                            */
/*  Last Modified: 2026/06/21 17:17:11                                        */
/*             By: pop-os                                                     */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 pop-os                                                 */
/*                                                                            */
/* *************************************************************************  */

#include <vector>

#include "api/vulkan/ShaderManager.hpp"
#include "api/vulkan/Device.hpp"
#include "ecs/AssetManager.hpp"

namespace	hel {

VkPipelineShaderStageCreateInfo		Shader::getStageInfo(void) const {
	VkPipelineShaderStageCreateInfo	info{};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.stage = stage;
	info.module = shaderModule;
	info.pName = "main";
	return (info);
}

std::shared_ptr<Shader>	Shader::load(Device &device, const std::string &path) {
	std::vector<char>	code = AssetManager::readFile(path);
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
	if (vkCreateShaderModule(device.getLogical(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		return (nullptr);//TODO -> give a fallback asset.
	VkDevice	logicalDevice = device.getLogical();
	return (std::shared_ptr<Shader>(
		new Shader{path, shaderModule, stage},
		[logicalDevice](Shader *s){
			if (s->shaderModule != VK_NULL_HANDLE)
				vkDestroyShaderModule(logicalDevice, s->shaderModule, nullptr);
			delete s;
		}
	));
}

std::shared_ptr<Shader>	ShaderManager::get(std::string_view shaderName) {
	auto	it = _shaders.find(shaderName);
	if (it != _shaders.end())
		return (it->second);
	std::string	shaderPath = "shaders/" + std::string(shaderName) + ".spv";
	auto	newShader = _shaders.emplace(shaderName,
							Shader::load(_device, shaderPath));
	return (newShader.first->second);
}

}
