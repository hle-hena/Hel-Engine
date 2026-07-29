/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: ExecutionContext.hpp                                                */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/07/25 17:04:20 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/29 14:27:35                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

#include "rhi/window/Window.hpp"
#include "rhi/resources/Descriptors.hpp"

namespace	hel {

struct	RenderRequest;

struct	GlobalData {
	private:
		struct	EngineData {
			std::shared_ptr<void>	data;
		};
		struct	ShaderData {
			std::shared_ptr<void>	data;
			Ref<Buffer>				buffer;
			uint32_t				bindingIndex;
		};

		template <typename Key, typename Tp>
		using u_map = std::unordered_map<Key, Tp>;
		u_map<std::string, EngineData>	_engineGlobals;
		u_map<std::string, ShaderData>	_shaderGlobals;
		std::optional<std::string>		_error;
		bool							_locked{false};

	public:
		GlobalData	*addData(const std::string &key, std::shared_ptr<void> data);
		GlobalData	*addData(const std::string &key, std::shared_ptr<void> data,
								Ref<Buffer> buffer, uint32_t bindingIndex);

		template <typename T>
		T	*get(const std::string &key) {
			if (auto it = _engineGlobals.find(key); it != _engineGlobals.end())
				return static_cast<T *>(it->second.data.get());
			if (auto it = _shaderGlobals.find(key); it != _shaderGlobals.end())
				return static_cast<T *>(it->second.data.get());
			return nullptr;
		}

		u_map<std::string, ShaderData>	&list(void);
		expected<void>	lock(void);
};

struct	ExecutionContext {
	ExecutionContext(uint32_t frameIndex, GlobalData *globalData)
		:	globals(globalData) {
		this->frameIndex = frameIndex;
	};

	Window					*window{nullptr};
	RenderRequest			*request{nullptr};

	VkCommandBuffer			commandBuffer{VK_NULL_HANDLE};

	GlobalData				*globals;
	VkDescriptorSet			globalSet{VK_NULL_HANDLE};
	uint32_t				setStride{0};
	VkDescriptorSetLayout	globalLayout{VK_NULL_HANDLE};

	DescriptorPool			*descriptorPool{nullptr};

	uint32_t				passIndex{0};
	uint32_t				frameIndex{0};
	uint32_t				swapIndex{0};
};

}
