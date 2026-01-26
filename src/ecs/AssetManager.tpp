/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: AssetManager.tpp                                                    */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/01/26 14:40:07 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/01/26 17:02:20                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/AssetManager.hpp"
#include "ecs/Assets.hpp"
#include "api/vulkan/Device.hpp"

#include <iostream>

namespace	hel {

template <typename Component>
std::shared_ptr<Component>	AssetManager::get(const std::string &path) {
	assetGroup	&group = _assets[typeid(Component)];
	if (group.find(path) != group.end())
		return (std::static_pointer_cast<Component>(group[path]));
	std::cout << "\n\nCreating a new onneeeeeeeeee\n\n" << std::endl;
	std::shared_ptr<Component>	ptr = load<Component>(path);
	group[path] = ptr;
	return (ptr);
}

template <typename Component>
std::shared_ptr<Component>	AssetManager::load(const std::string &path) {
	return (std::make_shared<Component>(readFile(path)));
}

template <>
inline std::shared_ptr<Shader>	AssetManager::load(const std::string &path) {
	std::vector<char>	code = readFile(path);
	if (code.size() == 0)
		return (nullptr);//TODO -> give a fallback asset.
	VkShaderStageFlagBits	stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	if (path.find(".vert") != std::string::npos) stage = VK_SHADER_STAGE_VERTEX_BIT;
	else if (path.find(".frag") != std::string::npos) stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	else
		return (nullptr);//TODO -> give a fallback asset.
	VkShaderModule		module {VK_NULL_HANDLE};
	VkShaderModuleCreateInfo	createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
	if (vkCreateShaderModule(_device.getLogical(), &createInfo, nullptr, &module) != VK_SUCCESS)
		return (nullptr);//TODO -> give a fallback asset.
	return (std::shared_ptr<Shader>(
		new Shader{path, module, stage},
		[&](Shader *s){
			if (s->_module != VK_NULL_HANDLE)
				vkDestroyShaderModule(_device.getLogical(), s->_module, nullptr);
		}
	));
}

}
