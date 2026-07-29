/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/07/29 14:30:00                                        */
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
#include <memory>

#include "rhi/render/ExecutionContext.hpp"
#include "rhi/context/Swapchain.hpp"
#include "rhi/resources/Buffer.hpp"
#include "utils/Expected.hpp"
#include "utils/Ref.hpp"

namespace	hel {

class	Window;
class	DescriptorPool;
class	DescriptorWriter;
struct	DescriptorSet;

struct	GlobalSetBindings {
	GlobalSetBindings	&addBinding(uint32_t bindingIndex,
									VkDescriptorType descriptorType,
									VkShaderStageFlags stage);

	struct	Binding {
		uint32_t			index;
		VkDescriptorType	type;
		VkShaderStageFlags	stage;
		Ref<Buffer>			buffer{};
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

		void	fillContext(ExecutionContext &execCtx, Window *window);
		void	writeGlobalData(ExecutionContext &execCtx);

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
