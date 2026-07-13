/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/13 16:01:34                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

#include <vulkan/vulkan.h>
#include <array>
#include <optional>
#include <map>
#include <unordered_map>
#include <memory>

#include "HelExpected.hpp"
#include "utils/Setters.hpp"
#include "api/vulkan/Swapchain.hpp"
#include "api/vulkan/Buffer.hpp"
#include "utils/Ref.hpp"

namespace	hel {

class	Window;
class	DescriptorPool;
class	DescriptorWriter;
struct	RenderRequest;
struct	DescriptorSet;

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
				return static_cast<T *>(it->second.data);
			if (auto it = _shaderGlobals.find(key); it != _shaderGlobals.end())
				return static_cast<T *>(it->second.data);
			return nullptr;
		}

		PASSKEY(FrameKey, Frame);
		u_map<std::string, ShaderData>	&list(FrameKey);
		PASSKEY(EngineKey, Engine)
		expected<void>	lock(EngineKey);
};

struct	FrameContext {
	FrameContext(uint32_t frameIndex, GlobalData *globalData)
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



struct	GlobalSetBindings {
	GlobalSetBindings	&addBinding(uint32_t bindingIndex,
									VkDescriptorType descriptorType,
									VkShaderStageFlags stage);

	struct	Binding {
		uint32_t			index;
		VkDescriptorType	type;
		VkShaderStageFlags	stage;
		Ref<Buffer>			buffer{nullptr};
		bool				dynamicBinding{false};
	};

	private:
		std::optional<std::string>	_error;
		std::map<uint32_t, Binding>	_bindings;
		bool	contains(uint32_t key);
		Binding	&operator[](uint32_t index);
	friend class	Frame;
};

class	Frame {
	public:
		Frame(void) = default;
		~Frame(void);

		static constexpr uint32_t	MAX_PASS_COUNT = 32;

		expected<void>	init(Device *device, const
											GlobalSetBindings &setConfig);
		expected<void>	bindBuffers(GlobalData *globalData);
		expected<void>	validateGlobalSet(void);

		void	fillContext(FrameContext &frameContext, Window *window);
		void	writeGlobalData(FrameContext &frameContext);

		static VkDescriptorSetLayout	getGlobalLayout(void)
			{ return _globalLayout; }

	private:
		Device	*_device;

		expected<void>							createCommandBuffers(void);
		VkCommandPool							_commandPool{VK_NULL_HANDLE};
		std::array<VkCommandBuffer,
			Swapchain::MAX_FRAMES_IN_FLIGHT>	_commandBuffers{};

		void			writeToUBO(void *data, uint32_t bindingIndex,
									uint32_t passIndex, uint32_t frameIndex);
		expected<void>	createGlobalSets(void);
		GlobalSetBindings							_bindingConfig;
		std::unique_ptr<DescriptorPool>				_descriptorPool;
		std::unique_ptr<DescriptorSet>				_descriptorSets{};
		uint32_t									_setStride{0};
		std::unique_ptr<DescriptorWriter>			_writer{nullptr};
		static VkDescriptorSetLayout				_globalLayout;

		std::array<std::unique_ptr<DescriptorPool>,
			Swapchain::MAX_FRAMES_IN_FLIGHT>		_dynamicPools{};
};

}
