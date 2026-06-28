/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Frame.hpp                                                           */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/03/13 15:47:41 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/06/27 20:54:15                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#pragma once

# include <vulkan/vulkan.h>
# include <array>
# define GLM_FORCE_RADIANS
# define GLM_FORCE_DEPTH_ZERO_TO_ONE
# include <glm/glm.hpp>

# include "api/vulkan/Swapchain.hpp"
# include "api/vulkan/Buffer.hpp"
# include "api/vulkan/Descriptors.hpp"
#include "HelExpected.hpp"

namespace	hel {

class	Window;
struct	RenderRequest;

struct	UserData {
	void		*data;
	uint32_t	bindingIndex;
};

struct	FrameContext {
	FrameContext(uint32_t frameIndex, std::vector<UserData> *globalData)
		: globalData(globalData) {
		this->frameIndex = frameIndex;
	};

	Window					*window{nullptr};
	RenderRequest			*request{nullptr};

	VkCommandBuffer			commandBuffer{VK_NULL_HANDLE};

	std::vector<UserData>	*globalData;
	VkDescriptorSet			globalSet;
	uint32_t				setStride;
	VkDescriptorSetLayout	globalLayout;

	DescriptorPool			*descriptorPool;

	uint32_t				passIndex{0};
	uint32_t				frameIndex{0};
	uint32_t				swapIndex{0};
};



struct	GlobalSetBindings {
	GlobalSetBindings	&addBinding(Buffer *buffer, uint32_t bindingIndex,
									VkDescriptorType descriptorType,
									VkShaderStageFlags stage);

	expected<GlobalSetBindings>	build(void);

	struct	Binding {
		uint32_t			index;
		VkDescriptorType	type;
		VkShaderStageFlags	stage;
		Buffer				*buffer;
		bool				dynamicBinding{false};
	};

	private:
		std::string								_error{""};
		std::unordered_map<uint32_t, Binding>	_bindings;
		bool	contains(uint32_t key);
		Binding	&operator[](uint32_t index);
	friend class	Frame;
};

class	Frame {
	public:
		Frame(void) = default;
		~Frame(void) = default;

		static constexpr uint32_t	MAX_PASS_COUNT = 32;

		tl::expected<void, std::string>	init(Device *device, const
											GlobalSetBindings &setConfig);

		void	fillContext(FrameContext &frameContext, Window *window);
		void	writeGlobalData(FrameContext &frameContext);

		static VkDescriptorSetLayout	getGlobalLayout(void)
			{ return _globalLayout; }

	private:
		Device	*_device;

		tl::expected<void, std::string>	createCommandBuffers(void);
		VkCommandPool							_commandPool{VK_NULL_HANDLE};
		std::array<VkCommandBuffer,
			Swapchain::MAX_FRAMES_IN_FLIGHT>	_commandBuffers{};

		void	writeToUBO(void *data, uint32_t bindingIndex,
						uint32_t passIndex, uint32_t frameIndex);
		tl::expected<void, std::string>	bindBuffers(void);
		tl::expected<void, std::string>	createGlobalSets(void);
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
